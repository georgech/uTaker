/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32L031_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************/


#if PIN_COUNT == PIN_COUNT_20_PIN                                        // TSSOP20
    #define _PIN_COUNT              0
#elif PIN_COUNT == PIN_COUNT_25_PIN                                      // WLCSP25
    #define _PIN_COUNT              1
#elif PIN_COUNT == PIN_COUNT_28_PIN                                      // UFQFPN28
    #define _PIN_COUNT              2
#elif PIN_COUNT == PIN_COUNT_32_PIN
    #if PACKAGE_TYPE ==  PACKAGE_LQFP
        #define _PIN_COUNT          4
    #else                                                                // UFQFPN32
        #define _PIN_COUNT          5
    #endif
#elif PIN_COUNT == PIN_COUNT_48_PIN
    #if PACKAGE_TYPE ==  PACKAGE_LQFP
        #define _PIN_COUNT          6
    #else                                                                // UFQFPN48
        #define _PIN_COUNT          7
    #endif
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS   8                                        // GPIO plus 8 possible peripheral functions

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH][8] = {
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO A
        {  "6",   "E5",  "6",   "6",   "6",   "6",   "10",  "10" },      // PA0
        {  "7",   "B4",  "7",   "7",   "7",   "7",   "11",  "11" },      // PA1
        {  "8",   "D4",  "8",   "8",   "8",   "8",   "12",  "12" },      // PA2
        {  "9",   "E4",  "9",   "9",   "9",   "9",   "13",  "13" },      // PA3
        {  "10",  "B3",  "10",  "10",  "10",  "10",  "14",  "14" },      // PA4
        {  "11",  "D3",  "11",  "11",  "11",  "11",  "15",  "15" },      // PA5
        {  "12",  "E3",  "12",  "12",  "12",  "12",  "16",  "16" },      // PA6
        {  "13",  "C3",  "13",  "13",  "13",  "13",  "17",  "17" },      // PA7
        {  "-",   "C1",  "18",  "18",  "18",  "18",  "29",  "29" },      // PA8
        {  "17",  "B1",  "19",  "19",  "19",  "19",  "30",  "30" },      // PA9
        {  "18",  "C2",  "20",  "20",  "20",  "20",  "31",  "31" },      // PA10
        {  "-",   "-",   "-",   "-",   "21",  "21",  "32",  "32" },      // PA11
        {  "-",   "-",   "-",   "-",   "22",  "22",  "33",  "33" },      // PA12
        {  "19",  "A1",  "21",  "21",  "23",  "23",  "34",  "34" },      // PA13
        {  "20",  "A2",  "22",  "22",  "24",  "24",  "37",  "37" },      // PA14
        {  "-",   "-",   "23",  "23",  "25",  "25",  "38",  "38" },      // PA15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO B
        {  "-",   "E2",  "14",  "14",  "14",  "14",  "18",  "18" },      // PB0
        {  "14",  "D2",  "15",  "15",  "15",  "15",  "19",  "19" },      // PB1
        {  "-",   "-",   "-",   "-",   "-",   "16",  "20",  "20" },      // PB2
        {  "-",   "B2",  "24",  "24",  "26",  "26",  "39",  "39" },      // PB3
        {  "-",   "-",   "-",   "25",  "27",  "27",  "40",  "40" },      // PB4
        {  "-",   "-",   "-",   "26",  "28",  "28",  "41",  "41" },      // PB5
        {  "-",   "A3",  "25",  "27",  "29",  "29",  "42",  "42" },      // PB6
        {  "-",   "A4",  "26",  "28",  "30",  "30",  "43",  "43" },      // PB7
        {  "-",   "-",   "-",   "-",   "-",   "32",  "45",  "45" },      // PB8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "46",  "46" },      // PB9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "21",  "21" },      // PB10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "22",  "22" },      // PB11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "25",  "25" },      // PB12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "26",  "26" },      // PB13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "27",  "27" },      // PB14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "28",  "28" },      // PB15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO C
        {  "-",   "-",   "-",   "-",   "-",   "-",   "1",   "1"  },      // PC0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PC12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "2",   "2"  },      // PC13
        {  "2",   "B5",  "2",   "2",   "2",   "2",   "2",   "3"  },      // PC14
        {  "3",   "C5",  "3",   "3",   "3",   "3",   "3",   "4"  },      // PC15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO D
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PD15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO E
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PE15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO F
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PF15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO G
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PG15
    },
    {
        //  TSSOP20, WLCSP25, UFQFPN28, UFQFPN28 (STM32L031GxUxS), LQFPN32, UFQFPN32, LQFPN48, UFQFPN48 GPIO H
        {  "-",   "-",   "-",   "-",   "-",   "-",   "5",   "5"  },      // PH0
        {  "-",   "-",   "-",   "-",   "-",   "-",   "6",   "6"  },      // PH1
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH2
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH3
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH4
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH5
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH6
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH7
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH8
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH9
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH10
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH11
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH12
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH13
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH14
        {  "-",   "-",   "-",   "-",   "-",   "-",   "-",   "-"  },      // PH15
    },
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][1 + ALTERNATIVE_FUNCTIONS] = {
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO A
        {  "PA0",      "-",           "LPTIM1_IN1","TIM2_CH1",  "-",         "USART2_CTS","TIM2_ETR",  "-",         "COMP1_OUT" }, // PA0
        {  "PA1",      "EVENTOUT",    "LPTIM1_IN2","TIM2_CH2",  "I2C1_SMBA", "USART2_RTS_DE","TIM21_ETR","-",       "-"         }, // PA1
        {  "PA2",      "TIM21_CH1",   "-",         "TIM2_CH3",  "-",         "USART2_TX", "-",         "LPUART1_TX","COMP2_OUT" }, // PA2
        {  "PA3",      "TIM21_CH2",   "-",         "TIM2_CH4",  "-",         "USART2_RX", "-",         "LPUART1_RX","-"         }, // PA3
        {  "PA4",      "SPI1_NSS",    "LPTIM1_IN1","-",         "-",         "USART2_CK", "TIM22_ETR", "-",         "-"         }, // PA4
        {  "PA5",      "SPI1_SCK",    "LPTIM1_IN2","TIM2_ETR",  "-",         "-",         "TIM2_CH1",  "-",         "-"         }, // PA5
        {  "PA6",      "SPI1_MISO",   "LPTIM1_ETR","-",         "-",         "LPUART1_CTS","TIM22_CH1","EVENTOUT",  "COMP1_OUT" }, // PA6
        {  "PA7",      "SPI1_MOSI",   "LPTIM1_OUT","-",         "-",         "USART2_CTS","TIM22_CH2", "EVENTOUT",  "COMP2_OUT" }, // PA7
        {  "PA8",      "MCO",         "-",         "LPTIM1_IN1","EVENTOUT",  "USART2_CK", "TIM22_CH1", "-",         "-"         }, // PA8
        {  "PA9",      "MCO",         "I2C1_SCL",  "-",         "-",         "USART2_TX", "TIM22_CH1", "-",         "-"         }, // PA9
        {  "PA10",     "-",           "I2C1_SDA",  "-",         "-",         "USART2_RX", "TIM22_CH2", "-",         "-"         }, // PA10
        {  "PA11",     "SPI1_MISO",   "-",         "EVENTOUT",  "-",         "USART2_CTS","TIM21_CH2", "-",         "COMP1_OUT" }, // PA11
        {  "PA12",     "SPI1_MOSI",   "-",         "EVENTOUT",  "-",         "USART2_RTS_DE","-",      "-",         "COMP2_OUT" }, // PA12
        {  "PA13",     "SWDIO",       "LPTIM1_ETR","-",         "-",         "-",         "-",         "LPUART1_RX","-"         }, // PA13
        {  "PA14",     "SWCLK",       "LPTIM1_OUT","-",         "I2C1_SMBA", "USART2_TX", "-",         "LPUART1_TX", "-"        }, // PA14
        {  "PA15",     "SPI1_NSS",    "-",         "TIM2_ETR",  "EVENTOUT",  "USART2_RX", "TIM2_CH1",  "-",         "-"         }  // PA15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO B
        {  "PB0",      "EVENTOUT",    "SPI1_MISO", "-",         "-",         "USART2_RTS_DE","TIM2_CH3","-",        "-"         }, // PB0
        {  "PB1",      "USART2_CK",   "SPI1_MOSI", "-",         "-",         "LPUART1_RTS_DE","TIM2_CH4","-",       "-"         }, // PB1
        {  "PB2",      "-",           "-",         "LPTIM1_OUT","-",         "-",         "-",         "-",         "-"         }, // PB2
        {  "PB3",      "SPI1_CLK",    "-",         "TIM2_CH2", "-",          "EVENTOUT",  "-",         "-",         "-"         }, // PB3
        {  "PB4",      "SPI1_MISO",   "-",         "EVENTOUT",  "-",         "TIM22_CH1", "-",         "-",         "-"         }, // PB4
        {  "PB5",      "SPI1_MOSI",   "-",         "LPTIM1_IN1","I2C1_SMBA", "TIM22_CH2", "-",         "-",         "-"         }, // PB5
        {  "PB6",      "USART2_TX",   "I2C1_SCL",  "LPTIM1_ETR","-",         "-",         "TIM21_CH1", "-",         "-"         }, // PB6
        {  "PB7",      "USART2_RX",   "I2C1_SDA",  "LPTIM1_IN2","-",         "-",         "-",         "-",         "-"         }, // PB7
        {  "PB8",      "-",           "-",         "-",         "-",         "I2C1_SCL",  "-",         "-",         "-"         }, // PB8
        {  "PB9",      "-",           "-",         "EVENTOUT",  "-",         "I2C1_SDA",  "-",         "-",         "-"         }, // PB9
        {  "PB10",     "-",           "-",         "TIM2_CH3",  "-",         "-",         "-",         "LPUART1_TX","-"         }, // PB10
        {  "PB11",     "EVENTOUT",    "-",         "TIM2_CH4",  "-",         "-",         "-",         "LPUART1_RX","-"         }, // PB11
        {  "PB12",     "SPI1_NSS",    "-",         "-",         "-",         "-",         "-",         "EVENTOUT",  "-"         }, // PB12
        {  "PB13",     "SPI1_SCK",    "-",         "MCO",       "-",         "-",         "TIM21_CH1", "LPUART1_CTS","-"        }, // PB13
        {  "PB14",     "SPI1_MISO",   "-",         "RTC_OUT",   "-",         "-",         "TIM21_CH2", "LPUART1_RTS_DE","-"     }, // PB14
        {  "PB15",     "SPI1_MOSI",   "-",         "RTC_REFIN", "-",         "-",         "-",         "-",         "-"         }  // PB15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO C
        {  "PC0",      "LPUART1_IN1", "-",         "EVENTOUT",  "-",         "-",         "-",         "LPUART1_RX","-"         }, // PC0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PC14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PC15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO D
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PD14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PD15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO E
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PE14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PE15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO F
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PF14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PF15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO G
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PG14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PG15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7               GPIO H
        {  "PH0",      "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH0
        {  "PH1",      "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }, // PH14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-"         }  // PH15
    }
};

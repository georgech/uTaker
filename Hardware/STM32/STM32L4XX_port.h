/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32L4XX_port.h [STM32L432]
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************/

#if defined _STM32L496
#if PIN_COUNT == PIN_COUNT_64_PIN
    #define _PIN_COUNT              0
#elif PIN_COUNT == PIN_COUNT_100_PIN
    #define _PIN_COUNT              1
#elif PIN_COUNT == PIN_COUNT_144_PIN
    #define _PIN_COUNT              2
#elif PIN_COUNT == PIN_COUNT_176_PIN
    #if PACKAGE_TYPE ==  PACKAGE_LQFP
        #define _PIN_COUNT          4
    #else
        #define _PIN_COUNT          3
    #endif
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS   16                                   // GPIO plus 16 possible peripheral functions

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][1 + ALTERNATIVE_FUNCTIONS + 1] = {
    {
    // PA0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15        Additional
    {  "PA0",    "-",      "TIM2_CH1/TIM2_ETR",   "TIM5_CH1",     "TIM8_ETR",     "-",           "-",              "-",            "USART2_CTS",     "UART4_TX",      "-",                  "-",          "ETH_MII_CRS",     "-",              "-",          "-",         "EVENTOUT", "ADC123_IN0/WKUP" }, // PA0
    {  "PA1",    "-",      "TIM2_CH2",            "TIM5_CH2",     "-",            "-",           "-",              "-",            "USART2_RTS",     "UART4_RX",      "-",                  "-",          "ETH_MII_RX_CLK/ETH_RMII_REF_CLK", "-","-",        "-",         "EVENTOUT", "ADC123_IN1" }, // PA1
    {  "PA2",    "-",      "TIM2_CH3",            "TIM5_CH3",     "TIM9_CH1",     "-",           "-",              "-",            "USART2_TX",      "-",             "-",                  "-",          "ETH_MDIO",        "-",              "-",          "-",         "EVENTOUT", "ADC123_IN2" }, // PA2
    {  "PA3",    "-",      "TIM2_CH4",            "TIM5_CH4",     "TIM9_CH2",     "-",           "-",              "-",            "USART2_RX",      "-",             "-",                  "OTG_HS_ULPI_D0","ETH_MII_COL",  "-",              "-",          "-",         "EVENTOUT", "ADC123_IN3" }, // PA3
    {  "PA4",    "-",      "-",                   "-",            "-",            "-",           "SPI1_NSS",       "SPI3_NSS/I2S3_WS", "USART2_CK",  "-",             "-",                  "-",          "-",               "OTG_HS_SOF",     "DCMI_HSYNC", "-",         "EVENTOUT", "ADC12_IN4/DAC1_OUT" }, // PA4
    {  "PA5",    "-",      "TIM2_CH1/TIM2_ETR",   "-",            "TIM8_CH1N",    "-",           "SPI1_SCK",       "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_CK", "-",           "-",              "-",          "-",         "EVENTOUT", "ADC12_IN5/DAC2_OUT" }, // PA5
    {  "PA6",    "-",      "TIM1_BKIN",           "TIM3_CH1",     "TIM8_BKIN",    "-",           "SPI1_MISO",      "-",            "-",              "-",             "TIM13_CH1",          "-",          "-",               "-",              "DCMI_PIXCK", "-",         "EVENTOUT", "ADC12_IN6" }, // PA6
    {  "PA7",    "-",      "TIM1_CH1N",           "TIM3_CH2",     "TIM8_CH1N",    "-",           "SPI1_MOSI",      "-",            "-",              "-",             "TIM14_CH1",          "-",          "ETH_MII_RX_DV/ETH_RMII_CRC_DV", "-","-",          "-",         "EVENTOUT", "ADC12_IN7" }, // PA7
    {  "PA8",    "MCO1",   "TIM1_CH1",            "-",            "-",            "I2C3_SCL",    "-",              "-",            "USART1_CK",      "-",             "-",                  "OTG_FS_SOF", "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA8
    {  "PA9",    "-",      "TIM1_CH2",            "-",            "-",            "I2C3_SMBA",   "-",              "-",            "USART1_TX",      "-",             "-",                  "OTG_FS_VBUS","-",               "-",              "DCMI_D0",    "-",         "EVENTOUT", "OTG_FS_VBUS" }, // PA9
    {  "PA10",   "-",      "TIM1_CH3",            "-",            "-",            "-",           "-",              "-",            "USART1_RX",      "-",             "-",                  "OTG_FS_ID",  "-",               "-",              "DCMI_D1",    "-",         "EVENTOUT", "-" }, // PA10    
    {  "PA11",   "-",      "TIM1_CH4",            "-",            "-",            "-",           "-",              "-",            "USART1_CTS",     "-",             "CAN1_RX",            "OTG_FS_DM",  "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA11
    {  "PA12",   "-",      "TIM1_ETR",            "-",            "-",            "-",           "-",              "-",            "USART1_RTS",     "-",             "CAN1_TX",            "OTG_FS_DP",  "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA12
    {  "PA13",   "JTMS-SWDIO", "-",               "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA13
    {  "PA14",   "JTMS-SWDIO", "-",               "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA14
    {  "PA15",   "JTDI",   "TIM2_CH1/TIM2_ETR",   "-",            "-",            "-",           "SPI1_NSS",       "SPI3_NSS/I2S3S_WS","-",          "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA15
    },
    {
    // PB0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PB0",    "-",      "TIM1_CH2N",           "TIM3_CH3",     "TIM8_CH2N",    "-",           "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_D1","ETH_MII_RXD2", "-",              "-",          "-",         "EVENTOUT", "ADC12_IN8" }, // PB0
    {  "PB1",    "-",      "TIM1_CH3N",           "TIM3_CH4",     "TIM8_CH3N",    "-",           "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_D2","ETH_MII_RXD3", "OTG_HS_INTN",    "-",          "-",         "EVENTOUT", "ADC12_IN9" }, // PB1
    {  "PB2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB2
    {  "PB3",    "JTDO/TRACESWO","TIM2_CH2",      "-",            "-",            "-",           "SPI1_SCK",       "SPI3_SCK/I2S3_CK", "-",          "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB3
    {  "PB4",    "JRST",   "-",                   "TIM3_CH1",     "-",            "-",           "SPI1_MISO",      "SPI3_MISO",    "I2S3ext_SD",     "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB4
    {  "PB5",    "-",      "-",                   "TIM3_CH2",     "-",            "I2C1_SMBA",   "SPI1_MOSI",      "SPI3_MOSI/I2S3_SD","-",          "-",             "CAN2_RX",            "OTG_HS_ULPI_D7", "ETH_PPS_OUT", "-",              "DCMI_D10",   "-",         "EVENTOUT", "-" }, // PB5
    {  "PB6",    "-",      "-",                   "TIM4_CH1",     "-",            "I2C1_SCL",    "I2S2_WS",        "-",            "USART1_TX",      "-",             "CAN2_TX",            "OTG_FS_INTN","-",               "-",              "DCMI_D5",    "-",         "EVENTOUT", "-" }, // PB6
    {  "PB7",    "-",      "-",                   "TIM4_CH2",     "-",            "I2C1_SDA",    "-",              "-",            "USART1_RX",      "-",             "-",                  "-",          "-",               "FSMC_NL",        "DCMI_VSYNC", "-",         "EVENTOUT", "-" }, // PB7
    {  "PB8",    "-",      "-",                   "TIM4_CH3",     "TIM10_CH1",    "I2C1_SCL",    "-",              "-",            "-",              "-",             "CAN1_RX",            "OTG_FS_SCL", "ETH_MII_TXD3",    "SDIO_D4",        "DCMI_D6",    "-",         "EVENTOUT", "-" }, // PB8
    {  "PB9",    "-",      "-",                   "TIM4_CH4",     "TIM11_CH1",    "I2C1_SDA",    "SPI2_NSS/I2S2_WS","-",           "-",              "-",             "CAN1_TX",            "OTG_FS_SDA", "-",               "SDIO_D5",        "DCMI_D7",    "-",         "EVENTOUT", "-" }, // PB9
    {  "PB10",   "-",      "TIM2_CH3",            "-",            "-",            "I2C2_SCL",    "SPI2_SCK/I2S2_CK","-",           "USART3_TX",      "-",             "-",                  "OTG_HS_ULPI_D3", "ETH_MII_RX_ER", "OTG_HS_SCL",   "-",          "-",         "EVENTOUT", "-" }, // PB10
    {  "PB11",   "-",      "TIM2_CH4",            "-",            "-",            "I2C2_SDA",    "-",              "-",            "USART3_RX",      "-",             "-",                  "OTG_HS_ULPI_D4", "ETH_MII_TX_EN/ETH_RMII_TX_EN",  "OTG_HS_SDA", "-","-",     "EVENTOUT", "-" }, // PB11
    {  "PB12",   "-",      "TIM1_BKIN",           "-",            "-",            "I2C2_SMBA",   "SPI2_NSS/I2S2_WS","-",           "USART3_CK",      "-",             "CAN2_RX",            "OTG_HS_ULPI_D5", "ETH_MII_TXD0/ETH_RMII_TXD0",    "OTG_HS_ID",  "-","-",     "EVENTOUT", "-" }, // PB12
    {  "PB13",   "-",      "TIM1_CH1N",           "-",            "-",            "-",           "SPI2_SCK/I2S2_CK","-",           "USART3_CTS",     "-",             "CAN2_TX",            "OTG_HS_ULPI_D6", "ETH_MII_TXD1/ETH_RMII_TXD1",    "-",          "-","-",     "EVENTOUT", "OTG_HS_VBUS" }, // PB13
    {  "PB14",   "-",      "TIM1_CH2N",           "-",            "TIM8_CH2N",    "-",           "SPI2_MISO",      "I2S2ext_SD",   "USART3_RTS",     "-",             "TIM12_CH1",          "-",          "-",               "OTG_HS_DM",      "-",          "-",         "EVENTOUT", "-" }, // PB14
    {  "PB15",   "RTC_50Hz","TIM1_CH3N",          "-",            "TIM8_CH3N",    "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "TIM12_CH2",          "-",          "-",               "OTG_HS_DP",      "-",          "-",         "EVENTOUT", "-" }, // PB15
    },
    {
    // PC0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PC0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_STP","-",           "-",              "-",          "-",         "EVENTOUT", "ADC123_IN10" }, // PC0
    {  "PC1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MDC",         "-",              "-",          "-",         "EVENTOUT", "ADC123_IN11" }, // PC1
    {  "PC2",    "-",      "-",                   "-",            "-",            "-",           "SPI2_MISO",      "I2S2ext_SD",   "-",              "-",             "-",                  "OTG_HS_ULPI_DIR","ETH_MII_TXD2","-",              "-",          "-",         "EVENTOUT", "ADC123_IN12" }, // PC2
    {  "PC3",    "-",      "-",                   "-",            "-",            "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "-",                  "OTG_HS_ULPI_NXT","ETH_MII_TX_CLK/ETH_RMII_TX_CLK","-", "-",     "-",         "EVENTOUT", "ADC123_IN13" }, // PC3
    {  "PC4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RXD0/ETH_RMII_RXD0", "-",   "-",          "-",         "EVENTOUT", "ADC12_IN14" }, // PC4
    {  "PC5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RXD1/ETH_RMII_RXD1", "-",   "-",          "-",         "EVENTOUT", "ADC12_IN15" }, // PC5
    {  "PC6",    "-",      "-",                   "TIM3_CH1",     "TIM8_CH1",     "-",           "I2S2_MCK",       "-",            "-",              "USART6_TX",     "-",                  "-",          "-",               "SDIO_D6",        "DCMI_D0",    "-",         "EVENTOUT", "-" }, // PC6
    {  "PC7",    "-",      "-",                   "TIM3_CH2",     "TIM8_CH2",     "-",           "-",              "I2S3_MCK",     "-",              "USART6_RX",     "-",                  "-",          "-",               "SDIO_D7",        "DCMI_D1",    "-",         "EVENTOUT", "-" }, // PC7
    {  "PC8",    "-",      "-",                   "TIM3_CH3",     "TIM8_CH3",     "-",           "-",              "-",            "-",              "USART6_CK",     "-",                  "-",          "-",               "SDIO_D0",        "DCMI_D2",    "-",         "EVENTOUT", "-" }, // PC8
    {  "PC9",    "MCO2",   "-",                   "TIM3_CH4",     "TIM8_CH4",     "I2C3_SDA",    "I2S2_CKIN",      "-",            "-",              "-",             "-",                  "-",          "-",               "SDIO_D1",        "DCMI_D3",    "-",         "EVENTOUT", "-" }, // PC9
    {  "PC10",   "-",      "-",                   "-",            "-",            "-",           "-",              "SPI3_SCK/I2S3S_CK","USART3_TX",  "USART4_TX",     "-",                  "-",          "-",               "SDIO_D2",        "DCMI_D8",    "-",         "EVENTOUT", "-" }, // PC10    
    {  "PC11",   "-",      "-",                   "-",            "-",            "-",           "-",              "SPI3_MISO/I2S3ext_SD","USART3_RX","USART4_RX",    "-",                  "-",          "-",               "SDIO_D3",        "DCMI_D4",    "-",         "EVENTOUT", "-" }, // PC11
    {  "PC12",   "-",      "-",                   "-",            "-",            "-",           "-",              "SPI3_MOSI/I2S3_SD", "USART3_CK", "USART5_TX",     "-",                  "-",          "-",               "SDIO_CK",        "DCIM_D9",    "-",         "EVENTOUT", "-" }, // PC12
    {  "PC13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "RTC_AF2" }, // PC13
    {  "PC14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "OSC32_IN" }, // PC14
    {  "PC15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "OSC32_OUT" }, // PC15
    },
    {
    // PD0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PD0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_RX",            "-",          "-",               "FSMC_D2",        "-",          "-",         "EVENTOUT", "-" }, // PD0
    {  "PD1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_TX",            "-",          "-",               "FSMC_D3",        "-",          "-",         "EVENTOUT", "-" }, // PD1
    {  "PD2",    "-",      "-",                   "TIM3_ETR",     "-",            "-",           "-",              "-",            "-",              "UART5_RX",      "-",                  "-",          "-",               "SDIO_CMD",       "DCMI_D11",   "-",         "EVENTOUT", "-" }, // PD2
    {  "PD3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_CTS",     "-",             "-",                  "-",          "-",               "FSMC_CLK",       "-",          "-",         "EVENTOUT", "-" }, // PD3
    {  "PD4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_RTS",     "-",             "-",                  "-",          "-",               "FSMC_NOE",       "-",          "-",         "EVENTOUT", "-" }, // PD4
    {  "PD5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_TX",      "-",             "-",                  "-",          "-",               "FSMC_NWE",       "-",          "-",         "EVENTOUT", "-" }, // PD5
    {  "PD6",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_RX",      "-",             "-",                  "-",          "-",               "FSMC_NWAIT",     "-",          "-",         "EVENTOUT", "-" }, // PD6
    {  "PD7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_CK",      "-",             "-",                  "-",          "-",               "FSMC_NE1/FSMC_NCE2","-",       "-",         "EVENTOUT", "-" }, // PD7
    {  "PD8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_TX",      "-",             "-",                  "-",          "-",               "FSMC_D13",       "-",          "-",         "EVENTOUT", "-" }, // PD8
    {  "PD9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_RX",      "-",             "-",                  "-",          "-",               "FSMC_D14",       "-",          "-",         "EVENTOUT", "-" }, // PD9
    {  "PD10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_CK",      "-",             "-",                  "-",          "-",               "FSMC_D15",       "-",          "-",         "-",        "-" }, // PD10
    {  "PD11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_CTS",     "-",             "-",                  "-",          "-",               "FSMC_A16",       "-",          "-",         "-",        "-" }, // PD11
    {  "PD12",   "-",      "-",                   "TIM4_CH1",     "-",            "-",           "-",              "-",            "USART3_RTS",     "-",             "-",                  "-",          "-",               "FSMC_A17",       "-",          "-",         "-",        "-" }, // PD12
    {  "PD13",   "-",      "-",                   "TIM4_CH2",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A18",       "-",          "-",         "-",        "-" }, // PD13
    {  "PD14",   "-",      "-",                   "TIM4_CH3",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D0",        "-",          "-",         "-",        "-" }, // PD14
    {  "PD15",   "-",      "-",                   "TIM4_CH4",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D1",        "-",          "-",         "-",        "-" }, // PD15
    },
    {
    // PE0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PE0",    "-",      "-",                   "TIM4_ETR",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_NBL0",      "DCMI_D2",    "-",         "EVENTOUT", "-" }, // PE0
    {  "PE1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_BLN1",      "DCMI_D3",    "-",         "EVENTOUT", "-" }, // PE1
    {  "PE2",    "TRACECLK","-",                  "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_TXD3",    "FSMC_A23",       "-",          "-",         "EVENTOUT", "-" }, // PE2
    {  "PE3",    "TRACED0","-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A19",       "-",          "-",         "EVENTOUT", "-" }, // PE3
    {  "PE4",    "TRACED1","-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A20",       "DCMI_D4",    "-",         "EVENTOUT", "-" }, // PE4
    {  "PE5",    "TRACED2","-",                   "-",            "TIM9_CH1",     "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A21",       "DCMI_D6",    "-",         "EVENTOUT", "-" }, // PE5
    {  "PE6",    "TRACED3","-",                   "-",            "TIM9_CH2",     "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A22",       "DCMI_D7",    "-",         "EVENTOUT", "-" }, // PE6
    {  "PE7",    "-",      "TIM1_ETR",            "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D4",        "-",          "-",         "EVENTOUT", "-" }, // PE7
    {  "PE8",    "-",      "TIM1_CH1N",           "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D5",        "-",          "-",         "EVENTOUT", "-" }, // PE8
    {  "PE9",    "-",      "TIM1_CH1",            "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D6",        "-",          "-",         "EVENTOUT", "-" }, // PE9
    {  "PE10",   "-",      "TIM1_CH2N",           "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D7",        "-",          "-",         "EVENTOUT", "-" }, // PE10
    {  "PE11",   "-",      "TIM1_CH2",            "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D8",        "-",          "-",         "EVENTOUT", "-" }, // PE11
    {  "PE12",   "-",      "TIM1_CH3N",           "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D9",        "-",          "-",         "EVENTOUT", "-" }, // PE12
    {  "PE13",   "-",      "TIM1_CH3",            "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D10",       "-",          "-",         "EVENTOUT", "-" }, // PE13
    {  "PE14",   "-",      "TIM1_CH4",            "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D11",       "-",          "-",         "EVENTOUT", "-" }, // PE14
    {  "PE15",   "-",      "TIM1_BKIN",           "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_D12",       "-",          "-",         "EVENTOUT", "-" }, // PE15
    },
    {
    // PF0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PF0",    "-",      "-",                   "-",            "-",            "I2C2_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A0",        "-",          "-",         "EVENTOUT", "-" }, // PF0
    {  "PF1",    "-",      "-",                   "-",            "-",            "I2C2_SCL",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A1",        "-",          "-",         "EVENTOUT", "-" }, // PF1
    {  "PF2",    "-",      "-",                   "-",            "-",            "I2C2_SMBA",   "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A2",        "-",          "-",         "EVENTOUT", "-" }, // PF2
    {  "PF3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A3",        "-",          "-",         "EVENTOUT", "ADC3_IN9" }, // PF3
    {  "PF4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A4",        "-",          "-",         "EVENTOUT", "ADC3_IN14" }, // PF4
    {  "PF5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A5",        "-",          "-",         "EVENTOUT", "ADC3_IN15" }, // PF5
    {  "PF6",    "-",      "-",                   "-",            "TIM10_CH1",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_NIOR",      "-",          "-",         "EVENTOUT", "ADC3_IN4" }, // PF6
    {  "PF7",    "-",      "-",                   "-",            "TIM11_CH1",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_NREG",      "-",          "-",         "EVENTOUT", "ADC3_IN5" }, // PF7
    {  "PF8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "TIM13_CH1",          "-",          "-",               "FSMC_NIOWR",      "-",         "-",         "EVENTOUT", "ADC3_IN6" }, // PF8
    {  "PF9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "TIM14_CH1",          "-",          "-",               "FSMC_CD",        "-",          "-",         "EVENTOUT", "ADC3_IN7" }, // PF9
    {  "PF10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_INTR",      "-",          "-",         "EVENTOUT", "ADC3_IN8" }, // PF10
    {  "PF11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D12",   "-",         "EVENTOUT", "-" }, // PF11
    {  "PF12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A6",        "-",          "-",         "EVENTOUT", "-" }, // PF12
    {  "PF13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A7",        "-",          "-",         "EVENTOUT", "-" }, // PF13
    {  "PF14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A8",        "-",          "-",         "EVENTOUT", "-" }, // PF14
    {  "PF15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A9",        "-",          "-",         "EVENTOUT", "-" }, // PF15
    },
    {
    // PG0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PG0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A10",       "-",          "-",         "EVENTOUT", "-" }, // PG0
    {  "PG1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A11",       "-",          "-",         "EVENTOUT", "-" }, // PG1
    {  "PG2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A12",       "-",          "-",         "EVENTOUT", "-" }, // PG2
    {  "PG3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A13",       "-",          "-",         "EVENTOUT", "-" }, // PG3
    {  "PG4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A14",       "-",          "-",         "EVENTOUT", "-" }, // PG4
    {  "PG5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_A15",       "-",          "-",         "EVENTOUT", "-" }, // PG5
    {  "PG6",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_INT2",      "-",          "-",         "EVENTOUT", "-" }, // PG6
    {  "PG7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_CK",     "-",                  "-",          "-",               "FSMC_INT3",      "-",          "-",         "EVENTOUT", "-" }, // PG7
    {  "PG8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_RTS",    "-",                  "-",          "ETH_PPS_OUT",     "-",              "-",          "-",         "EVENTOUT", "-" }, // PG8
    {  "PG9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_RX",     "-",                  "-",          "-",               "FSMC_NE2/FSMC_NCE3",  "-",     "-",         "EVENTOUT", "-" }, // PG9
    {  "PG10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FSMC_NE4_1/FSMC_NE3", "-",     "-",         "EVENTOUT", "-" }, // PG10
    {  "PG11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_TX_EN/ETH_RMII_TX_EN", "FSMC_NCE4_2", "-",  "-",         "EVENTOUT", "-" }, // PG11
    {  "PG12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_RTS",    "-",                  "-",          "-",               "FSMC_NE4",       "-",          "-",         "EVENTOUT", "-" }, // PG12
    {  "PG13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_CTS",    "-",                  "-",          "ETH_MII_TXD0/ETH_RMII_TXD0", "FSMC_A24", "-",     "-",         "EVENTOUT", "-" }, // PG13
    {  "PG14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_TX",     "-",                  "-",          "ETH_MII_TXD1/ETH_RMII_TXD1", "FSMC_A25", "-",     "-",         "EVENTOUT", "-" }, // PG14
    {  "PG15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_CTS",    "-",                  "-",          "-",               "-",              "DCMI_D13",   "-",         "EVENTOUT", "-" }, // PG15
    },
    {
    // PH0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PH0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "OSC_IN" }, // PH0
    {  "PH1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "OSC_OUT" }, // PH1
    {  "PH2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_CRS",     "-",              "-",          "-",         "EVENTOUT", "-" }, // PH2
    {  "PH3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_COL",     "-",              "-",          "-",         "EVENTOUT", "-" }, // PH3
    {  "PH4",    "-",      "-",                   "-",            "-",            "I2C2_SCL",    "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_NXT", "-",          "-",              "-",          "-",         "EVENTOUT", "-" }, // PH4
    {  "PH5",    "-",      "-",                   "-",            "-",            "I2C2_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PH5
    {  "PH6",    "-",      "-",                   "-",            "-",            "I2C2_SMBA",   "-",              "-",            "-",              "-",             "TIM12_CH1",          "-",          "ETH_MII_RXD2",    "-",              "-",          "-",         "EVENTOUT", "-" }, // PH6
    {  "PH7",    "-",      "-",                   "-",            "-",            "I2C3_SCL",    "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RXD3",    "-",              "-",          "-",         "EVENTOUT", "-" }, // PH7
    {  "PH8",    "-",      "-",                   "-",            "-",            "I2C3_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_HSYNC", "-",         "EVENTOUT", "-" }, // PH8
    {  "PH9",    "-",      "-",                   "-",            "-",            "I2C3_SMBA",   "-",              "-",            "-",              "-",             "TIM12_CH2",          "-",          "-",               "-",              "DCMI_D0",    "-",         "EVENTOUT", "-" }, // PH9
    {  "PH10",   "-",      "-",                   "TIM5_CH1/TIM5_ETR","-",        "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D1",    "-",         "EVENTOUT", "-" }, // PH10
    {  "PH11",   "-",      "-",                   "TIM5_CH2",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D2",    "-",         "EVENTOUT", "-" }, // PH11
    {  "PH12",   "-",      "-",                   "TIM5_CH3",     "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D3",    "-",         "EVENTOUT", "-" }, // PH12
    {  "PH13",   "-",      "-",                   "-",            "TIM8_CH1N",    "-",           "-",              "-",            "-",              "-",             "CAN1_TX",            "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PH13
    {  "PH14",   "-",      "-",                   "-",            "TIM8_CH2N",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D4",    "-",         "EVENTOUT", "-" }, // PH14
    {  "PH15",   "-",      "-",                   "-",            "TIM8_CH3N",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D11",   "-",         "EVENTOUT", "-" }, // PH15
    },
    {
    // PI0..15
    // port      sys       tim1/2                 Tim3/4/5        Tim8/9/10/11    I2C1/2/3       SPI1/2/I2S2ext    SPI3/I2S3ext    USART1/2/3        USART4/5/6       CAN1/2/Tim12/13/14    USB           ETH                FSMC/SDIO/OTG     DCMI          AF014        AF15    
    {  "PI0",    "-",      "-",                   "TIM5_CH4",     "-",            "-",           "SPI2_NSS/I2S2_WS","-",           "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D13",   "-",         "EVENTOUT", "-" }, // PI0
    {  "PI1",    "-",      "-",                   "-",            "-",            "-",           "SPI2_SCK/I2S2_CK","-",           "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D8",    "-",         "EVENTOUT", "-" }, // PI1
    {  "PI2",    "-",      "-",                   "-",            "TIM8_CH4",     "-",           "SPI2_MISO",      "I2Sext_SD",    "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D9",    "-",         "EVENTOUT", "-" }, // PI2
    {  "PI3",    "-",      "-",                   "-",            "TIM8_ETR",     "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D10",   "-",         "EVENTOUT", "-" }, // PI3
    {  "PI4",    "-",      "-",                   "-",            "TIM8_BKIN",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D5",    "-",         "EVENTOUT", "-" }, // PI4
    {  "PI5",    "-",      "-",                   "-",            "TIM8_CH1",     "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCIM_VSYNC", "-",         "EVENTOUT", "-" }, // PI5
    {  "PI6",    "-",      "-",                   "-",            "TIM8_CH2",     "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D6",    "-",         "EVENTOUT", "-" }, // PI6
    {  "PI7",    "-",      "-",                   "-",            "TIM8_CH3",     "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "DCMI_D7",    "-",         "EVENTOUT", "-" }, // PI7
    {  "PI8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "RTC_AF2" }, // PI8
    {  "PI9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_RX",            "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PI9
    {  "PI10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RX_ER",   "-",              "-",          "-",         "EVENTOUT", "-" }, // PI10
    {  "PI11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_DIR","-",           "-",              "-",          "-",         "EVENTOUT", "-" }, // PI11
    {  "PI12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PI12
    {  "PI13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PI13
    {  "PI14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PI14
    {  "PI15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PI15
    },
};



static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH][5] = {
    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO A
        {  "14",    "23",    "34",    "N3",   "40"   },                  // PA0
        {  "15",    "24",    "35",    "N2",   "41"   },                  // PA1
        {  "16",    "25",    "36",    "P2",   "42"   },                  // PA2
        {  "17",    "26",    "37",    "R2",   "47"   },                  // PA3
        {  "20",    "29",    "40",    "N4",   "50"   },                  // PA4
        {  "21",    "30",    "41",    "P4",   "51"   },                  // PA5
        {  "22",    "31",    "42",    "P3",   "52"   },                  // PA6
        {  "23",    "32",    "43",    "R3",   "53"   },                  // PA7
        {  "41",    "67",    "100",   "F15",  "119"  },                  // PA8
        {  "42",    "68",    "101",   "E15",  "120"  },                  // PA9
        {  "43",    "69",    "102",   "D15",  "121"  },                  // PA10
        {  "44",    "70",    "103",   "C15",  "122"  },                  // PA11
        {  "45",    "71",    "104",   "B15",  "123"  },                  // PA12
        {  "46",    "72",    "105",   "A15",  "124"  },                  // PA13
        {  "49",    "76",    "109",   "A14",  "137"  },                  // PA14
        {  "50",    "77",    "110",   "A13",  "138"  },                  // PA15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO B
        {  "26",    "35",    "46",    "R5",   "56"   },                  // PB0
        {  "27",    "36",    "47",    "R4",   "57"   },                  // PB1
        {  "28",    "37",    "48",    "M6",   "58"   },                  // PB2
        {  "55",    "89",    "133",   "A10",  "161"  },                  // PB3
        {  "56",    "90",    "134",   "A9",   "162"  },                  // PB4
        {  "57",    "91",    "135",   "A6",   "163"  },                  // PB5
        {  "58",    "92",    "136",   "B6",   "164"  },                  // PB6
        {  "59",    "93",    "137",   "B5",   "165"  },                  // PB7
        {  "61",    "95",    "139",   "A5",   "167"  },                  // PB8
        {  "62",    "96",    "140",   "B4",   "168"  },                  // PB9
        {  "29",    "47",    "69",    "R12",  "79"   },                  // PB10
        {  "30",    "48",    "70",    "R13",  "80"   },                  // PB11
        {  "33",    "51",    "73",    "P12",  "92"   },                  // PB12
        {  "34",    "52",    "74",    "P13",  "93"   },                  // PB13
        {  "35",    "53",    "75",    "R14",  "94"   },                  // PB14
        {  "36",    "54",    "76",    "R15",  "95"   },                  // PB15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO C
        {  "8",     "15",    "26",    "M2",   "32"   },                  // PC0
        {  "9",     "16",    "27",    "M3",   "33"   },                  // PC1
        {  "10",    "17",    "28",    "M4",   "34"   },                  // PC2
        {  "11",    "18",    "29",    "M5",   "35"   },                  // PC3
        {  "24",    "33",    "44",    "N5",   "54"   },                  // PC4
        {  "25",    "34",    "45",    "P5",   "55"   },                  // PC5
        {  "37",    "63",    "96",    "H15",  "115"  },                  // PC6
        {  "38",    "64",    "97",    "G15",  "116"  },                  // PC7
        {  "39",    "65",    "98",    "G14",  "117"  },                  // PC8
        {  "40",    "66",    "99",    "F14",  "118"  },                  // PC9
        {  "51",    "78",    "111",   "B14",  "139"  },                  // PC10
        {  "52",    "79",    "112",   "B13",  "140"  },                  // PC11
        {  "53",    "80",    "113",   "A12",  "141"  },                  // PC12
        {  "2",     "7",     "7",     "D1",   "8"    },                  // PC13
        {  "3",     "8",     "8",     "E1",   "9"    },                  // PC14
        {  "4",     "9",     "9",     "F1",   "10"   },                  // PC15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO D
        {  "-",     "81",    "114",   "B12",  "142"  },                  // PD0
        {  "-",     "82",    "115",   "C12",  "143"  },                  // PD1
        {  "54",    "83",    "116",   "D12",  "144"  },                  // PD2
        {  "-",     "84",    "117",   "D11",  "145"  },                  // PD3
        {  "-",     "85",    "118",   "D10",  "146"  },                  // PD4
        {  "-",     "86",    "119",   "C11",  "147"  },                  // PD5
        {  "-",     "87",    "122",   "B11",  "150"  },                  // PD6
        {  "-",     "88",    "123",   "A11",  "151"  },                  // PD7
        {  "-",     "55",    "77",    "P15",  "96"   },                  // PD8
        {  "-",     "56",    "78",    "P14",  "97"   },                  // PD9
        {  "-",     "57",    "79",    "N15",  "98"   },                  // PD10
        {  "-",     "58",    "80",    "N14",  "99"   },                  // PD11
        {  "-",     "59",    "81",    "N13",  "100"  },                  // PD12
        {  "-",     "60",    "82",    "M15",  "101"  },                  // PD13
        {  "-",     "61",    "85",    "M14",  "104"  },                  // PD14
        {  "-",     "62",    "86",    "L14",  "105"  },                  // PD15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO E
        {  "-",     "97",    "141",   "A4",   "169"  },                  // PE0
        {  "-",     "98",    "142",   "A3",   "170"  },                  // PE1
        {  "-",     "1",     "1",     "A2",   "1"    },                  // PE2
        {  "-",     "2",     "2",     "A1",   "2"    },                  // PE3
        {  "-",     "3",     "3",     "B1",   "3"    },                  // PE4
        {  "-",     "4",     "4",     "B2",   "4"    },                  // PE5
        {  "-",     "5",     "5",     "B3",   "5"    },                  // PE6
        {  "-",     "38",    "58",    "R8",   "68"   },                  // PE7
        {  "-",     "39",    "59",    "P8",   "69"   },                  // PE8
        {  "-",     "40",    "60",    "P9",   "70"   },                  // PE9
        {  "-",     "41",    "63",    "R9",   "73"   },                  // PE10
        {  "-",     "42",    "64",    "P10",  "74"   },                  // PE11
        {  "-",     "43",    "65",    "R10",  "75"   },                  // PE12
        {  "-",     "44",    "66",    "N11",  "76"   },                  // PE13
        {  "-",     "45",    "67",    "P11",  "77"   },                  // PE14
        {  "-",     "46",    "68",    "R11",  "78"   },                  // PE15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO F
        {  "-",     "-",     "10",    "E2",   "16"   },                  // PF0
        {  "-",     "-",     "11",    "H3",   "17"   },                  // PF1
        {  "-",     "-",     "12",    "H2",   "18"   },                  // PF2
        {  "-",     "-",     "13",    "J2",   "19"   },                  // PF3
        {  "-",     "-",     "14",    "J3",   "20"   },                  // PF4
        {  "-",     "-",     "15",    "K3",   "21"   },                  // PF5
        {  "-",     "-",     "18",    "K2",   "24"   },                  // PF6
        {  "-",     "-",     "19",    "K1",   "25"   },                  // PF7
        {  "-",     "-",     "20",    "L3",   "26"   },                  // PF8
        {  "-",     "-",     "21",    "L2",   "27"   },                  // PF9
        {  "-",     "-",     "22",    "L1",   "28"   },                  // PF10
        {  "-",     "-",     "49",    "R6",   "59"   },                  // PF11
        {  "-",     "-",     "50",    "P6",   "60"   },                  // PF12
        {  "-",     "-",     "53",    "N6",   "63"   },                  // PF13
        {  "-",     "-",     "54",    "R7",   "64"   },                  // PF14
        {  "-",     "-",     "55",    "P7",   "65"   },                  // PF15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO G
        {  "-",     "-",     "56",    "N7",   "66"   },                  // PG0
        {  "-",     "-",     "57",    "M7",   "67"   },                  // PG1
        {  "-",     "-",     "87",    "L15",  "106"  },                  // PG2
        {  "-",     "-",     "88",    "K15",  "107"  },                  // PG3
        {  "-",     "-",     "89",    "K14",  "108"  },                  // PG4
        {  "-",     "-",     "90",    "K13",  "109"  },                  // PG5
        {  "-",     "-",     "91",    "J15",  "110"  },                  // PG6
        {  "-",     "-",     "92",    "J14",  "111"  },                  // PG7
        {  "-",     "-",     "93",    "H14",  "112"  },                  // PG8
        {  "-",     "-",     "124",   "C10",  "152"  },                  // PG9
        {  "-",     "-",     "125",   "B10",  "153"  },                  // PG10
        {  "-",     "-",     "126",   "B9",   "154"  },                  // PG11
        {  "-",     "-",     "127",   "B8",   "155"  },                  // PG12
        {  "-",     "-",     "128",   "A8",   "156"  },                  // PG13
        {  "-",     "-",     "129",   "A7",   "157"  },                  // PG14
        {  "-",     "-",     "132",   "B7",   "160"  },                  // PG15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO H
        {  "5",     "12",    "23",    "G1",   "29"   },                  // PH0
        {  "6",     "13",    "24",    "H1",   "30"   },                  // PH1
        {  "-",     "-",     "-",     "F4",   "43"   },                  // PH2
        {  "-",     "-",     "-",     "G4",   "44"   },                  // PH3
        {  "-",     "-",     "-",     "H4",   "45"   },                  // PH4
        {  "-",     "-",     "-",     "J4",   "46"   },                  // PH5
        {  "-",     "-",     "-",     "M11",  "83"   },                  // PH6
        {  "-",     "-",     "-",     "N12",  "84"   },                  // PH7
        {  "-",     "-",     "-",     "M12",  "85"   },                  // PH8
        {  "-",     "-",     "-",     "M13",  "86"   },                  // PH9
        {  "-",     "-",     "-",     "L13",  "87"   },                  // PH10
        {  "-",     "-",     "-",     "L12",  "88"   },                  // PH11
        {  "-",     "-",     "-",     "K12",  "89"   },                  // PH12
        {  "-",     "-",     "-",     "E12",  "128"  },                  // PH13
        {  "-",     "-",     "-",     "E13",  "129"  },                  // PH14
        {  "-",     "-",     "-",     "D13",  "130"  },                  // PH15
    },

    {
        //  LQFP64  LQFP100  LQFP144  BGA176  LQFP176                    GPIO I
        {  "-",     "-",     "-",     "E14",  "131"  },                  // PI0
        {  "-",     "-",     "-",     "D14",  "132"  },                  // PI1
        {  "-",     "-",     "-",     "C14",  "133"  },                  // PI2
        {  "-",     "-",     "-",     "C13",  "134"  },                  // PI3
        {  "-",     "-",     "-",     "D4",   "173"  },                  // PI4
        {  "-",     "-",     "-",     "C4",   "174"  },                  // PI5
        {  "-",     "-",     "-",     "C3",   "175"  },                  // PI6
        {  "-",     "-",     "-",     "C2",   "176"  },                  // PI7
        {  "-",     "-",     "-",     "D2",   "7"    },                  // PI8
        {  "-",     "-",     "-",     "D3",   "11"   },                  // PI9
        {  "-",     "-",     "-",     "E3",   "12"   },                  // PI10
        {  "-",     "-",     "-",     "E4",   "13"   },                  // PI11
        {  "-",     "-",     "-",     "-",    "-"    },                  // PI12
        {  "-",     "-",     "-",     "-",    "-"    },                  // PI13
        {  "-",     "-",     "-",     "-",    "-"    },                  // PI14
        {  "-",     "-",     "-",     "-",    "-"    },                  // PI15
    },
};
#elif defined _STM32L432
#define _PIN_COUNT              0

#define ALTERNATIVE_FUNCTIONS   16                                       // GPIO plus 16 possible peripheral functions

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH][1] = {
    {
        //  UFQFPN32                                                     GPIO A
        {  "6"  },                                                       // PA0
        {  "7"  },                                                       // PA1
        {  "8"  },                                                       // PA2
        {  "9"  },                                                       // PA3
        {  "10"  },                                                      // PA4
        {  "11"  },                                                      // PA5
        {  "12"  },                                                      // PA6
        {  "13"  },                                                      // PA7
        {  "18"  },                                                      // PA8
        {  "19"  },                                                      // PA9
        {  "20"  },                                                      // PA10
        {  "21"  },                                                      // PA11
        {  "22"  },                                                      // PA12
        {  "23"  },                                                      // PA13
        {  "24"  },                                                      // PA14
        {  "25"  },                                                      // PA15
    },
    {
        //  UFQFPN32                                                     GPIO B
        {  "14"  },                                                      // PB0
        {  "15"  },                                                      // PB1
        {  "26"  },                                                      // PB2
        {  "27"  },                                                      // PB3
        {  "28"  },                                                      // PB4
        {  "29"  },                                                      // PB5
        {  "30"  },                                                      // PB6
        {  "-"  },                                                       // PB7
        {  "-"  },                                                       // PB8
        {  "-"  },                                                       // PB9
        {  "-"  },                                                       // PB10
        {  "-"  },                                                       // PB11
        {  "-"  },                                                       // PB12
        {  "-"  },                                                       // PB13
        {  "-"  },                                                       // PB14
        {  "-"  },                                                       // PB15
    },
    {
        //  UFQFPN32                                                     GPIO C
        {  "-"  },                                                       // PC0
        {  "-"  },                                                       // PC1
        {  "-"  },                                                       // PC2
        {  "-"  },                                                       // PC3
        {  "-"  },                                                       // PC4
        {  "-"  },                                                       // PC5
        {  "-"  },                                                       // PC6
        {  "-"  },                                                       // PC7
        {  "-"  },                                                       // PC8
        {  "-"  },                                                       // PC9
        {  "-"  },                                                       // PC10
        {  "-"  },                                                       // PC11
        {  "-"  },                                                       // PC12
        {  "-"  },                                                       // PC13
        {  "2"  },                                                       // PC14 - limited current sink only [3mA], max. speed 2MHz
        {  "3"  },                                                       // PC15 - limited current sink only [3mA], max. speed 2MHz
    },
    {
        //  UFQFPN32                                                     GPIO D
        {  "-"  },                                                       // PD0
        {  "-"  },                                                       // PD1
        {  "-"  },                                                       // PD2
        {  "-"  },                                                       // PD3
        {  "-"  },                                                       // PD4
        {  "-"  },                                                       // PD5
        {  "-"  },                                                       // PD6
        {  "-"  },                                                       // PD7
        {  "-"  },                                                       // PD8
        {  "-"  },                                                       // PD9
        {  "-"  },                                                       // PD10
        {  "-"  },                                                       // PD11
        {  "-"  },                                                       // PD12
        {  "-"  },                                                       // PD13
        {  "-"  },                                                       // PD14
        {  "-"  },                                                       // PD15
    },
    {
        //  UFQFPN32                                                     GPIO E
        {  "-"  },                                                       // PE0
        {  "-"  },                                                       // PE1
        {  "-"  },                                                       // PE2
        {  "-"  },                                                       // PE3
        {  "-"  },                                                       // PE4
        {  "-"  },                                                       // PE5
        {  "-"  },                                                       // PE6
        {  "-"  },                                                       // PE7
        {  "-"  },                                                       // PE8
        {  "-"  },                                                       // PE9
        {  "-"  },                                                       // PE10
        {  "-"  },                                                       // PE11
        {  "-"  },                                                       // PE12
        {  "-"  },                                                       // PE13
        {  "-"  },                                                       // PE14
        {  "-"  },                                                       // PE15
    },
    {
        //  UFQFPN32                                                     GPIO F
        {  "-"  },                                                       // PF0
        {  "-"  },                                                       // PF1
        {  "-"  },                                                       // PF2
        {  "-"  },                                                       // PF3
        {  "-"  },                                                       // PF4
        {  "-"  },                                                       // PF5
        {  "-"  },                                                       // PF6
        {  "-"  },                                                       // PF7
        {  "-"  },                                                       // PF8
        {  "-"  },                                                       // PF9
        {  "-"  },                                                       // PF10
        {  "-"  },                                                       // PF11
        {  "-"  },                                                       // PF12
        {  "-"  },                                                       // PF13
        {  "-"  },                                                       // PF14
        {  "-"  },                                                       // PF15
    },
    {
        //  UFQFPN32                                                     GPIO G
        {  "-"  },                                                       // PG0
        {  "-"  },                                                       // PG1
        {  "-"  },                                                       // PG2
        {  "-"  },                                                       // PG3
        {  "-"  },                                                       // PG4
        {  "-"  },                                                       // PG5
        {  "-"  },                                                       // PG6
        {  "-"  },                                                       // PG7
        {  "-"  },                                                       // PG8
        {  "-"  },                                                       // PG9
        {  "-"  },                                                       // PG10
        {  "-"  },                                                       // PG11
        {  "-"  },                                                       // PG12
        {  "-"  },                                                       // PG13
        {  "-"  },                                                       // PG14
        {  "-"  },                                                       // PG15
    },
    {
        //  UFQFPN32                                                     GPIO H
        {  "-"  },                                                       // PH0
        {  "-"  },                                                       // PH1
        {  "-"  },                                                       // PH2
        {  "32" },                                                       // PH3
        {  "-"  },                                                       // PH4
        {  "-"  },                                                       // PH5
        {  "-"  },                                                       // PH6
        {  "-"  },                                                       // PH7
        {  "-"  },                                                       // PH8
        {  "-"  },                                                       // PH9
        {  "-"  },                                                       // PH10
        {  "-"  },                                                       // PH11
        {  "-"  },                                                       // PH12
        {  "-"  },                                                       // PH13
        {  "-"  },                                                       // PH14
        {  "-"  },                                                       // PH15
    }
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][1 + ALTERNATIVE_FUNCTIONS] = {
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO A
        {  "PA0",      "-",           "TIM2_CH1",  "-",         "-",         "-",         "-",         "-",         "USART2_CTS","-",         "-",         "-",         "-",         "COMP1_OUT", "SAI1_EXTCLK","TIM2_ETR", "EVENTOUT" }, // PA0
        {  "PA1",      "-",           "TIM2_CH2",  "-",         "-",         "I2C1_SMBA", "SPI1_SCK",  "-",         "USART2_RTS_DE","-",      "-",         "-",         "-",         "-",         "-",         "TIM15_CH1N","EVENTOUT" }, // PA1
        {  "PA2",      "-",           "TIM2_CH3",  "-",         "-",         "-",         "-",         "-",         "USART2_TX", "LPUART1_TX","-",         "QUADSPI_BK1_NCS","-",    "COMP2_OUT", "-",         "TIM15_CH1", "EVENTOUT" }, // PA2
        {  "PA3",      "-",           "TIM2_CH4",  "-",         "-",         "-",         "-",         "-",         "USART2_RX", "LPUART1_RX","-",         "QUADSPI_CLK","-",        "-",         "SAI1_MCLK_A","TIM15_CH2","EVENTOUT" }, // PA3
        {  "PA4",      "-",           "-",         "-",         "-",         "-",         "SPI1_NSS",  "SPI3_NSS",  "USART2_CK", "-",         "-",         "-",         "-",         "-",         "SAI1_FS_B", "LPTIM2_OUT","EVENTOUT" }, // PA4
        {  "PA5",      "-",           "TIM2_CH1",  "TIM2_ETR",  "-",         "-",         "SPI1_SCK",  "-",         "-",         "-",         "-",         "-",         "-",         "-",         "SAI1_EXTCLK","LPTIM2_ETR","EVENTOUT"}, // PA5
        {  "PA6",      "-",           "TIM1_BKIN", "-",         "-",         "-",         "SPI1_MISO", "COMP1_OUT", "USART3_CTS","LPUART1_CTS","-",        "QUADSPI_BK1_IO3","-",    "TIM1_BKIN_COMP2","-",    "TIM16_CH1", "EVENTOUT" }, // PA6
        {  "PA7",      "-",           "TIM1_CH1N", "-",         "-",         "I2C3_SCL",  "SPI1_MOSI", "-",         "-",         "-",         "-",         "QUADSPI_BK1_IO2","-",    "COMP2_OUT", "-",         "-",         "EVENTOUT" }, // PA7
        {  "PA8",      "MCO",         "TIM1_CH1",  "-",         "-",         "-",         "-",         "-",         "USART1_CK", "-",         "-",         "-",         "-",         "SWPMI1_IO", "SAI1_SCK_A","LPTIM2_OUT","EVENTOUT" }, // PA8
        {  "PA9",      "-",           "TIM1_CH2",  "-",         "-",         "I2C1_SCL",  "-",         "-",         "USART1_TX", "-",         "-",         "-",         "-",         "-",         "SAI1_FS_A", "TIM15_BKIN","EVENTOUT" }, // PA9
        {  "PA10",     "-",           "TIM1_CH3",  "-",         "-",         "I2C1_SDA",  "-",         "-",         "USART1_RX", "-",         "-",         "USB_CRS_SYNC","-",       "-",         "SAI1_SD_A", "-",         "EVENTOUT" }, // PA10
        {  "PA11",     "-",           "TIM1_CH4",  "TIM1_BKIN2","-",         "-",         "SPI1_MISO", "COMP1_OUT", "USART1_CTS","-",         "CAN1_RX",   "USB_DM",    "-",         "TIM1_BKIN2_COMP1","-",   "-",         "EVENTOUT" }, // PA11
        {  "PA12",     "-",           "TIM1_ETR",  "-",         "-",         "-",         "SPI1_MOSI", "-",         "USART1_RTS_DE","-",      "CAN1_TX",   "USB_DP",    "-",         "-", "SAI1_EXTCLK",       "-",         "EVENTOUT" }, // PA12
        {  "PA13",     "JTMS-SWDIO",  "IR_OUT",    "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "USB_NOE",   "-",         "SWPMI1_TX", "SAI1_SD_B", "-",         "EVENTOUT" }, // PA13
        {  "PA14",     "JTCK-SWCLK",  "LPTIM1_OUT","-",         "-",         "I2C1_SMBA", "-",         "-",         "-",         "-",         "-",         "-",         "-",         "SWPMI1_RX", "SAI1_FS_B", "-",         "EVENTOUT" }, // PA14
        {  "PA15",     "JTDI",        "TIM2_CH1",  "TIM2_ETR",  "USART2_RX", "-",         "SPI1_NSS",  "SPI3_NSS",  "USART3_RTS_DE","-",      "TSC_G3_IO1","-",         "-",         "SWPMI1_SUSPEND", "-",    "-",         "EVENTOUT" }  // PA15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO B
        {  "PB0",      "-",           "TIM1_CH2N", "-",         "-",         "-",         "SPI1_NSS",  "-",         "USART3_CK", "-",         "-",         "-",         "-",         "COMP1_OUT", "SAI1_EXTCLK","-",        "EVENTOUT" }, // PB0
        {  "PB1",      "-",           "TIM1_CH3N", "-",         "-",         "-",         "-",         "-",         "USART3_RTS_DE","LPUART1_RTS_DE","-",  "-",         "-",         "-",         "-",         "LPTIM2_IN1","EVENTOUT" }, // PB1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "QUADSPI_BK1_IO1","-",    "-",         "-",         "-",         "EVENTOUT" }, // PB2
        {  "PB3",      "JTDO-TRACESWO","TIM2_CH2", "-",         "-",         "-",         "SPI1_SCK",  "SPI3_SCK",  "USART1_RTS_DE","-",      "-",         "QUADSPI_BK1_IO0","-",    "-",         "SAI1_SCK_B","-",         "EVENTOUT" }, // PB3
        {  "PB4",      "NJTRST",      "-",         "-",         "-",         "I2C3_SDA",  "SPI1_MISO", "SPI3_MOSI", "USART1_CTS","-",         "TSC_G2_IO1","-",         "-",         "-",         "SAI1_MCLK_B","-",        "EVENTOUT" }, // PB4
        {  "PB5",      "-",           "LPTIM1_IN1","-",         "-",         "I2C1_SMBA", "SPI1_MOSI", "SPI3_MISO", "USART1_CK", "-",         "TSC_G2_IO2","-",         "-",         "COMP2_OUT", "SAI1_SD_B", "TIM16_BKIN","EVENTOUT" }, // PB5
        {  "PB6",      "-",           "LPTIM1_ETR","-",         "-",         "I2C1_SCL",  "-",         "-",         "USART1_TX", "-",         "TSC_G2_IO3","-",         "-",         "-",         "SAI1_FS_B", "TIM16_CH1N","EVENTOUT" }, // PB6
        {  "PB7",      "-",           "LPTIM1_IN2","-",         "-",         "I2C1_SDA",  "-",         "-",         "USART1_RX", "-",         "TSC_G2_IO4","-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PB14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }  // PB15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO C
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PC13
        {  "PC14",     "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PC14
        {  "PC15",     "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }  // PC15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO D
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PD14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }  // PD15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO E
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PE14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }  // PE15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO F
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PF14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }  // PF15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO G
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG2
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PG14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }  // PG15
    },
    {
        //             AF0            AF1          AF2          AF3          AF4          AF5          AF6          AF7          AF8          AF9          AF10         AF11         AF12         AF13         AF14         AF15          GPIO H
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH0
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH1
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH2
        {  "PH3",      "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "EVENTOUT" }, // PH3
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH4
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH5
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH6
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH7
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH8
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH9
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH10
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH11
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH12
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH13
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }, // PH14
        {  "-",        "-",           "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-",         "-"        }  // PH15
    }
};
#endif
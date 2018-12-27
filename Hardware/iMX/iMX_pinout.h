/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      iMX_port_k80.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************/


#if PIN_COUNT == PIN_COUNT_100_PIN                                       // 100 pin LQFP
    #define _PIN_COUNT              0
#elif PIN_COUNT == PIN_COUNT_144_PIN
    #define _PIN_COUNT              1                                    // 144 pin LQFP
#else
    #error "Package not clearly defined!!"
#endif

static const char cPinDefaults[PORTS_AVAILABLE][PORT_WIDTH] = {
    { // GPIO1
        0,
        0,
        0,
        0,
        0,
        0,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    },
    { // GPIO2
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        6,
        6,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    },
    { // GPIO3
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    },
    { // GPIO4
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    },
    { // GPIO5
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    },
};

#define ALTERNATIVE_FUNCTIONS   7                                        // GPIO plus 7 possible peripheral/extra/system functions

static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][2] = {
    {
        //  100LQFP    144 LQFPP                    GPIO_AD_B0/1 [GPIO1]
        {  "78",       "111",    },                 // GPIO_AD_B0_00 [GPIO1.IO[0]] jtag_mux.TMS
        {  "77",       "110",    },                 // GPIO_AD_B0_01 [GPIO1.IO[1]] jtag_mux.TCK
        {  "76",       "109",    },                 // GPIO_AD_B0_02 [GPIO1.IO[2]] jtag_mux.MOD
        {  "75",       "108",    },                 // GPIO_AD_B0_03 [GPIO1.IO[3]] jtag_mux.TDI
        {  "74",       "107",    },                 // GPIO_AD_B0_04 [GPIO1.IO[4]] jtag_mux.TDO
        {  "74",       "106",    },                 // GPIO_AD_B0_05 [GPIO1.IO[5]] jtag_mux.TRSTB
        {  "72",       "105",     },                 // GPIO_AD_B0_06 [GPIO1.IO[6]]
        {  "68",       "101",     },                 // GPIO_AD_B0_07 [GPIO1.IO[7]]
        {  "67",       "100",     },                 // GPIO_AD_B0_08 [GPIO1.IO[8]]
        {  "66",       "99",     },                 // GPIO_AD_B0_09 [GPIO1.IO[9]]
        {  "65",       "98",     },                 // GPIO_AD_B0_10 [GPIO1.IO[10]]
        {  "64",       "97",     },                 // GPIO_AD_B0_11 [GPIO1.IO[11]]
        {  "63",       "96",    },                 // GPIO_AD_B0_12 [GPIO1.IO[12]]
        {  "62",       "95",    },                 // GPIO_AD_B0_13 [GPIO1.IO[13]]
        {  "61",       "94",    },                 // GPIO_AD_B0_14 [GPIO1.IO[14]]
        {  "60",       "93",   },                 // GPIO_AD_B0_15 [GPIO1.IO[15]]
        {  "-",        "92",   },                 // GPIO_AD_B1_00 [GPIO1.IO[16]]
        {  "-",        "91",   },                 // GPIO_AD_B1_01 [GPIO1.IO[17]]
        {  "-",        "90",   },                 // GPIO_AD_B1_02 [GPIO1.IO[18]]
        {  "-",        "89",   },                 // GPIO_AD_B1_03 [GPIO1.IO[19]]
        {  "-",        "88",     },                 // GPIO_AD_B1_04 [GPIO1.IO[20]]
        {  "-",        "87",     },                 // GPIO_AD_B1_05 [GPIO1.IO[21]]
        {  "-",        "84",     },                 // GPIO_AD_B1_06 [GPIO1.IO[22]]
        {  "-",        "83",     },                 // GPIO_AD_B1_07 [GPIO1.IO[23]]
        {  "-",        "82",     },                 // GPIO_AD_B1_08 [GPIO1.IO[24]]
        {  "-",        "81",     },                 // GPIO_AD_B1_09 [GPIO1.IO[25]]
        {  "57",       "80",     },                 // GPIO_AD_B1_10 [GPIO1.IO[26]]
        {  "56",       "79",     },                 // GPIO_AD_B1_11 [GPIO1.IO[27]]
        {  "55",       "78",     },                 // GPIO_AD_B1_12 [GPIO1.IO[28]]
        {  "54",       "76",     },                 // GPIO_AD_B1_13 [GPIO1.IO[29]]
        {  "53",       "75",     },                 // GPIO_AD_B1_14 [GPIO1.IO[30]]
        {  "52",       "74",     },                 // GPIO_AD_B1_15 [GPIO1.IO[31]]
    },
    {
        //  100LQFP    144 LQFP LCSP                    GPIO_EMC [GPIO2]
        {  "-",        "18",       },                 // GPIO_EMC_00 [GPIO2.IO[0]]
        {  "-",        "17",       },                 // GPIO_EMC_01 [GPIO2.IO[1]]
        {  "-",        "16",       },                 // GPIO_EMC_02 [GPIO2.IO[2]]
        {  "-",        "15",       },                 // GPIO_EMC_03 [GPIO2.IO[3]]
        {  "7",        "14",       },                 // GPIO_EMC_04 [GPIO2.IO[4]]
        {  "6",        "13",       },                 // GPIO_EMC_05 [GPIO2.IO[5]]
        {  "4",        "12",       },                 // GPIO_EMC_06 [GPIO2.IO[6]]
        {  "3",        "10",       },                 // GPIO_EMC_07 [GPIO2.IO[7]]
        {  "2",        "9",    },                 // GPIO_EMC_08 [GPIO2.IO[8]]
        {  "1",        "8",    },                 // GPIO_EMC_09 [GPIO2.IO[9]]
        {  "-",      "7",    },                 // GPIO_EMC_10 [GPIO2.IO[10]]
        {  "-",      "4",    },                 // GPIO_EMC_11 [GPIO2.IO[11]]
        {  "-",        "3",    },                 // GPIO_EMC_12 [GPIO2.IO[12]]
        {  "-",        "2",     },                 // GPIO_EMC_13 [GPIO2.IO[13]]
        {  "-",        "1",     },                 // GPIO_EMC_14 [GPIO2.IO[14]]
        {  "-",        "143",     },                 // GPIO_EMC_15 [GPIO2.IO[15]]
        {  "100",      "142",   },                 // GPIO_EMC_16 [GPIO2.IO[16]] src.BOOT_MODE[0]
        {  "99",       "141",   },                 // GPIO_EMC_17 [GPIO2.IO[17]] src.BOOT_MODE[1]
        {  "98",      "140",   },                 // GPIO_EMC_18 [GPIO2.IO[18]]
        {  "97",      "139",   },                 // GPIO_EMC_19 [GPIO2.IO[19]]
        {  "96",      "138",   },                 // GPIO_EMC_20 [GPIO2.IO[20]]
        {  "94",      "137",   },                 // GPIO_EMC_21 [GPIO2.IO[21]]
        {  "93",      "136",   },                 // GPIO_EMC_22 [GPIO2.IO[22]]
        {  "90",      "133",   },                 // GPIO_EMC_23 [GPIO2.IO[23]]
        {  "89",        "132", },                 // GPIO_EMC_24 [GPIO2.IO[24]]
        {  "88",        "131",  },                 // GPIO_EMC_25 [GPIO2.IO[25]]
        {  "87",        "130",  },                 // GPIO_EMC_26 [GPIO2.IO[26]]
        {  "86",        "129",  },                 // GPIO_EMC_27 [GPIO2.IO[27]]
        {  "-",        "128",  },                 // GPIO_EMC_28 [GPIO2.IO[28]]
        {  "-",        "127",  },                 // GPIO_EMC_29 [GPIO2.IO[29]]
        {  "-",        "126",  },                 // GPIO_EMC_30 [GPIO2.IO[30]]
        {  "-",        "125",  },                 // GPIO_EMC_31 [GPIO2.IO[31]]
    },
    {
        //  100LQFP    144 LQFPLCSP                    GPIO_EMC/GPIO_S [GPIO3]
        {  "85",       "124",      },                 // GPIO_EMC_32 [GPIO3.IO[0]]
        {  "84",       "123",      },                 // GPIO_EMC_33 [GPIO3.IO[1]]
        {  "83",       "122",      },                 // GPIO_EMC_34 [GPIO3.IO[2]]
        {  "82",      "121",      },                 // GPIO_EMC_35 [GPIO3.IO[3]]
        {  "-",      "120",      },                 // GPIO_EMC_36 [GPIO3.IO[4]]
        {  "-",      "119",      },                 // GPIO_EMC_37 [GPIO3.IO[5]]
        {  "-",      "118",      },                 // GPIO_EMC_38 [GPIO3.IO[6]]
        {  "-",      "117",      },                 // GPIO_EMC_39 [GPIO3.IO[7]]
        {  "-",      "116",      },                 // GPIO_EMC_40 [GPIO3.IO[8]]
        {  "-",      "115",  },                 // GPIO_EMC_41 [GPIO3.IO[9]]
        {  "-",      "-",  },                 //
        {  "-",      "-",  },                 //
        {  "-",      "-",  },                 //
        {  "-",      "48",  },                 // GPIO_SD_B0_00 [GPIO3.IO[13]]
        {  "-",       "47",  },                 // GPIO_SD_B0_01 [GPIO3.IO[14]]
        {  "-",       "46",  },                 // GPIO_SD_B0_02 [GPIO3.IO[15]]
        {  "-",       "45",       },                 // GPIO_SD_B0_03 [GPIO3.IO[16]]
        {  "-",       "43",       },                 // GPIO_SD_B0_04 [GPIO3.IO[17]]
        {  "-",       "42",       },                 // GPIO_SD_B0_05 [GPIO3.IO[18]]
        {  "-",       "41",       },                 // GPIO_SD_B0_06 [GPIO3.IO[19]]
        {  "22",        "33",  },                 // GPIO_SD_B1_00 [GPIO3.IO[20]]
        {  "21",        "32",  },                 // GPIO_SD_B1_01 [GPIO3.IO[21]]
        {  "17",        "30",  },                 // GPIO_SD_B1_02 [GPIO3.IO[22]]
        {  "16",        "28",  },                 // GPIO_SD_B1_03 [GPIO3.IO[23]]
        {  "15",        "27",  },                 // GPIO_SD_B1_04 [GPIO3.IO[24]]
        {  "14",        "26",  },                 // GPIO_SD_B1_05 [GPIO3.IO[25]]
        {  "13",        "25",  },                 // GPIO_SD_B1_06 [GPIO3.IO[26]]
        {  "12",        "24",  },                 // GPIO_SD_B1_07 [GPIO3.IO[27]]
        {  "11",        "23",  },                 // GPIO_SD_B1_08 [GPIO3.IO[28]]
        {  "10",        "22",  },                 // GPIO_SD_B1_09 [GPIO3.IO[29]]
        {  "9",        "21",  },                 // GPIO_SD_B1_10 [GPIO3.IO[30]]
        {  "8",        "19",  },                 // GPIO_SD_B1_11 [GPIO3.IO[31]]
    },
    {
        //  100LQFP   144LQFP                     GPIO D
        {  "-",       "-",    },                 // PD0
        {  "-",       "-",    },                 // PD1
        {  "-",       "-",    },                 // PD2
        {  "-",       "-",    },                 // PD3
        {  "-",       "-",    },                 // PD4
        {  "-",       "-",    },                 // PD5
        {  "-",       "-",    },                 // PD6
        {  "-",       "-",    },                 // PD7
        {  "-",       "-",    },                 // PD8
        {  "-",       "-",    },                 // PD9
        {  "-",       "-",    },                 // PD10
        {  "-",       "-",    },                 // PD11
        {  "-",       "-",    },                 // PD12
        {  "-",       "-",    },                 // PD13
        {  "-",       "-",    },                 // PD14
        {  "-",       "-",    },                 // PD15
        {  "-",        "-",     },                 // PD16
        {  "-",        "-",     },                 // PD17
        {  "-",        "-",     },                 // PD18
        {  "-",        "-",     },                 // PD19
        {  "-",        "-",     },                 // PD20
        {  "-",        "-",     },                 // PD21
        {  "-",        "-",     },                 // PD22
        {  "-",        "-",     },                 // PD23
        {  "-",        "-",     },                 // PD24
        {  "-",        "-",     },                 // PD25
        {  "-",        "-",     },                 // PD26
        {  "-",        "-",     },                 // PD27
        {  "-",        "-",     },                 // PD28
        {  "-",        "-",     },                 // PD29
        {  "-",        "-",     },                 // PD30
        {  "-",        "-",     },                 // PD31
    },
    {
        //  100LQFP   144LQFP                     [GPIO5]
        {  "-",        "-",     },                 // PE0
        {  "-",        "-",     },                 // PE1
        {  "-",        "-",     },                 // PE2
        {  "-",        "-",     },                 // PE3
        {  "-",        "-",     },                 // PE4
        {  "-",        "52",     },                 // PE5
        {  "-",        "-",     },                 // PE6
        {  "-",       "-",    },                 // PE7
        {  "-",       "-",    },                 // PE8
        {  "-",       "-",    },                 // PE9
        {  "-",       "-",    },                 // PE10
        {  "-",       "-",    },                 // PE11
        {  "-",       "-",     },                 // PE12
        {  "-",       "-",     },                 // PE13
        {  "-",        "-",     },                 // PE14
        {  "-",        "-",     },                 // PE15
        {  "-",       "-",     },                 // PE16
        {  "-",       "-",     },                 // PE17
        {  "-",       "-",     },                 // PE18
        {  "-",       "-",     },                 // PE19
        {  "-",        "-",     },                 // PE20
        {  "-",        "-",     },                 // PE21
        {  "-",        "-",     },                 // PE22
        {  "-",        "-",     },                 // PE23
        {  "-",        "-",     },                 // PE24
        {  "-",        "-",     },                 // PE25
        {  "-",        "-",     },                 // PE26
        {  "-",        "-",     },                 // PE27
        {  "-",        "-",     },                 // PE28
        {  "-",        "-",     },                 // PE29
        {  "-",        "-",     },                 // PE30
        {  "-",        "-",     },                 // PE31
    },
    { 
        //  100LQFP   144LQFP                      dedicated ADC pins
        {  "-",      "-",      },                    // ADC0_DP0
        {  "-",      "-",      },                    // ADC0_DM0
        {  "-",      "-",      },                    // ADC0_DP3
        {  "-",      "-",      },                    // ADC0_DM3
        {  "-",      "-",     },                    // VREFH
        {  "-",      "-",     },                    // VREFL
        {  "-",      "-",     },                    // ADC0_DP1
        {  "-",      "-",     },                    // ADC0_DM1
        {  "-",      "-",     },                    // VREF_OUT/CMP1_IN5/CMP0_IN5/ADC0_SE22
        {  "-",      "-",     },                    // DAC0_OUT/CMP1_IN3/ADC0_SE23
        {  "-",      "-",     },                    // RTC_WAKEUP_B
        {  "-",      "-",     },                    // XTAL32
        {  "-",      "-",     },                    // EXTAL32
        {  "-",      "-",     },                    // VBAT
        {  "-",      "-",     },                    // RESET_b
        {  "-",       "-",      },                    // ADC0_SE16
        {  "-",       "-",      },                    // VDDIO_E
        {  "-",      "-",     },                    // VDDIO_E
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
        {  "-",       "-",      },                    //
    }
};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ADC_DM1_SINGLE, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
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
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }, // GPIO1
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
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "SEMC_DATA00",  "QTIMER2_TIMER0","LPUART4_CTS_B","SPDIF_SR_CLK","LPSPI2_SCK","GPIO2_IO00","FLEXCAN1_TX","PIT_TRIGGER02"}, // pad GPIO_EMC_00 [GPIO2]
        {  "SEMC_DATA01",  "QTIMER2_TIMER1","LPUART4_RTS_B","SPDIF_OUT","LPSPI2_PCS0","GPIO2_IO01","FLEXCAN1_RX","PIT_TRIGGER03"}, // pad GPIO_EMC_01
        {  "SEMC_DATA02",  "QTIMER2_TIMER2","LPUART4_TX","SPDIF_LOCK","LPSPI2_SDO","GPIO2_IO02","LPI2C1_SCL","-"           }, // pad GPIO_EMC_02
        {  "SEMC_DATA03",  "QTIMER2_TIMER3","LPUART4_RX","SPDIF_EXT_CLK","LPSPI2_SDI","GPIO2_IO03","LPI2C1_SDA","-"        }, // pad GPIO_EMC_03
        {  "SEMC_DATA04",  "XBAR1_INOUT04","SPDIF_OUT","SAI2_TX_BCLK","FLEXIO1_FLEXIO16","GPIO2_IO04","-","-"              }, // pad GPIO_EMC_04
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
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }, // GPIO3
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
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }, // GPIO4
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
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "-",     "-",         "-",          "-",           "GPIO5_IO00","-",        "NMI_GLUE_NMI"      }, // pad WAKEUP [GPIO5]
        {  "SNVS_LP_PMIC_ON_REQ","-","-",        "-",          "-",           "GPIO5_IO01","-",        "-"                 }, // pad PMIC_ON_REQ
        {  "CCM_PMIC_VSTBY_REQ","-","-",         "-",          "-",           "GPIO5_IO02","-",        "-"                 }, // pad PMIC_STBY_REQ
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
};


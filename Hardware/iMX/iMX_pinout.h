/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      iMX_pinout.h
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

#if defined _PIN_DEFAULTS
static const unsigned char cPinDefaults[PORTS_AVAILABLE][PORT_WIDTH] = {
    {                                                                    // GPIO1
        0,
        0,
        0,
        0,
        0,
        0,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO
    },
    {                                                                    // GPIO2
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        GPIO_EMC_16_SRC_BOOT_MODE00,
        GPIO_EMC_17_SRC_BOOT_MODE01,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO
    },
    {                                                                    // GPIO3
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO
    },
    {                                                                    // GPIO4
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO
    },
    {                                                                    // GPIO5
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO,
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_GPIO
    },
};
#endif

#define ALTERNATIVE_FUNCTIONS   7                                        // GPIO plus 7 possible peripheral/extra/system functions

static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][2] = {
    {
        //  100LQFP    144 LQFPP                                         GPIO_AD_B0/1 [GPIO1]
        {  "78",       "111",    },                                      // GPIO_AD_B0_00 [GPIO1.IO[0]] jtag_mux.TMS
        {  "77",       "110",    },                                      // GPIO_AD_B0_01 [GPIO1.IO[1]] jtag_mux.TCK
        {  "76",       "109",    },                                      // GPIO_AD_B0_02 [GPIO1.IO[2]] jtag_mux.MOD
        {  "75",       "108",    },                                      // GPIO_AD_B0_03 [GPIO1.IO[3]] jtag_mux.TDI
        {  "74",       "107",    },                                      // GPIO_AD_B0_04 [GPIO1.IO[4]] jtag_mux.TDO
        {  "74",       "106",    },                                      // GPIO_AD_B0_05 [GPIO1.IO[5]] jtag_mux.TRSTB
        {  "72",       "105",    },                                      // GPIO_AD_B0_06 [GPIO1.IO[6]]
        {  "68",       "101",    },                                      // GPIO_AD_B0_07 [GPIO1.IO[7]]
        {  "67",       "100",    },                                      // GPIO_AD_B0_08 [GPIO1.IO[8]]
        {  "66",       "99",     },                                      // GPIO_AD_B0_09 [GPIO1.IO[9]]
        {  "65",       "98",     },                                      // GPIO_AD_B0_10 [GPIO1.IO[10]]
        {  "64",       "97",     },                                      // GPIO_AD_B0_11 [GPIO1.IO[11]]
        {  "63",       "96",     },                                      // GPIO_AD_B0_12 [GPIO1.IO[12]]
        {  "62",       "95",     },                                      // GPIO_AD_B0_13 [GPIO1.IO[13]]
        {  "61",       "94",     },                                      // GPIO_AD_B0_14 [GPIO1.IO[14]]
        {  "60",       "93",     },                                      // GPIO_AD_B0_15 [GPIO1.IO[15]]
        {  "-",        "92",     },                                      // GPIO_AD_B1_00 [GPIO1.IO[16]]
        {  "-",        "91",     },                                      // GPIO_AD_B1_01 [GPIO1.IO[17]]
        {  "-",        "90",     },                                      // GPIO_AD_B1_02 [GPIO1.IO[18]]
        {  "-",        "89",     },                                      // GPIO_AD_B1_03 [GPIO1.IO[19]]
        {  "-",        "88",     },                                      // GPIO_AD_B1_04 [GPIO1.IO[20]]
        {  "-",        "87",     },                                      // GPIO_AD_B1_05 [GPIO1.IO[21]]
        {  "-",        "84",     },                                      // GPIO_AD_B1_06 [GPIO1.IO[22]]
        {  "-",        "83",     },                                      // GPIO_AD_B1_07 [GPIO1.IO[23]]
        {  "-",        "82",     },                                      // GPIO_AD_B1_08 [GPIO1.IO[24]]
        {  "-",        "81",     },                                      // GPIO_AD_B1_09 [GPIO1.IO[25]]
        {  "57",       "80",     },                                      // GPIO_AD_B1_10 [GPIO1.IO[26]]
        {  "56",       "79",     },                                      // GPIO_AD_B1_11 [GPIO1.IO[27]]
        {  "55",       "78",     },                                      // GPIO_AD_B1_12 [GPIO1.IO[28]]
        {  "54",       "76",     },                                      // GPIO_AD_B1_13 [GPIO1.IO[29]]
        {  "53",       "75",     },                                      // GPIO_AD_B1_14 [GPIO1.IO[30]]
        {  "52",       "74",     },                                      // GPIO_AD_B1_15 [GPIO1.IO[31]]
    },
    {
        //  100LQFP    144 LQFP LCSP                    GPIO_EMC [GPIO2]
        {  "-",        "18",     },                 // GPIO_EMC_00 [GPIO2.IO[0]]
        {  "-",        "17",     },                 // GPIO_EMC_01 [GPIO2.IO[1]]
        {  "-",        "16",     },                 // GPIO_EMC_02 [GPIO2.IO[2]]
        {  "-",        "15",     },                 // GPIO_EMC_03 [GPIO2.IO[3]]
        {  "7",        "14",     },                 // GPIO_EMC_04 [GPIO2.IO[4]]
        {  "6",        "13",     },                 // GPIO_EMC_05 [GPIO2.IO[5]]
        {  "4",        "12",     },                 // GPIO_EMC_06 [GPIO2.IO[6]]
        {  "3",        "10",     },                 // GPIO_EMC_07 [GPIO2.IO[7]]
        {  "2",        "9",    },                 // GPIO_EMC_08 [GPIO2.IO[8]]
        {  "1",        "8",    },                 // GPIO_EMC_09 [GPIO2.IO[9]]
        {  "-",        "7",    },                 // GPIO_EMC_10 [GPIO2.IO[10]]
        {  "-",        "4",    },                 // GPIO_EMC_11 [GPIO2.IO[11]]
        {  "-",        "3",    },                 // GPIO_EMC_12 [GPIO2.IO[12]]
        {  "-",        "2",     },                 // GPIO_EMC_13 [GPIO2.IO[13]]
        {  "-",        "1",     },                 // GPIO_EMC_14 [GPIO2.IO[14]]
        {  "-",        "143",     },                 // GPIO_EMC_15 [GPIO2.IO[15]]
        {  "100",      "142",   },                 // GPIO_EMC_16 [GPIO2.IO[16]] src.BOOT_MODE[0]
        {  "99",       "141",   },                 // GPIO_EMC_17 [GPIO2.IO[17]] src.BOOT_MODE[1]
        {  "98",       "140",   },                 // GPIO_EMC_18 [GPIO2.IO[18]]
        {  "97",       "139",   },                 // GPIO_EMC_19 [GPIO2.IO[19]]
        {  "96",       "138",   },                 // GPIO_EMC_20 [GPIO2.IO[20]]
        {  "94",       "137",   },                 // GPIO_EMC_21 [GPIO2.IO[21]]
        {  "93",       "136",   },                 // GPIO_EMC_22 [GPIO2.IO[22]]
        {  "90",       "133",   },                 // GPIO_EMC_23 [GPIO2.IO[23]]
        {  "89",       "132", },                 // GPIO_EMC_24 [GPIO2.IO[24]]
        {  "88",       "131",  },                 // GPIO_EMC_25 [GPIO2.IO[25]]
        {  "87",       "130",  },                 // GPIO_EMC_26 [GPIO2.IO[26]]
        {  "86",       "129",  },                 // GPIO_EMC_27 [GPIO2.IO[27]]
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
        {  "SEMC_DATA00",  "QTIMER2_TIMER0","LPUART4_CTS_B","SPDIF_SR_CLK","LPSPI2_SCK","GPIO2_IO00","FLEXCAN1_TX","PIT_TRIGGER02"}, // pad GPIO_EMC_00 [GPIO2-0]
        {  "SEMC_DATA01",  "QTIMER2_TIMER1","LPUART4_RTS_B","SPDIF_OUT","LPSPI2_PCS0","GPIO2_IO01","FLEXCAN1_RX","PIT_TRIGGER03"}, // pad GPIO_EMC_01 [GPIO2-1]
        {  "SEMC_DATA02",  "QTIMER2_TIMER2","LPUART4_TX","SPDIF_LOCK","LPSPI2_SDO","GPIO2_IO02","LPI2C1_SCL","-"           }, // pad GPIO_EMC_02 [GPIO2-2]
        {  "SEMC_DATA03",  "QTIMER2_TIMER3","LPUART4_RX","SPDIF_EXT_CLK","LPSPI2_SDI","GPIO2_IO03","LPI2C1_SDA","-"        }, // pad GPIO_EMC_03 [GPIO2-3]
        {  "SEMC_DATA04",  "XBAR1_INOUT04","SPDIF_OUT","SAI2_TX_BCLK","FLEXIO1_FLEXIO16","GPIO2_IO04","-","-"              }, // pad GPIO_EMC_04 [GPIO2-4]
        {  "SEMC_DATA05",  "XBAR1_INOUT05","SPDIF_IN","SAI2_TX_SYNC","FLEXIO1_FLEXIO17","GPIO2_IO05","-","-"               }, // pad GPIO_EMC_05 [GPIO2-5]
        {  "SEMC_DATA06",  "XBAR1_INOUT06","LPUART3_TX","SAI2_TX_DATA","FLEXIO1_FLEXIO18","GPIO2_IO06","-","-"             }, // pad GPIO_EMC_06 [GPIO2-6]
        {  "SEMC_DATA07",  "XBAR1_INOUT07","LPUART3_RX","SAI2_RX_SYNC","FLEXIO1_FLEXIO19","GPIO2_IO07","-","-"             }, // pad GPIO_EMC_07 [GPIO2-7]
        {  "SEMC_DM00",    "XBAR1_INOUT08","FLEXCAN2_TX","SAI2_RX_DATA","FLEXIO1_FLEXIO20","GPIO2_IO08","-","-"            }, // pad GPIO_EMC_08 [GPIO2-8]
        {  "SEMC_WE",      "XBAR1_INOUT09","FLEXCAN2_RX","SAI2_RX_BCLK","FLEXIO1_FLEXIO21","GPIO2_IO09","-","-"            }, // pad GPIO_EMC_09 [GPIO2-9]
        {  "SEMC_CAS",     "XBAR1_INOUT10","LPI2C4_SDA","SAI1_TX_SYNC","LPSPI2_SCK","GPIO2_IO10","FLEXPWM2_PWMX00","-"     }, // pad GPIO_EMC_10 [GPIO2-10]
        {  "SEMC_RAS",     "XBAR1_INOUT11","LPI2C4_SCL","SAI1_TX_BCLK","LPSPI2_PCS0","GPIO2_IO11","FLEXPWM2_PWMX01","-"    }, // pad GPIO_EMC_11 [GPIO2-11]
        {  "SEMC_CS0",     "XBAR1_INOUT12","LPUART6_TX","SAI1_TX_DATA00","LPSPI2_SDO","GPIO2_IO12","FLEXPWM2_PWMX02","-"   }, // pad GPIO_EMC_12 [GPIO2-12]
        {  "SEMC_BA0",     "XBAR1_INOUT13","LPUART6_RX","SAI1_RX_DATA00","LPSPI2_SDI","GPIO2_IO13","FLEXPWM2_PWMX03","CCM_PMIC_RDY"}, // pad GPIO_EMC_13 [GPIO2-13]
        {  "SEMC_BA1",     "XBAR1_INOUT14","LPUART6_CTS_B","SAI1_RX_BCLK","LPSPI2_PCS1","GPIO2_IO14","FLEXCAN1_TX","-"     }, // pad GPIO_EMC_14 [GPIO2-14]
        {  "SEMC_ADDR10",  "XBAR1_INOUT15","LPUART6_RTS_B","SAI1_RX_SYNC","WDOG1_B","GPIO2_IO15","FLEXCAN1_RX","-"         }, // pad GPIO_EMC_15 [GPIO2-15]
        {  "SEMC_ADDR00",  "-",     "MQS_RIGHT", "SAI2_MCLK",  "-",           "GPIO2_IO16","SRC_BOOT_MODE00","-"           }, // pad GPIO_EMC_16 [GPIO2-16]
        {  "SEMC_ADDR01",  "-",     "MQS_LEFT",  "SAI3_MCLK",  "-",           "GPIO2_IO17","SRC_BOOT_MODE01","-"           }, // pad GPIO_EMC_17 [GPIO2-17]
        {  "SEMC_ADDR02",  "XBAR1_INOUT16","LPI2C2_SDA","SAI1_RX_SYNC","FLEXIO1_FLEXIO22","GPIO2_IO18","SRC_BT_CFG00","-"  }, // pad GPIO_EMC_18 [GPIO2-18]
        {  "SEMC_ADDR03",  "XBAR1_INOUT17","LPI2C2_SCL","SAI1_RX_BCLK","FLEXIO1_FLEXIO23","GPIO2_IO19","SRC_BT_CFG01","-"  }, // pad GPIO_EMC_19 [GPIO2-19]
        {  "SEMC_ADDR04",  "FLEXPWM1_PWMA03","LPUART2_CTS_B","SAI1_MCLK","FLEXIO1_FLEXIO24","GPIO2_IO20","SRC_BT_CFG02","-"}, // pad GPIO_EMC_20 [GPIO2-20]
        {  "SEMC_ADDR05",  "FLEXPWM1_PWMB03","LPUART2_RTS_B","SAI1_RX_DATA00","FLEXIO1_FLEXIO25","GPIO2_IO21","SRC_BT_CFG03","-"}, // pad GPIO_EMC_21 [GPIO2-21]
        {  "SEMC_ADDR06",  "FLEXPWM1_PWMA02","LPUART2_TX","SAI1_TX_DATA03","FLEXIO1_FLEXIO26","GPIO2_IO22","SRC_BT_CFG04","-"}, // pad GPIO_EMC_22 [GPIO2-22]
        {  "SEMC_ADDR07",  "FLEXPWM1_PWMB02","LPUART2_RX","SAI1_TX_DATA02","FLEXIO1_FLEXIO27","GPIO2_IO23","SRC_BT_CFG05","-"}, // pad GPIO_EMC_23 [GPIO2-23]
        {  "SEMC_ADDR08",  "FLEXPWM1_PWMA01","LPUART8_CTS_B","SAI1_TX_DATA01","FLEXIO1_FLEXIO28","GPIO2_IO24","SRC_BT_CFG06","-"}, // pad GPIO_EMC_24 [GPIO2-24]
        {  "SEMC_ADDR09",  "FLEXPWM1_PWMB01","LPUART8_RTS_B","SAI1_TX_DATA00","FLEXIO1_FLEXIO29","GPIO2_IO25","SRC_BT_CFG07","-"}, // pad GPIO_EMC_25 [GPIO2-25]
        {  "SEMC_ADDR11",  "FLEXPWM1_PWMA00","LPUART8_TX","SAI1_TX_BCLK","FLEXIO1_FLEXIO30","GPIO2_IO26","SRC_BT_CFG08","-"}, // pad GPIO_EMC_26 [GPIO2-26]
        {  "SEMC_ADDR12",  "FLEXPWM1_PWMB00","LPUART8_RX","SAI1_TX_SYNC","FLEXIO1_FLEXIO31","GPIO2_IO27","SRC_BT_CFG09","-"}, // pad GPIO_EMC_27 [GPIO2-27]
        {  "SEMC_DQS",     "FLEXPWM2_PWMA03","XBAR1_INOUT18","SAI3_MCLK","EWM_OUT_B","GPIO2_IO28","GPT2_CAPTURE2","FLEXPWM1_PWMX00"}, // pad GPIO_EMC_28 [GPIO2-28]
        {  "SEMC_CKE",     "FLEXPWM2_PWMB03","XBAR1_INOUT19","SAI3_RX_BCLK","WDOG2_RST_B_DEB","GPIO2_IO29","GPT2_COMPARE2","FLEXPWM1_PWMX01"}, // pad GPIO_EMC_29 [GPIO2-29]
        {  "SEMC_CLK",     "FLEXPWM2_PWMA02","LPUART4_CTS_B","SAI3_RX_SYNC","WDOG1_RST_B_DEB","GPIO2_IO30","GPT2_COMPARE3","FLEXPWM1_PWMX02"}, // pad GPIO_EMC_30 [GPIO2-30]
        {  "SEMC_DM01",    "FLEXPWM2_PWMB02","LPUART4_RTS_B","SAI3_RX_DATA","WDOG2_B","GPIO2_IO31","GPT2_CLK","FLEXPWM1_PWMX03"}  // pad GPIO_EMC_31 [GPIO2-31]
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "SEMC_DATA12",  "QTIMER1_TIMER0","LPUART4_TX","SAI3_TX_DATA","LPSPI4_SCK","GPIO3_IO00","-", "REF_24M_OUT"       }, // pad GPIO_EMC_32 [GPIO3-0]
        {  "SEMC_DATA09",  "QTIMER1_TIMER1","LPUART4_RX","SAI3_TX_BCLK","LPSPI4_PCS0","GPIO3_IO01","-","-"                 }, // pad GPIO_EMC_33 [GPIO3-1]
        {  "SEMC_DATA10",  "QTIMER1_TIMER2","LPUART7_TX","SAI3_TX_SYNC","LPSPI4_SDO","GPIO3_IO02","ENET_CRS","-"           }, // pad GPIO_EMC_34 [GPIO3-2]
        {  "SEMC_DATA11",  "QTIMER1_TIMER3","LPUART7_RX","USDHC2_WP","LPSPI4_SDI","GPIO3_IO03","ENET_COL","-"              }, // pad GPIO_EMC_35 [GPIO3-3]
        {  "SEMC_DATA12",  "FLEXPWM2_PWMA01","LPUART5_CTS_B","CCM_PMIC_RDY","LPSPI4_PCS1","GPIO3_IO04","ENET_RX_CLK","USDHC1_WP"}, // pad GPIO_EMC_36 [GPIO3-4]
        {  "SEMC_DATA13",  "FLEXPWM2_PWMB01","LPUART5_RTS_B","MQS_RIGHT","LPSPI4_PCS2","GPIO3_IO05","ENET_RX_CLK","USDHC1_VSELECT"}, // pad GPIO_EMC_37 [GPIO3-5]
        {  "SEMC_DATA14",  "FLEXPWM2_PWMA00","LPUART5_TX","MQS_LEFT","LPSPI4_PCS3","GPIO3_IO06","ENET_RDATA02","USDHC1_CD_B"}, // pad GPIO_EMC_38 [GPIO3-6]
        {  "SEMC_DATA15",  "FLEXPWM2_PWMB00","LPUART5_RX","USB_OTG1_OC","WDOG1_B","GPIO3_IO07","ENET_TX_ER","GPT1_CLK"     }, // pad GPIO_EMC_39 [GPIO3-7]
        {  "SEMC_CSX00",   "XBAR1_INOUT18","SPDIF_OUT","USB_OTG1_ID","ENET_MDIO","GPIO3_IO08","ENET_TDATA03","GPT1_COMPARE3"}, // pad GPIO_EMC_40 [GPIO3-8]
        {  "SEMC_READY",   "XBAR1_INOUT19","SPDIF_IN","USB_OTG1_PWR","ENET_MDC","GPIO3_IO09","ENET_TDATA02","GPT1_COMPARE2"}, // pad GPIO_EMC_41 [GPIO3-9]
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


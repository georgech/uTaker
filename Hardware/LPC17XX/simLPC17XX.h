/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    simLPC17XX.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    02.11.2011 Add LPC1788 support
    19.11.2011 Revise LPC17XX_SCM struct                                 {1}
    06.10.2012 Add ADC                                                   {2}
    07.10.2012 Add SD card interface                                     {3}
    15.11.2012 Add LPC17XX_USB_1788                                      {4}

*/  


typedef struct stLPC17XX_FAST_GPIO
{
unsigned long FIODIR;
unsigned long res1[3];                      
unsigned long FIOMASK;                     
unsigned long FIOPIN;                      
unsigned long FIOSET;                      
unsigned long FIOCLR;                      
} LPC17XX_FAST_GPIO;


typedef struct stLPC17XX_WATCHDOG
{
unsigned long WDMOD;                      
unsigned long WDTC;                     
unsigned long WDFEED;                      
unsigned long WDTV;
#if defined LPC1788
    unsigned long ulRes0;
    unsigned long WDWARNINT;
    unsigned long WDWINDOW;
#else                      
    unsigned long WDCLKSEL;                      
#endif
} LPC17XX_WATCHDOG;


typedef struct stLPC17XX_TIMER
{
unsigned long TIR;        
unsigned long TTCR;       
unsigned long TTC;        
unsigned long TPR;        
unsigned long TPC;        
unsigned long TMCR;       
unsigned long TMR0;       
unsigned long TMR1;       
unsigned long TMR2;       
unsigned long TMR3;       
unsigned long TCCR;       
unsigned long TCR0;       
unsigned long TCR1;       
unsigned long TCR2;       
unsigned long TCR3;       
unsigned long TEMR;       
unsigned long ulRes1[12];
unsigned long TCTCR;
} LPC17XX_TIMER;

typedef struct stLPC17XX_UART
{
unsigned long RBR_THR_DLL;
unsigned long DLM_IER;
unsigned long IIR_FCR;
unsigned long LCR;
unsigned long U1MCR;                                                     // only UART1
unsigned long LSR;
unsigned long U1MSR;                                                     // only UART1
unsigned long SCR;
unsigned long ACR;
unsigned long U3ICR;                                                     // only UART3
unsigned long FDR;
unsigned long ulRes4;
unsigned long TER;
} LPC17XX_UART;

typedef struct stLPC17XX_SSP
{
unsigned long SSPCR0; 
unsigned long SSPCR1;
unsigned long SSPDR;
unsigned long SSPSR;
unsigned long SSPCPSR;
unsigned long SSPIMSC;
unsigned long SSPRIS;
unsigned long SSPMIS;
unsigned long SSPICR;
unsigned long SSPDMACR;
} LPC17XX_SSP;


typedef struct stLPC17XX_ADC                                             // {2}
{
unsigned long AD0CR;
unsigned long AD0GDR;
unsigned long ulRes0;
unsigned long AD0INTEN;
unsigned long AD0DR0;
unsigned long AD0DR1;
unsigned long AD0DR2;
unsigned long AD0DR3;
unsigned long AD0DR4;
unsigned long AD0DR5;
unsigned long AD0DR6;
unsigned long AD0DR7;
unsigned long AD0STAT;
} LPC17XX_ADC;


typedef struct stLPC17XX_IIC
{
unsigned long I2CCONSET;
unsigned long I2CSTAT;
unsigned long I2CDAT;   
unsigned long I2CADR;   
unsigned long I2CSCLH;  
unsigned long I2CSCLL;  
unsigned long I2CCONCLR;
} LPC17XX_IIC;


typedef struct stLPC17XX_RIT
{
unsigned long RICOMPVAL;
unsigned long RIMASK;
unsigned long RICTRL;   
unsigned long RICOUNTER;   
} LPC17XX_RIT;


typedef struct stLPC17XX_RTC
{
unsigned long RTC_ILR;
unsigned long RTC_CTC;
unsigned long RTC_CCR;   
unsigned long RTC_CIIR;   
unsigned long RTC_AMR;  
unsigned long RTC_CTIME0;  
unsigned long RTC_CTIME1;
unsigned long RTC_CTIME2;
unsigned long RTC_SEC;
unsigned long RTC_MIN;
unsigned long RTC_HOUR;
unsigned long RTC_DOM;
unsigned long RTC_DOW;
unsigned long RTC_DOY;
unsigned long RTC_MONTH;
unsigned long RTC_YEAR;
unsigned long RTC_CALIBRATION;
unsigned long RTC_GPREG0;
unsigned long RTC_GPREG1;
unsigned long RTC_GPREG2;
unsigned long RTC_GPREG3;
unsigned long RTC_GPREG4;
unsigned long RTC_AUXEN;
unsigned long RTC_AUX;
unsigned long RTC_ALSEC;
unsigned long RTC_ALMIN;
unsigned long RTC_ALHOUR;
unsigned long RTC_ALDOM;
unsigned long RTC_ALDOW;
unsigned long RTC_ALDOY;
unsigned long RTC_ALMON;
unsigned long RTC_ALYEAR;
} LPC17XX_RTC;

typedef struct stLPC17XX_RTC_RAM
{
unsigned long RTC_RAM[(2 * 1024)/sizeof(unsigned long)];
} LPC17XX_RTC_RAM;


typedef struct stLPC17XX_LEG_GPIO
{
unsigned long IOPIN0;
volatile unsigned long IOSET0;
unsigned long IODIR0;
volatile unsigned long IOCLR0;
unsigned long IOPIN1;
volatile unsigned long IOSET1;
unsigned long IODIR1;
volatile unsigned long IOCLR1;
unsigned long ulRes1[24];
unsigned long IOIntStatus;
unsigned long IO0IntStatR;
unsigned long IO0IntStatF;
unsigned long IO0IntClr;
unsigned long IO0IntEnR;
unsigned long IO0IntEnF;
unsigned long ulRes2[3];
unsigned long IO2IntStatR;
unsigned long IO2IntStatF;
unsigned long IO2IntClr;
unsigned long IO2IntEnR;
unsigned long IO2IntEnF;
} LPC17XX_LEG_GPIO;

#ifdef LPC1788
typedef struct stLPC17XX_PIN_CON
{
unsigned long IOCON_P0_00;
unsigned long IOCON_P0_01;
unsigned long IOCON_P0_02;
unsigned long IOCON_P0_03;
unsigned long IOCON_P0_04;
unsigned long IOCON_P0_05;
unsigned long IOCON_P0_06;
unsigned long IOCON_P0_07;
unsigned long IOCON_P0_08;
unsigned long IOCON_P0_09;
unsigned long IOCON_P0_10;
unsigned long IOCON_P0_11;
unsigned long IOCON_P0_12;
unsigned long IOCON_P0_13;
unsigned long IOCON_P0_14;
unsigned long IOCON_P0_15;
unsigned long IOCON_P0_16;
unsigned long IOCON_P0_17;
unsigned long IOCON_P0_18;
unsigned long IOCON_P0_19;
unsigned long IOCON_P0_20;
unsigned long IOCON_P0_21;
unsigned long IOCON_P0_22;
unsigned long IOCON_P0_23;
unsigned long IOCON_P0_24;
unsigned long IOCON_P0_25;
unsigned long IOCON_P0_26;
unsigned long IOCON_P0_27;
unsigned long IOCON_P0_28;
unsigned long IOCON_P0_29;
unsigned long IOCON_P0_30;
unsigned long IOCON_P0_31;
unsigned long IOCON_P1_00;
unsigned long IOCON_P1_01;
unsigned long IOCON_P1_02;
unsigned long IOCON_P1_03;
unsigned long IOCON_P1_04;
unsigned long IOCON_P1_05;
unsigned long IOCON_P1_06;
unsigned long IOCON_P1_07;
unsigned long IOCON_P1_08;
unsigned long IOCON_P1_09;
unsigned long IOCON_P1_10;
unsigned long IOCON_P1_11;
unsigned long IOCON_P1_12;
unsigned long IOCON_P1_13;
unsigned long IOCON_P1_14;
unsigned long IOCON_P1_15;
unsigned long IOCON_P1_16;
unsigned long IOCON_P1_17;
unsigned long IOCON_P1_18;
unsigned long IOCON_P1_19;
unsigned long IOCON_P1_20;
unsigned long IOCON_P1_21;
unsigned long IOCON_P1_22;
unsigned long IOCON_P1_23;
unsigned long IOCON_P1_24;
unsigned long IOCON_P1_25;
unsigned long IOCON_P1_26;
unsigned long IOCON_P1_27;
unsigned long IOCON_P1_28;
unsigned long IOCON_P1_29;
unsigned long IOCON_P1_30;
unsigned long IOCON_P1_31;
unsigned long IOCON_P2_00;
unsigned long IOCON_P2_01;
unsigned long IOCON_P2_02;
unsigned long IOCON_P2_03;
unsigned long IOCON_P2_04;
unsigned long IOCON_P2_05;
unsigned long IOCON_P2_06;
unsigned long IOCON_P2_07;
unsigned long IOCON_P2_08;
unsigned long IOCON_P2_09;
unsigned long IOCON_P2_10;
unsigned long IOCON_P2_11;
unsigned long IOCON_P2_12;
unsigned long IOCON_P2_13;
unsigned long IOCON_P2_14;
unsigned long IOCON_P2_15;
unsigned long IOCON_P2_16;
unsigned long IOCON_P2_17;
unsigned long IOCON_P2_18;
unsigned long IOCON_P2_19;
unsigned long IOCON_P2_20;
unsigned long IOCON_P2_21;
unsigned long IOCON_P2_22;
unsigned long IOCON_P2_23;
unsigned long IOCON_P2_24;
unsigned long IOCON_P2_25;
unsigned long IOCON_P2_26;
unsigned long IOCON_P2_27;
unsigned long IOCON_P2_28;
unsigned long IOCON_P2_29;
unsigned long IOCON_P2_30;
unsigned long IOCON_P2_31;
unsigned long IOCON_P3_00;
unsigned long IOCON_P3_01;
unsigned long IOCON_P3_02;
unsigned long IOCON_P3_03;
unsigned long IOCON_P3_04;
unsigned long IOCON_P3_05;
unsigned long IOCON_P3_06;
unsigned long IOCON_P3_07;
unsigned long IOCON_P3_08;
unsigned long IOCON_P3_09;
unsigned long IOCON_P3_10;
unsigned long IOCON_P3_11;
unsigned long IOCON_P3_12;
unsigned long IOCON_P3_13;
unsigned long IOCON_P3_14;
unsigned long IOCON_P3_15;
unsigned long IOCON_P3_16;
unsigned long IOCON_P3_17;
unsigned long IOCON_P3_18;
unsigned long IOCON_P3_19;
unsigned long IOCON_P3_20;
unsigned long IOCON_P3_21;
unsigned long IOCON_P3_22;
unsigned long IOCON_P3_23;
unsigned long IOCON_P3_24;
unsigned long IOCON_P3_25;
unsigned long IOCON_P3_26;
unsigned long IOCON_P3_27;
unsigned long IOCON_P3_28;
unsigned long IOCON_P3_29;
unsigned long IOCON_P3_30;
unsigned long IOCON_P3_31;
unsigned long IOCON_P4_00;
unsigned long IOCON_P4_01;
unsigned long IOCON_P4_02;
unsigned long IOCON_P4_03;
unsigned long IOCON_P4_04;
unsigned long IOCON_P4_05;
unsigned long IOCON_P4_06;
unsigned long IOCON_P4_07;
unsigned long IOCON_P4_08;
unsigned long IOCON_P4_09;
unsigned long IOCON_P4_10;
unsigned long IOCON_P4_11;
unsigned long IOCON_P4_12;
unsigned long IOCON_P4_13;
unsigned long IOCON_P4_14;
unsigned long IOCON_P4_15;
unsigned long IOCON_P4_16;
unsigned long IOCON_P4_17;
unsigned long IOCON_P4_18;
unsigned long IOCON_P4_19;
unsigned long IOCON_P4_20;
unsigned long IOCON_P4_21;
unsigned long IOCON_P4_22;
unsigned long IOCON_P4_23;
unsigned long IOCON_P4_24;
unsigned long IOCON_P4_25;
unsigned long IOCON_P4_26;
unsigned long IOCON_P4_27;
unsigned long IOCON_P4_28;
unsigned long IOCON_P4_29;
unsigned long IOCON_P4_30;
unsigned long IOCON_P4_31;
unsigned long IOCON_P5_00;
unsigned long IOCON_P5_01;
unsigned long IOCON_P5_02;
unsigned long IOCON_P5_03;
unsigned long IOCON_P5_04;
} LPC17XX_PIN_CON;
#else
typedef struct stLPC17XX_PIN_CON
{
unsigned long PINSEL0;
unsigned long PINSEL1;
unsigned long PINSEL2;
unsigned long PINSEL3;
unsigned long PINSEL4;
unsigned long PINSEL5;
unsigned long PINSEL6;
unsigned long PINSEL7;
unsigned long PINSEL8;
unsigned long PINSEL9;
unsigned long PINSEL10;
unsigned long ulRes0[5];
unsigned long PINMODE0;
unsigned long PINMODE1;
unsigned long PINMODE2;
unsigned long PINMODE3;
unsigned long PINMODE4;
unsigned long PINMODE5;
unsigned long PINMODE6;
unsigned long PINMODE7;
unsigned long PINMODE8;
unsigned long PINMODE9;
} LPC17XX_PIN_CON;
#endif

typedef struct stLPC17XX_SCM                                             // {1}
{
unsigned long MAMCR;
unsigned long ulRes1[31];
unsigned long PLL0CON;
unsigned long PLL0CFG;
unsigned long PLL0STAT;
unsigned long PLL0FEED;
unsigned long ulRes2[4];
unsigned long PLL1CON;
unsigned long PLL1CFG;
unsigned long PLL1STAT;
unsigned long PLL1FEED;
unsigned long ulRes3[4];
unsigned long PCON;
unsigned long PCONP;
unsigned long ulRes4[15];
unsigned long CCLKCFG;
unsigned long USBCLKCFG;
unsigned long CLKRCSEL;
unsigned long ulRes5[12];
unsigned long EXTINT;
unsigned long INTWAKE;
unsigned long EXTMODE;
unsigned long EXTPOLAR;
unsigned long ulRes6[12];
unsigned long RSID;
#ifdef LPC1788
    unsigned long ulRes7;
    unsigned long Matrix_Arb;
    unsigned long ulRes8[5];
    unsigned long SCS;
    unsigned long ulRes9;
    unsigned long PCLKSEL;
    unsigned long ulRes10;
    unsigned long PBOOST;
    unsigned long ulRes11;
    unsigned long LCD_CFG;
    unsigned long ulRes12;
#else
    unsigned long ulRes8[7];
    unsigned long SCS;
    unsigned long IRCTRIM;
    unsigned long PCLKSEL0;
    unsigned long PCLKSEL1;
    unsigned long ulRes9[5];
#endif
unsigned long USBIntSt;
unsigned long ulRes13;
unsigned long CLKOUTCFG;
#ifdef LPC1788
    unsigned long ulRes14[4];
    unsigned long EMCDLYCTL;
    unsigned long EMCCAL;
#endif
} LPC17XX_SCM;

typedef struct stLPC17XX_EMAC
{
unsigned long MAC1;
unsigned long MAC2;
unsigned long IPGT;
unsigned long IPGR;
unsigned long CLRT;
unsigned long MAXF;
unsigned long SUPP;
unsigned long TEST;
unsigned long MCFG;
unsigned long MCMD;
unsigned long MADR;
unsigned long MWTD;
unsigned long MRDD;
unsigned long MIND;
unsigned long ulRes1;
unsigned long SA0;
unsigned long SA1;
unsigned long SA2;
unsigned char ucRes1[184];
unsigned long emac_command;
unsigned long emac_status;
unsigned long emac_RxDescriptor;
unsigned long emac_RxStatus;
unsigned long emac_RxDescriptorNumber;
unsigned long emac_RxProduceIndex;
unsigned long emac_RxConsumeIndex;
unsigned long emac_TxDescriptor;
unsigned long emac_TxStatus;
unsigned long emac_TxDescriptorNumber;
unsigned long emac_TxProduceIndex;
unsigned long emac_TxConsumeIndex;
unsigned char ucRes2[40];
unsigned long TSV0;
unsigned long TSV1;
unsigned long RSV;
unsigned char ucRes3[16];
unsigned long emac_FlowControlCounter;
unsigned long emac_FlowControlStatus;
unsigned char ucRes4[136];
unsigned long emac_RxFilterControl;
unsigned long emac_RxFilterWoLStatus;
unsigned long emac_RxFilterWoLClear;
unsigned char ulRes2;
unsigned long emac_HashFilterL;
unsigned long emac_HashFilterH;
unsigned char ucRes5[3528];
unsigned long emac_IntStatus;
unsigned long emac_IntEnable;
unsigned long emac_IntClear;
unsigned long emac_IntSet;
unsigned char ulRes3;
unsigned long emac_PowerDown;
unsigned long ulRes4[2];
unsigned long EMAC_MODULE_ID;
} LPC17XX_EMAC;



typedef struct stLPC17XX_USB
{
unsigned long USBDevIntSt;
unsigned long USBDevIntEn;
unsigned long USBDevIntClr;
unsigned long USBDevIntSet;
unsigned long USBCmdCode;
unsigned long USBCmdData;
unsigned long USBRxData;
unsigned long USBTxData;
unsigned long USBRxPLen;
unsigned long USBTxPLen;
unsigned long USBCtrl;
unsigned long USBDevIntPri;
unsigned long USBEpIntSt;
unsigned long USBEpIntEn;
unsigned long USBEpIntClr;
unsigned long USBEpIntSet;
unsigned long USBEpIntPri;
unsigned long USBReEp;
unsigned long USBEpInd;
unsigned long USBMaxPSize;
unsigned long USBDMARSt;
unsigned long USBDMARClr;
unsigned long USBDMARSet;
unsigned long res1[9];
unsigned long USBUDCAH;
unsigned long USBEpDMASt;
unsigned long USBEpDMAEn;
unsigned long USBEpDMADis;
unsigned long USBDMAIntSt;
unsigned long USBDMAIntEn;
unsigned long res2[2];
unsigned long USBEoTIntSt;
unsigned long USBEoTIntClr;
unsigned long USBEoTIntSet;
unsigned long USBNDDRIntSt;
unsigned long USBNDDRIntClr;
unsigned long USBNDDRIntSet;
unsigned long USBSysErrIntSt;
unsigned long USBSysErrIntClr;
unsigned long USBSysErrIntSet;
unsigned long res3[843];
unsigned long USBClkCtrl;
unsigned long USBClkSt;
} LPC17XX_USB;

#ifdef LPC1788

typedef struct stLPC17XX_USB_1788                                        // {4}
{
unsigned long USBPortSel;
} LPC17XX_USB_1788;


typedef struct stLPC17XX_MCI
{
unsigned long MCIPower;
unsigned long MCIClock;
unsigned long MCIArgument;
unsigned long MCICommand;
unsigned long MCIRespCmd;
unsigned long MCIResponse0;
unsigned long MCIResponse1;
unsigned long MCIResponse2;
unsigned long MCIResponse3;
unsigned long MCIDataTimer;
unsigned long MCIDataLength;
unsigned long MCIDataCtrl;
unsigned long MCIDataCnt;
unsigned long MCIStatus;
unsigned long MCIClear;
unsigned long MCIMask0;
unsigned long ulRes1[2];
unsigned long MCIFifoCnt;
unsigned long ulRes2[13];
unsigned long MCIFIFO0;
unsigned long MCIFIFO1;
unsigned long MCIFIFO2;
unsigned long MCIFIFO3;
unsigned long MCIFIFO4;
unsigned long MCIFIFO5;
unsigned long MCIFIFO6;
unsigned long MCIFIFO7;
unsigned long MCIFIFO8;
unsigned long MCIFIFO9;
unsigned long MCIFIFO10;
unsigned long MCIFIFO11;
unsigned long MCIFIFO12;
unsigned long MCIFIFO13;
unsigned long MCIFIFO14;
unsigned long MCIFIFO15;
} LPC17XX_MCI;

typedef struct stLPC17XX_EMC
{
unsigned long EMCControl;
unsigned long EMCStatus;
unsigned long EMCConfig;
unsigned long res0[5];
unsigned long EMCDynamicControl;
unsigned long EMCDynamicRefresh;
unsigned long EMCDynamicReadConfig;
unsigned long res1;
unsigned long EMCDynamicRP;
unsigned long EMCDynamicRAS;
unsigned long EMCDynamicSREX;
unsigned long EMCDynamicAPR;
unsigned long EMCDynamicDAL;
unsigned long EMCDynamicWR;
unsigned long EMCDynamicRC;
unsigned long EMCDynamicRFC;
unsigned long EMCDynamicXSR;
unsigned long EMCDynamicRRD;
unsigned long EMCDynamicMRD;
unsigned long res2[9];
unsigned long EMCStaticExtendedWait;
unsigned long res3[31];
unsigned long EMCDynamicConfig0;
unsigned long EMCDynamicRasCas0;
unsigned long res4[6];
unsigned long EMCDynamicConfig1;
unsigned long EMCDynamicRasCas1;
unsigned long res5[6];
unsigned long EMCDynamicConfig2; 
unsigned long EMCDynamicRasCas2; 
unsigned long res6[6];
unsigned long EMCDynamicConfig3; 
unsigned long EMCDynamicRasCas3; 
unsigned long res7[38];
unsigned long EMCStaticConfig0; 
unsigned long EMCStaticWaitWen0;
unsigned long EMCStaticWaitOen0; 
unsigned long EMCStaticWaitRd0; 
unsigned long EMCStaticWaitPage0;
unsigned long EMCStaticWaitWr0;
unsigned long EMCStaticWaitTurn0;
unsigned long ulRes8;
unsigned long EMCStaticConfig1;
unsigned long EMCStaticWaitWen1;
unsigned long EMCStaticWaitOen1;
unsigned long EMCStaticWaitRd1;
unsigned long EMCStaticWaitPage1;
unsigned long EMCStaticWaitWr1;
unsigned long EMCStaticWaitTurn1;
unsigned long ulRes9;
unsigned long EMCStaticConfig2;
unsigned long EMCStaticWaitWen2;
unsigned long EMCStaticWaitOen2;
unsigned long EMCStaticWaitRd2;
unsigned long EMCStaticWaitPage2;
unsigned long EMCStaticWaitWr2;
unsigned long EMCStaticWaitTurn2;
unsigned long ulRes10;
unsigned long EMCStaticConfig3;
unsigned long EMCStaticWaitWen3;
unsigned long EMCStaticWaitOen3;
unsigned long EMCStaticWaitRd3;
unsigned long EMCStaticWaitPage3;
unsigned long EMCStaticWaitWr3;
unsigned long EMCStaticWaitTurn3;
} LPC17XX_EMC;

typedef struct stLPC17XX_LCD
{
unsigned long LCD_TIMH;
unsigned long LCD_TIMV;
unsigned long LCD_POL;
unsigned long LCD_LE;
unsigned long LCD_UPBASE;
unsigned long LCD_LPBASE;
unsigned long LCD_CTRL;
unsigned long LCD_INTMSK;
unsigned long LCD_INTRAW;
unsigned long LCD_INTSTAT;
unsigned long LCD_INTCLR;
unsigned long LCD_UPCURR;
unsigned long LCD_LPCURR;
unsigned long ulRes0[115];
unsigned long LCD_PAL[128];
unsigned long ulRes1[256];
unsigned long CRSR_IMG[256];
unsigned long CRSR_CTRL;
unsigned long CRSR_CFG;
unsigned long CRSR_PAL0;
unsigned long CRSR_PAL1;
unsigned long CRSR_XY;
unsigned long CRSR_CLIP;
unsigned long ulRes2[2];
unsigned long CRSR_INTMSK;
unsigned long CRSR_INTCLR;
unsigned long CRSR_INTRAW;
unsigned long CRSR_INTSTAT;
} LPC17XX_LCD; 
#endif


typedef struct stLM3SXXXX_CORTEX_M3
{
unsigned long ulRes1;
const unsigned long INT_CONT_TYPE;
unsigned long ulRes2[2];
unsigned long SYSTICK_CSR;
unsigned long SYSTICK_RELOAD;
unsigned long SYSTICK_CURRENT;
const unsigned long SYSTICK_CALIB;
unsigned long ulRes3[56];
unsigned long IRQ0_31_SER;   
unsigned long IRQ32_63_SER;  
unsigned long IRQ64_95_SER;  
unsigned long IRQ96_127_SER; 
unsigned long IRQ128_159_SER;
unsigned long IRQ160_191_SER;
unsigned long IRQ192_223_SER;
unsigned long IRQ224_239_SER;
unsigned long ulRes4[24];
unsigned long IRQ0_31_CER;
unsigned long IRQ32_63_CER;  
unsigned long IRQ64_95_CER;
unsigned long IRQ96_127_CER;
unsigned long IRQ128_159_CER;
unsigned long IRQ160_191_CER;
unsigned long IRQ192_223_CER;
unsigned long IRQ224_239_CER;
unsigned long ulRes5[24];
unsigned long IRQ0_31_SPR;
unsigned long IRQ32_63_SPR;
unsigned long IRQ64_95_SPR;
unsigned long IRQ96_127_SPR;
unsigned long IRQ128_159_SPR;
unsigned long IRQ160_191_SPR;
unsigned long IRQ192_223_SPR;
unsigned long IRQ224_239_SPR;
unsigned long ulRes6[24];
unsigned long IRQ0_31_CPR;
unsigned long IRQ32_63_CPR;
unsigned long IRQ64_95_CPR;
unsigned long IRQ96_127_CPR;
unsigned long IRQ128_159_CPR;
unsigned long IRQ160_191_CPR;
unsigned long IRQ192_223_CPR;
unsigned long IRQ224_239_CPR;
unsigned long ulRes7[24];
unsigned long IRQ0_31_ABR;
unsigned long IRQ32_63_ABR;
unsigned long IRQ64_95_ABR;
unsigned long IRQ96_127_ABR;
unsigned long IRQ128_159_ABR;
unsigned long IRQ160_191_ABR;
unsigned long IRQ192_223_ABR;
unsigned long IRQ224_239_ABR;
unsigned long ulRes8[56];
unsigned long IRQ0_3_PRIORITY_REGISTER;
unsigned long IRQ4_7_PRIORITY_REGISTER;
unsigned long IRQ8_11_PRIORITY_REGISTER;
unsigned long IRQ12_15_PRIORITY_REGISTER;
unsigned long IRQ16_19_PRIORITY_REGISTER;
unsigned long IRQ20_23_PRIORITY_REGISTER;
unsigned long IRQ24_27_PRIORITY_REGISTER;
unsigned long IRQ28_31_PRIORITY_REGISTER;
unsigned long IRQ32_35_PRIORITY_REGISTER;
unsigned long IRQ36_39_PRIORITY_REGISTER;
unsigned long IRQ40_43_PRIORITY_REGISTER;
unsigned long IRQ44_47_PRIORITY_REGISTER;
unsigned long IRQ48_51_PRIORITY_REGISTER;
unsigned long IRQ52_55_PRIORITY_REGISTER;
unsigned long IRQ56_59_PRIORITY_REGISTER;
unsigned long IRQ60_63_PRIORITY_REGISTER;
unsigned long IRQ64_67_PRIORITY_REGISTER;
unsigned long IRQ68_71_PRIORITY_REGISTER;
unsigned long IRQ72_75_PRIORITY_REGISTER;
unsigned long IRQ76_79_PRIORITY_REGISTER;
unsigned long IRQ80_83_PRIORITY_REGISTER;
unsigned long IRQ84_87_PRIORITY_REGISTER;
unsigned long IRQ88_91_PRIORITY_REGISTER;
unsigned long IRQ92_95_PRIORITY_REGISTER;
unsigned long IRQ96_99_PRIORITY_REGISTER;
unsigned long IRQ100_103_PRIORITY_REGISTER;
unsigned long IRQ104_107_PRIORITY_REGISTER;
unsigned long IRQ108_111_PRIORITY_REGISTER;
unsigned long IRQ112_115_PRIORITY_REGISTER;
unsigned long IRQ116_119_PRIORITY_REGISTER;
unsigned long IRQ120_123_PRIORITY_REGISTER;
unsigned long IRQ124_127_PRIORITY_REGISTER;
unsigned long IRQ128_131_PRIORITY_REGISTER;
unsigned long IRQ132_135_PRIORITY_REGISTER;
unsigned long IRQ136_139_PRIORITY_REGISTER;
unsigned long IRQ140_143_PRIORITY_REGISTER;
unsigned long IRQ144_147_PRIORITY_REGISTER;
unsigned long IRQ148_151_PRIORITY_REGISTER;
unsigned long IRQ152_155_PRIORITY_REGISTER;
unsigned long IRQ156_159_PRIORITY_REGISTER;
unsigned long IRQ160_163_PRIORITY_REGISTER;
unsigned long IRQ164_167_PRIORITY_REGISTER;
unsigned long IRQ168_171_PRIORITY_REGISTER;
unsigned long IRQ172_175_PRIORITY_REGISTER;
unsigned long IRQ176_179_PRIORITY_REGISTER;
unsigned long IRQ180_183_PRIORITY_REGISTER;
unsigned long IRQ184_187_PRIORITY_REGISTER;
unsigned long IRQ188_191_PRIORITY_REGISTER;
unsigned long IRQ192_195_PRIORITY_REGISTER;
unsigned long IRQ196_199_PRIORITY_REGISTER;
unsigned long IRQ200_203_PRIORITY_REGISTER;
unsigned long IRQ204_207_PRIORITY_REGISTER;
unsigned long IRQ208_211_PRIORITY_REGISTER;
unsigned long IRQ212_215_PRIORITY_REGISTER;
unsigned long IRQ216_219_PRIORITY_REGISTER;
unsigned long IRQ220_223_PRIORITY_REGISTER;
unsigned long IRQ224_227_PRIORITY_REGISTER;
unsigned long IRQ228_231_PRIORITY_REGISTER;
unsigned long IRQ232_235_PRIORITY_REGISTER;
unsigned long IRQ236_239_PRIORITY_REGISTER;
unsigned long ulRes9[516];
unsigned long CPUID_BASE_REGISTER;
unsigned long INT_CONT_STATE_REG;
unsigned long VECTOR_TABLE_OFFSET_REG;
unsigned long APPLICATION_INT_RESET_CTR_REG;
unsigned long SYSTEM_CONTROL_REGISTER;
unsigned long CONFIGURATION_CONTROL_REGISTER;
} LM3SXXXX_CORTEX_M3;

typedef struct stLM3SXXXX_CORTEX_M3_REGS
{
unsigned long ulPRIMASK;
} LM3SXXXX_CORTEX_M3_REGS;

#define INTERRUPT_MASKED 0x00000001




typedef struct stLPC17XX_PERIPH
{
#if defined LPC1788
    LPC17XX_FAST_GPIO   ucFastGPIO[6];
#else
    LPC17XX_FAST_GPIO   ucFastGPIO[5];
#endif
    LPC17XX_WATCHDOG    ucWDOG;
    LPC17XX_TIMER       ucSimTimer0;
    LPC17XX_TIMER       ucSimTimer1;
    LPC17XX_UART        ucSimUart0;
    LPC17XX_UART        ucSimUart1;
	LPC17XX_IIC         ucIIC0;
    LPC17XX_RTC         ucRTC;
    LPC17XX_LEG_GPIO    ucLegacyGPIO;
    LPC17XX_PIN_CON     ucPIN_CON;
    LPC17XX_SSP         ucSSP1;
    LPC17XX_ADC         ucADC;                                           // {2}
    LPC17XX_IIC         ucIIC1;
    LPC17XX_SSP         ucSSP0;
    LPC17XX_TIMER       ucSimTimer2;
    LPC17XX_TIMER       ucSimTimer3;
    LPC17XX_UART        ucSimUart2;
    LPC17XX_UART        ucSimUart3;
#if defined LPC1788
    LPC17XX_UART        ucSimUart4;
#endif
	LPC17XX_IIC         ucIIC2;
	LPC17XX_RIT         ucRIT;
    LPC17XX_RTC_RAM     ulRTC_ram;
    LPC17XX_SCM         ucSCM;
    LPC17XX_EMAC        ucEMAC;
    LPC17XX_USB         ucUSB;
#if defined LPC1788
    LPC17XX_USB_1788    ulLPC1788USB;                                    // {4}
    LPC17XX_MCI         ulMCI;                                           // {3}
    LPC17XX_EMC         ucEMC;
    LPC17XX_LCD         ucLCD;
#endif

    LM3SXXXX_CORTEX_M3  CORTEX_M3;

    LM3SXXXX_CORTEX_M3_REGS CORTEX_M3_REGS;                              // only for simulation use

} LPC17XX_PERIPH;

extern LPC17XX_PERIPH  ucLPC17xx;


#define _PORT0   0
#define _PORT1   1
#define _PORT2   2
#define _PORT3   3
#define _PORT4   4
#define _PORT5   5



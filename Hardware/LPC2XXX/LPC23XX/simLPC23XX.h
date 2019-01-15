/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    simLPC23XX.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    08.10.2009 GPIO quantity made chip dependent                         {1}
    19.10.2009 RTC correction for LPC2101/2/3 (rev. A)                   {2}
    07.11.2009 Add LPC2103 PWM register                                  {3}
    26.01.2010 Add SD/MMC registers                                      {4}
    16.03.2010 Add USB device registers                                  {5}
    30.07.2010 Add DMA controller                                        {6}
    02.08.2010 Add PWM controller                                        {7}
    11.08.2010 Add ADC controller                                        {8}
    25.08.2010 Correct LPC23XX_PIN_CON struct                            {9}
    29.08.2010 Add SPI registers                                         {10}
    03.02.2011 Add CAN registers                                         {11}
    17.02.2012 Add APBDIV to _LPC213X/_LPC214X                           {12}
    18.09.2012 Correct EMAC addresses                                    {13}

*/  


typedef struct stLPC23XX_FAST_GPIO
{
unsigned long FIODIR;
unsigned long res1[3];                      
unsigned long FIOMASK;                     
unsigned long FIOPIN;                      
unsigned long FIOSET;                      
unsigned long FIOCLR;                      
} LPC23XX_FAST_GPIO;


typedef struct stLPC23XX_WATCHDOG
{
unsigned long WDMOD;                      
unsigned long WDTC;                     
unsigned long WDFEED;                      
unsigned long WDTV;                      
unsigned long WDCLKSEL;                      
} LPC23XX_WATCHDOG;


typedef struct stLPC23XX_TIMER
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
unsigned long ucRes1[12];
unsigned long TCTCR;
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {3}
    unsigned long PWMCON;
#endif
} LPC23XX_TIMER;

typedef struct stLPC23XX_UART
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
} LPC23XX_UART;



typedef struct stLPC23XX_PWM
{
unsigned long PWMIR;
unsigned long PWMTCR;
unsigned long PWMTC;
unsigned long PWMPR;
unsigned long PWMPC;
unsigned long PWMMCR;
unsigned long PWMMR0;
unsigned long PWMMR1;
unsigned long PWMMR2;
unsigned long PWMMR3;
unsigned long PWMCCR;
unsigned long PWMCR0;
unsigned long PWMCR1;
unsigned long PWMCR2;
unsigned long PWMCR3;
unsigned long ulRes0;
unsigned long PWMMR4;
unsigned long PWMMR5;
unsigned long PWMMR6;
unsigned long PWMPCR;
unsigned long PWMLER;
unsigned long ulRes1[7];
unsigned long PWMCTCR;
} LPC23XX_PWM;



typedef struct stLPC23XX_SSP
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
} LPC23XX_SSP;


typedef struct stLPC23XX_ADC                                             // {8}
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
} LPC23XX_ADC;


typedef struct stLPC23XX_CAN_AFRAM                                       // {11}
{
    unsigned long ulRes0;
} LPC23XX_CAN_AFRAM;

typedef struct stLPC23XX_CAN_AF                                          // {11}
{
unsigned long AFMR;
unsigned long SFF_sa;
unsigned long SFF_GRP_sa;
unsigned long EFF_sa;
unsigned long EFF_GRP_sa;
unsigned long ENDofTable;
unsigned long LUTerrAd;
unsigned long LUTerr;
unsigned long FCANIE;
unsigned long FCANIC0;
unsigned long FCANIC1;
} LPC23XX_CAN_AF;

typedef struct stLPC23XX_CAN                                             // {11}
{
unsigned long CANTxSR;
unsigned long CANRxSR;
unsigned long CANMSR;
} LPC23XX_CAN;

typedef struct stLPC23XX_CAN_REG                                         // {11}
{
unsigned long CANMOD;
unsigned long CANCMR;
unsigned long CANGSR;
unsigned long CANICR;
unsigned long CANIER;
unsigned long CANBTR;
unsigned long CANEWL;
unsigned long CANSR;
unsigned long CANRFS;
unsigned long CANRID;
unsigned long CANRDA;
unsigned long CANRDB;
unsigned long CANTFI1;
unsigned long CANTID1;
unsigned long CANTDA1;
unsigned long CANTDB1;
unsigned long CANTFI2;
unsigned long CANTID2;
unsigned long CANTDA2;
unsigned long CANTDB2;
unsigned long CANTFI3;
unsigned long CANTID3;
unsigned long CANTDA3;
unsigned long CANTDB3;
} LPC23XX_CAN_REG;





typedef struct stLPC23XX_IIC
{
unsigned long I2CCONSET;
unsigned long I2CSTAT;
unsigned long I2CDAT;   
unsigned long I2CADR;   
unsigned long I2CSCLH;  
unsigned long I2CSCLL;  
unsigned long I2CCONCLR;
} LPC23XX_IIC;


typedef struct stLPC23XX_SPI                                             // {10}
{
unsigned long SPISPCR;
unsigned long SPISPSR;
unsigned long SPISPDR;
unsigned long SPISPCCR;
unsigned long ulRes0[3];
unsigned long SPISPINT;
} LPC23XX_SPI;


typedef struct stLPC23XX_RTC
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
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {2}
    unsigned long RTC_PWRCTRL;
    unsigned long RTC_GPREG0;
    unsigned long RTC_GPREG1;
    unsigned long RTC_GPREG2;
    unsigned long ulRes[4];
#else
    unsigned long RTC_CISS;
    unsigned long ulRes[7];
#endif
unsigned long RTC_ALSEC;
unsigned long RTC_ALMIN;
unsigned long RTC_ALHOUR;
unsigned long RTC_ALDOM;
unsigned long RTC_ALDOW;
unsigned long RTC_ALDOY;
unsigned long RTC_ALMON;
unsigned long RTC_ALYEAR;
unsigned long RTC_PREINT;
unsigned long RTC_PREFRAC;
} LPC23XX_RTC;

typedef struct stLPC23XX_RTC_RAM
{
unsigned long RTC_RAM[(2 * 1024)/sizeof(unsigned long)];
} LPC23XX_RTC_RAM;


typedef struct stLPC23XX_LEG_GPIO
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
} LPC23XX_LEG_GPIO;

typedef struct stLPC23XX_PIN_CON
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
unsigned long PINSEL11;                                                  // only LPC247X
unsigned long ulRes0[4];                                                 // {9}
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
} LPC23XX_PIN_CON;

typedef struct stLPC23XX_MCI
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
} LPC23XX_MCI;


typedef struct stLPC23XX_SCM
{
unsigned long MAMCR;
unsigned long MAMTIM;
unsigned char ucRes1[0x40 - 8];
unsigned long MEMMAP;
unsigned char ucRes2[0x40 - 4];
unsigned long PLLCON;                   
unsigned long PLLCFG;                   
unsigned long PLLSTAT;                  
unsigned long PLLFEED;                  
unsigned char ucRes3[0xc0 - 0x8c - 4];
unsigned long PCON;                     
unsigned long PCONP;    
#if defined _LPC214X || defined _LPC213X                                 // {12}
    unsigned char ucRes4[0x104 - 0xc4 - 8];
    unsigned long APBDIV; 
#else
    unsigned char ucRes4[0x104 - 0xc4 - 4];
#endif                
unsigned long CCLKCFG;                  
unsigned long USBCLKCFG;                
unsigned long CLKRCSEL;                 
unsigned char ucRes5[0x144 - 0x10c - 4];
unsigned long INTWAKE;                  
unsigned char ucRes6[0x180 - 0x144 - 4];
unsigned long RSIR;                     
unsigned char ucRes7[0x1a0 - 0x180 - 4];
unsigned long SCS;                                                       // {4}
unsigned long IRCTRIM;                  
unsigned long PCLKSEL0;                 
unsigned long PCLKSEL1; 
unsigned long ulRes8[2];
unsigned long LCD_CFG;
unsigned long ulRes9;
unsigned long USBIntSt;                                                  // {5}
} LPC23XX_SCM;


typedef struct stLPC23XX_USB_2378
{
unsigned long USBPortSel;
} LPC23XX_USB_2378;


typedef struct stLPC23XX_USB_DEVICE
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
unsigned long ulRes0[9];
unsigned long USBUDCAH;
unsigned long USBEpDMASt;
unsigned long USBEpDMAEn;
unsigned long USBEpDMADis;
unsigned long USBDMAIntSt;
unsigned long USBDMAIntEn;
unsigned long ulRes1[2];
unsigned long USBEoTIntSt;
unsigned long USBEoTIntClr;
unsigned long USBEoTIntSet;
unsigned long USBNDDRIntSt;
unsigned long USBNDDRIntClr;
unsigned long USBNDDRIntSet;
unsigned long USBSysErrIntSt;
unsigned long USBSysErrIntClr;
unsigned long USBSysErrIntSetSystem;
unsigned long ulRes2[884];
unsigned long USBClkCtrl;
unsigned long USBClkSt;
} LPC23XX_USB_DEVICE;

typedef struct stLPC23XX_EMAC
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
unsigned char ucRes1[180];                                               // {13}
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
} LPC23XX_EMAC;


typedef struct stLPC23XX_DMAC
{
unsigned long DMACIntStatus;
unsigned long DMACIntTCStatus;
unsigned long DMACIntTCClear;
unsigned long DMACIntErrorStatus;
unsigned long DMACIntErrClr;
unsigned long DMACRawIntTCStatus;
unsigned long DMACRawIntErrorStatus;
unsigned long DMACEnbldChns;
unsigned long DMACSoftBReq;
unsigned long DMACSoftSReq;
unsigned long DMACSoftLBReq;
unsigned long DMACSoftLSReq;
unsigned long DMACConfiguration;
unsigned long DMACSync;
unsigned long ulRes0[51];
unsigned long DMACC0SrcAddr;
unsigned long DMACC0DestAddr;
unsigned long DMACC0LLI;
unsigned long DMACC0Control;
unsigned long DMACC0Configuration;
unsigned long ulRes1[3];
unsigned long DMACC1SrcAddr;
unsigned long DMACC1DestAddr;
unsigned long DMACC1LLI;
unsigned long DMACC1Control;
unsigned long DMACC1Configuration;
} LPC23XX_DMAC;


typedef struct stLPC23XX_EMC
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
} LPC23XX_EMC;

typedef struct stLPC23XX_LCD
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
} LPC23XX_LCD;


typedef struct stLPC23XX_VIC
{
unsigned long VICIRQStatus;              
unsigned long VICFIQStatus;              
unsigned long VICRawIntr;               
unsigned long VICIntSelect;             
unsigned long VICIntEnable;             
unsigned long VICIntEnClr;              
unsigned long VICSoftInt;               
unsigned long VICSoftIntClear;          
unsigned long VICProtection;            
unsigned long VICSWPriorityMask;        
unsigned char ucRes1[0x100 - 0x24 - 4];
unsigned long VICVectAddr0;             
unsigned long VICVectAddr1;             
unsigned long VICVectAddr2;             
unsigned long VICVectAddr3;             
unsigned long VICVectAddr4;              
unsigned long VICVectAddr5;              
unsigned long VICVectAddr6;              
unsigned long VICVectAddr7;              
unsigned long VICVectAddr8;              
unsigned long VICVectAddr9;              
unsigned long VICVectAddr10;             
unsigned long VICVectAddr11;             
unsigned long VICVectAddr12;             
unsigned long VICVectAddr13;             
unsigned long VICVectAddr14;             
unsigned long VICVectAddr15;             
unsigned long VICVectAddr16;             
unsigned long VICVectAddr17;             
unsigned long VICVectAddr18;             
unsigned long VICVectAddr19;             
unsigned long VICVectAddr20;             
unsigned long VICVectAddr21;          
unsigned long VICVectAddr22;             
unsigned long VICVectAddr23;             
unsigned long VICVectAddr24;             
unsigned long VICVectAddr25;             
unsigned long VICVectAddr26;             
unsigned long VICVectAddr27;             
unsigned long VICVectAddr28;             
unsigned long VICVectAddr29;             
unsigned long VICVectAddr30;             
unsigned long VICVectAddr31;             
unsigned char ucRes2[0x200 - 0x17c - 4];
unsigned long VICVectPriority0;          
unsigned long VICVectPriority1;    
unsigned long VICVectPriority2;    
unsigned long VICVectPriority3;    
unsigned long VICVectPriority4;          
unsigned long VICVectPriority5;          
unsigned long VICVectPriority6;          
unsigned long VICVectPriority7;          
unsigned long VICVectPriority8;          
unsigned long VICVectPriority9;          
unsigned long VICVectPriority10;         
unsigned long VICVectPriority11;         
unsigned long VICVectPriority12;         
unsigned long VICVectPriority13;         
unsigned long VICVectPriority14;         
unsigned long VICVectPriority15;         
unsigned long VICVectPriority16;         
unsigned long VICVectPriority17;         
unsigned long VICVectPriority18;         
unsigned long VICVectPriority19;         
unsigned long VICVectPriority20;         
unsigned long VICVectPriority21; 
unsigned long VICVectPriority22;         
unsigned long VICVectPriority23;         
unsigned long VICVectPriority24;         
unsigned long VICVectPriority25;         
unsigned long VICVectPriority26;         
unsigned long VICVectPriority27;         
unsigned long VICVectPriority28;         
unsigned long VICVectPriority29;         
unsigned long VICVectPriority30;         
unsigned long VICVectPriority31;         
} LPC23XX_VIC;

typedef struct stLPC23XX_VIC_ADD
{
volatile unsigned long VICAddress;
} LPC23XX_VIC_ADD;

typedef struct stLPC23XX_PERIPH
{
#ifdef _LPC21XX
    LPC23XX_FAST_GPIO   ucFastGPIO[1];                                   // {1}
#else
    LPC23XX_FAST_GPIO   ucFastGPIO[5];                                   // {1}
#endif
    LPC23XX_WATCHDOG    ucWDOG;
    LPC23XX_TIMER       ucSimTimer0;
    LPC23XX_TIMER       ucSimTimer1;
    LPC23XX_UART        ucSimUart0;
    LPC23XX_UART        ucSimUart1;
    LPC23XX_PWM         ucPWM0;                                          // {7}
    LPC23XX_PWM         ucPWM1;
	LPC23XX_IIC         ucIIC0;
    LPC23XX_SPI         ucSPI0;                                          // {10}
    LPC23XX_RTC         ucRTC;
    LPC23XX_LEG_GPIO    ucLegacyGPIO;
    LPC23XX_PIN_CON     ucPIN_CON;
    LPC23XX_SSP         ucSSP1;
    LPC23XX_ADC         ucADC;                                           // {8}
    LPC23XX_CAN_AFRAM   ucCAN_AFRAM;                                     // {11}
    LPC23XX_CAN_AF      ucCAN_AF;
    LPC23XX_CAN         ucCAN;
    LPC23XX_CAN_REG     ucCAN1;
    LPC23XX_CAN_REG     ucCAN2;
    LPC23XX_IIC         ucIIC1;
    LPC23XX_SSP         ucSSP0;
    LPC23XX_TIMER       ucSimTimer2;
    LPC23XX_TIMER       ucSimTimer3;
    LPC23XX_UART        ucSimUart2;
    LPC23XX_UART        ucSimUart3;
	LPC23XX_IIC         ucIIC2;
    LPC23XX_RTC_RAM     ulRTC_ram;
    LPC23XX_MCI         ulMCI;                                           // {4}
    LPC23XX_SCM         ucSCM;
    LPC23XX_USB_2378    ulLPC2378USB;                                    // {5}
    LPC23XX_USB_DEVICE  ulUSB;
    LPC23XX_EMAC        ucEMAC;
    LPC23XX_DMAC        ucDMA;                                           // {6}
    LPC23XX_EMC         ucEMC;
    LPC23XX_LCD         ucLCD;
    LPC23XX_VIC         ucVIC;
    LPC23XX_VIC_ADD     ucVICAddress;
} LPC23XX_PERIPH;

extern LPC23XX_PERIPH  ucLPC23xx;


#define _PORT0   0
#define _PORT1   1
#define _PORT2   2
#define _PORT3   3
#define _PORT4   4



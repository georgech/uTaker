/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      SimAVR32.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    26.01.2010 Add USBB                                                  {1}
    08.11.2010 Add SDRAM controller                                      {2}
    19.06.2011 Add _AT32UC3C                                             {3}
    19.07.2012 Add RTC                                                   {4}
    06.08.2012 Replaced ODMER by PDER registers for _AT32UC3C            {5}

*/


extern void fnSimPortsAVR(int iPort);


typedef struct stAVR32_USBB                                              // {1}
{
unsigned long UDCON;
unsigned long UDINT;
unsigned long UDINTCLR;
unsigned long UDINTSET;
unsigned long UDINTE;
unsigned long UDINTECLR;
unsigned long UDINTESET;
unsigned long UERST;
unsigned long UDFNUM;
unsigned long ulRes1[55];
unsigned long UECFG0;
unsigned long UECFG1;
unsigned long UECFG2;
unsigned long UECFG3;
unsigned long UECFG4;
unsigned long UECFG5;
unsigned long UECFG6;
unsigned long ulRes2[5];
unsigned long UESTA0;
unsigned long UESTA1;
unsigned long UESTA2;
unsigned long UESTA3;
unsigned long UESTA4;
unsigned long UESTA5;
unsigned long UESTA6;
unsigned long ulRes3[5];
unsigned long UESTA0CLR;
unsigned long UESTA1CLR;
unsigned long UESTA2CLR;
unsigned long UESTA3CLR;
unsigned long UESTA4CLR;
unsigned long UESTA5CLR;
unsigned long UESTA6CLR;
unsigned long ulRes4[5];
unsigned long UESTA0SET;
unsigned long UESTA1SET;
unsigned long UESTA2SET;
unsigned long UESTA3SET;
unsigned long UESTA4SET;
unsigned long UESTA5SET;
unsigned long UESTA6SET;
unsigned long ulRes5[5];
unsigned long UECON0;
unsigned long UECON1;
unsigned long UECON2;         
unsigned long UECON3;
unsigned long UECON4;
unsigned long UECON5;
unsigned long UECON6;
unsigned long ulRes6[5];
unsigned long UECON0SET;
unsigned long UECON1SET;
unsigned long UECON2SET;
unsigned long UECON3SET;
unsigned long UECON4SET;
unsigned long UECON5SET;
unsigned long UECON6SET;
unsigned long ulRes7[5];
unsigned long UECON0CLR;
unsigned long UECON1CLR;
unsigned long UECON2CLR;
unsigned long UECON3CLR;
unsigned long UECON4CLR;
unsigned long UECON5CLR;
unsigned long UECON6CLR;
unsigned long ulRes8[53];
unsigned long UDDMA1_NEXTDESC;
unsigned long UDDMA1_ADDR;
unsigned long UDDMA1_CONTROL;
unsigned long UDDMA1_STATUS;
unsigned long UDDMA2_NEXTDESC;
unsigned long UDDMA2_ADDR;
unsigned long UDDMA2_CONTROL;
unsigned long UDDMA2_STATUS;
unsigned long UDDMA3_NEXTDESC;
unsigned long UDDMA3_ADDR;
unsigned long UDDMA3_CONTROL;
unsigned long UDDMA3_STATUS;
unsigned long UDDMA4_NEXTDESC;
unsigned long UDDMA4_ADDR;
unsigned long UDDMA4_CONTROL;
unsigned long UDDMA4_STATUS;
unsigned long UDDMA5_NEXTDESC;
unsigned long UDDMA5_ADDR;
unsigned long UDDMA5_CONTROL;
unsigned long UDDMA5_STATUS;
unsigned long UDDMA6_NEXTDESC;
unsigned long UDDMA6_ADDR;
unsigned long UDDMA6_CONTROL;
unsigned long UDDMA6_STATUS;
unsigned long ulRes9[36];
unsigned long UHCON;
unsigned long UHINT;
unsigned long UHINTCLR;
unsigned long UHINTSET;
unsigned long UHINTE;
unsigned long UHINTECLR;
unsigned long UHINTESET;
unsigned long UPRST;
unsigned long UHFNUM;
unsigned long UHADDR1;
unsigned long UHADDR2;
unsigned long ulRes10[53];
unsigned long UPCFG0;
unsigned long UPCFG1;
unsigned long UPCFG2;
unsigned long UPCFG3;
unsigned long UPCFG4;
unsigned long UPCFG5;
unsigned long UPCFG6;
unsigned long ulRes11[5];
unsigned long UPSTA0;
unsigned long UPSTA1;
unsigned long UPSTA2;
unsigned long UPSTA3;
unsigned long UPSTA4;
unsigned long UPSTA5;
unsigned long UPSTA6;
unsigned long ulRes12[5];
unsigned long SPSTA0CLR;
unsigned long SPSTA1CLR;
unsigned long SPSTA2CLR;
unsigned long SPSTA3CLR;
unsigned long SPSTA4CLR;
unsigned long SPSTA5CLR;
unsigned long SPSTA6CLR;
unsigned long ulRes13[5];
unsigned long SPSTA0SET;
unsigned long SPSTA1SET;
unsigned long SPSTA2SET;
unsigned long SPSTA3SET;
unsigned long SPSTA4SET;
unsigned long SPSTA5SET;
unsigned long SPSTA6SET;
unsigned long ulRes14[5];
unsigned long UPCON0;
unsigned long UPCON1;
unsigned long UPCON2;
unsigned long UPCON3;
unsigned long UPCON4;
unsigned long UPCON5;
unsigned long UPCON6;
unsigned long ulRes15[5];
unsigned long UPCON0SET;
unsigned long UPCON1SET;
unsigned long UPCON2SET;
unsigned long UPCON3SET;
unsigned long UPCON4SET;
unsigned long UPCON5SET;
unsigned long UPCON6SET;
unsigned long ulRes16[5];
unsigned long UPCON0CLR;
unsigned long UPCON1CLR;
unsigned long UPCON2CLR;
unsigned long UPCON3CLR;
unsigned long UPCON4CLR;
unsigned long UPCON5CLR;
unsigned long UPCON6CLR;
unsigned long ulRes17[5];
unsigned long UPINRQ0;
unsigned long UPINRQ1;
unsigned long UPINRQ2;
unsigned long UPINRQ3;
unsigned long UPINRQ4;
unsigned long UPINRQ5;
unsigned long UPINRQ6;
unsigned long ulRes18[5];
unsigned long UPERR0;
unsigned long UPERR1;
unsigned long UPERR2;
unsigned long UPERR3;
unsigned long UPERR4;
unsigned long UPERR5;
unsigned long UPERR6;
unsigned long ulRes19[29];
unsigned long UHDMA1_NEXTDESC;
unsigned long UHDMA1_ADDR;
unsigned long UHDMA1_CONTROL;
unsigned long UHDMA1_STATUS;
unsigned long UHDMA2_NEXTDESC;
unsigned long UHDMA2_ADDR;
unsigned long UHDMA2_CONTROL;
unsigned long UHDMA2_STATUS;
unsigned long UHDMA3_NEXTDESC;
unsigned long UHDMA3_ADDR;
unsigned long UHDMA3_CONTROL;
unsigned long UHDMA3_STATUS;
unsigned long UHDMA4_NEXTDESC;
unsigned long UHDMA4_ADDR;
unsigned long UHDMA4_CONTROL;
unsigned long UHDMA4_STATUS;
unsigned long UHDMA5_NEXTDESC;
unsigned long UHDMA5_ADDR;
unsigned long UHDMA5_CONTROL;
unsigned long UHDMA5_STATUS;
unsigned long UHDMA6_NEXTDESC;
unsigned long UHDMA6_ADDR;
unsigned long UHDMA6_CONTROL;
unsigned long UHDMA6_STATUS;
unsigned long ulRes20[36];
unsigned long USBCON;
unsigned long USBSTA;
unsigned long USBSTACLR;
unsigned long USBSTASET;
unsigned long ulRes21[2];
unsigned long UVERS;
unsigned long UFEATURES;
unsigned long UADDRSIZE;
unsigned long UNAME1;
unsigned long UNAME2;
unsigned long USBFSM;
} AVR32_USBB;

typedef struct stAVR32_HSBMATRIX
{
unsigned long HSB_MCFG0;
unsigned long HSB_MCFG1;
unsigned long HSB_MCFG2;
unsigned long HSB_MCFG3;
unsigned long HSB_MCFG4;
unsigned long HSB_MCFG5;
unsigned long HSB_MCFG6;
unsigned long HSB_MCFG7;
unsigned long HSB_MCFG8;
unsigned long HSB_MCFG9;
unsigned long HSB_MCFG10;
unsigned long HSB_MCFG11;
unsigned long HSB_MCFG12;
unsigned long HSB_MCFG13;
unsigned long HSB_MCFG14;
unsigned long HSB_MCFG15;
unsigned long HSB_SCFG0;
unsigned long HSB_SCFG1;
unsigned long HSB_SCFG2;
unsigned long HSB_SCFG3;
unsigned long HSB_SCFG4;
unsigned long HSB_SCFG5;
unsigned long HSB_SCFG6;
unsigned long HSB_SCFG7;
unsigned long HSB_SCFG8;
unsigned long HSB_SCFG9;
unsigned long HSB_SCFG10;
unsigned long HSB_SCFG11;
unsigned long HSB_SCFG12;
unsigned long HSB_SCFG13;
unsigned long HSB_SCFG14;
unsigned long HSB_SCFG15;
unsigned long HSB_PRAS0;
unsigned long HSB_PRBS0;
unsigned long HSB_PRAS1;
unsigned long HSB_PRBS1;
unsigned long HSB_PRAS2;
unsigned long HSB_PRBS2;
unsigned long HSB_PRAS3;
unsigned long HSB_PRBS3;
unsigned long HSB_PRAS4;
unsigned long HSB_PRBS4;
unsigned long HSB_PRAS5;
unsigned long HSB_PRBS5;
unsigned long HSB_PRAS6;
unsigned long HSB_PRBS6;
unsigned long HSB_PRAS7;
unsigned long HSB_PRBS7;
unsigned long HSB_PRAS8;
unsigned long HSB_PRBS8;
unsigned long HSB_PRAS9;
unsigned long HSB_PRBS9;
unsigned long HSB_PRAS10;
unsigned long HSB_PRBS10;
unsigned long HSB_PRAS11;
unsigned long HSB_PRBS11;
unsigned long HSB_PRAS12;
unsigned long HSB_PRBS12;
unsigned long HSB_PRAS13;
unsigned long HSB_PRBS13;
unsigned long HSB_PRAS14;
unsigned long HSB_PRBS14;
unsigned long HSB_PRAS15;
unsigned long HSB_PRBS15;
unsigned long HSB_MRCR;
unsigned long ucRes[3];
unsigned long HSB_SFR0;
unsigned long HSB_SFR1;
unsigned long HSB_SFR2;
unsigned long HSB_SFR3;
unsigned long HSB_SFR4;
unsigned long HSB_SFR5;
unsigned long HSB_SFR6;
unsigned long HSB_SFR7;
unsigned long HSB_SFR8;
unsigned long HSB_SFR9;
unsigned long HSB_SFR10;
unsigned long HSB_SFR11;
unsigned long HSB_SFR12;
unsigned long HSB_SFR13;
unsigned long HSB_SFR14;
} AVR32_HSBMATRIX;


typedef struct stAVR32_FLASHC
{
unsigned long FC_FCR;
unsigned long FC_CMD;
unsigned long FC_FSR;
unsigned long FC_FGPFRHI;
unsigned long FC_FGPFRLO;
} AVR32_FLASHC;

typedef struct stAVR32_MACB
{
unsigned long       MACB_NCR;     
unsigned long       MACB_NCFG;     
unsigned long       MACB_NSR;      
unsigned long       MACB_RES1;     
unsigned long       MACB_RES2;     
unsigned long       MACB_TSR;      
unsigned long       MACB_RBQP;     
unsigned long       MACB_TBQP;     
unsigned long       MACB_RSR;      
unsigned long       MACB_ISR;      
unsigned long       MACB_IER;      
unsigned long       MACB_IDR;      
unsigned long       MACB_IMR;      
unsigned long       MACB_MAN;
unsigned long       MACB_PTR;   
unsigned long       MACB_PFR;   
unsigned long       MACB_FTO;   
unsigned long       MACB_SCF;   
unsigned long       MACB_MCF;   
unsigned long       MACB_FRO;   
unsigned long       MACB_FCSE;  
unsigned long       MACB_ALE;   
unsigned long       MACB_DTF;   
unsigned long       MACB_LCOL;  
unsigned long       MACB_ECOL;  
unsigned long       MACB_TUND;  
unsigned long       MACB_CSE;   
unsigned long       MACB_RRE;   
unsigned long       MACB_ROV;   
unsigned long       MACB_RSE;   
unsigned long       MACB_ELE;   
unsigned long       MACB_RJA;   
unsigned long       MACB_USF;   
unsigned long       MACB_STE;   
unsigned long       MACB_RLE;   
unsigned long       MACB_TPF;  
unsigned long       MACB_HRB;   
unsigned long       MACB_HRT;   
unsigned long       MACB_SA1B;  
unsigned long       MACB_SA1T;  
unsigned long       MACB_SA2B;  
unsigned long       MACB_SA2T;  
unsigned long       MACB_SA3B;  
unsigned long       MACB_SA3T;  
unsigned long       MACB_SA4B;  
unsigned long       MACB_SA4T;  
unsigned long       MACB_TID;   
unsigned long       MACB_TPQ;  
unsigned long       MACB_USRIO; 
unsigned long       MACB_WOL;
} AVR32_MACB;


typedef struct stAVR32_SMC
{
unsigned long SMC_SETUP;
unsigned long SMC_PULSE;
unsigned long SMC_CYCLE;
unsigned long SMC_MODE;
} AVR32_SMC;



typedef struct stAVR32_SDRAM                                             // {2}
{
unsigned long SDRAMC_MR;
unsigned long SDRAMC_TR;
unsigned long SDRAMC_CR;
unsigned long SDRAMC_HSR;
unsigned long SDRAMC_LPR;
unsigned long SDRAMC_IER;
unsigned long SDRAMC_IDR;
unsigned long SDRAMC_IMR;
unsigned long SDRAMC_ISR;
unsigned long SDRAMC_MDR;
} AVR32_SDRAM;



typedef struct stAVR32_PDCA
{
unsigned long DMA_MAR;
unsigned long DMA_PSR;
unsigned long DMA_TCR;
unsigned long DMA_MARR;
unsigned long DMA_TCRR;
unsigned long DMA_CR;
unsigned long DMA_MR;
unsigned long DMA_SR;
unsigned long DMA_IER;
unsigned long DMA_IDR;
unsigned long DMA_IMR;
unsigned long DMA_ISR;
unsigned long ucRes[4];
} AVR32_PDCA;

typedef struct stAVR32_INTC
{
unsigned long IPR0;
unsigned long IPR1;
unsigned long IPR2;
unsigned long IPR3;
unsigned long IPR4;
unsigned long IPR5;
unsigned long IPR6;
unsigned long IPR7;
unsigned long IPR8;
unsigned long IPR9;
unsigned long IPR10;
unsigned long IPR11;
unsigned long IPR12;
unsigned long IPR13;
unsigned long IPR14;
unsigned long IPR15;
unsigned long IPR16;
unsigned long IPR17;
unsigned long IPR18;
unsigned long IPR19;
unsigned long IPR20;
unsigned long IPR21;
unsigned long IPR22;
unsigned long IPR23;
unsigned long IPR24;
unsigned long IPR25;
unsigned long IPR26;
unsigned long IPR27;
unsigned long IPR28;
unsigned long IPR29;
unsigned long IPR30;
unsigned long IPR31;
unsigned long IPR32;
unsigned long IPR33;
unsigned long IPR34;
unsigned long IPR35;
unsigned long IPR36;
unsigned long IPR37;
unsigned long IPR38;
unsigned long IPR39;
unsigned long IPR40;
unsigned long IPR41;
unsigned long IPR42;
unsigned long IPR43;
unsigned long IPR44;
unsigned long IPR45;
unsigned long IPR46;
unsigned long IPR47;
unsigned long IPR48;
unsigned long IPR49;
unsigned long IPR50;
unsigned long IPR51;
unsigned long IPR52;
unsigned long IPR53;
unsigned long IPR54;
unsigned long IPR55;
unsigned long IPR56;
unsigned long IPR57;
unsigned long IPR58;
unsigned long IPR59;
unsigned long IPR60;
unsigned long IPR61;
unsigned long IPR62;
unsigned long IPR63;
unsigned long IRR0;
unsigned long IRR1;
unsigned long IRR2;
unsigned long IRR3;
unsigned long IRR4;
unsigned long IRR5;
unsigned long IRR6;
unsigned long IRR7;
unsigned long IRR8;
unsigned long IRR9;
unsigned long IRR10;
unsigned long IRR11;
unsigned long IRR12;
unsigned long IRR13;
unsigned long IRR14;
unsigned long IRR15;
unsigned long IRR16;
unsigned long IRR17;
unsigned long IRR18;
unsigned long IRR19;
unsigned long IRR20;
unsigned long IRR21;
unsigned long IRR22;
unsigned long IRR23;
unsigned long IRR24;
unsigned long IRR25;
unsigned long IRR26;
unsigned long IRR27;
unsigned long IRR28;
unsigned long IRR29;
unsigned long IRR30;
unsigned long IRR31;
unsigned long IRR32;
unsigned long IRR33;
unsigned long IRR34;
unsigned long IRR35;
unsigned long IRR36;
unsigned long IRR37;
unsigned long IRR38;
unsigned long IRR39;
unsigned long IRR40;
unsigned long IRR41;
unsigned long IRR42;
unsigned long IRR43;
unsigned long IRR44;
unsigned long IRR45;
unsigned long IRR46;
unsigned long IRR47;
unsigned long IRR48;
unsigned long IRR49;
unsigned long IRR50;
unsigned long IRR51;
unsigned long IRR52;
unsigned long IRR53;
unsigned long IRR54;
unsigned long IRR55;
unsigned long IRR56;
unsigned long IRR57;
unsigned long IRR58;
unsigned long IRR59;
unsigned long IRR60;
unsigned long IRR61;
unsigned long IRR62;
unsigned long IRR63;
unsigned long ICR3;
unsigned long ICR2;
unsigned long ICR1;
unsigned long ICR0;
} AVR32_INTC;



typedef struct stAVR32_PM
{
unsigned long PM_MCCTRL;
unsigned long PM_CKSEL;
unsigned long PM_CPUMASK;
unsigned long PM_HSBMASK;
unsigned long PM_PBAMASK;
unsigned long PM_PBBMASK;
unsigned long ulRes0[2];
unsigned long PM_PLL0;
unsigned long PM_PLL1;
unsigned long PM_OSCCTRL0;
unsigned long PM_OSCCTRL1;
unsigned long PM_OSCCTRL32;
unsigned long ulRes1[4];
unsigned long PM_IER;
unsigned long PM_IDR;
unsigned long PM_IMR;
unsigned long PM_ISR;
unsigned long PM_ICR;
unsigned long PM_POSCSR;
unsigned long ulRes2[2];
unsigned long PM_GCCTRL; 
unsigned long ulRes3[22];
unsigned long PM_RCCR;
unsigned long PM_BGCR;
unsigned long PM_VREGCR;
unsigned long ulRes4;
unsigned long PM_BOD;
unsigned long ulRes5[26];
unsigned long PM_RCAUSE;
unsigned long ulRes6[46];
unsigned long PM_GPLP0;
unsigned long PM_GPLP1;
} AVR32_PM;

#ifdef _AT32UC3C                                                         // {3}
typedef struct stAVR32_SCIF
{
unsigned long SCIF_IER;
unsigned long SCIF_IDR;
unsigned long SCIF_IMR;
unsigned long SCIF_ISR;
unsigned long SCIF_ICR;
unsigned long SCIF_PCLKSR;
unsigned long SCIF_UNLOCK;
unsigned long SCIF_PLL0;
unsigned long SCIF_PLL1;
unsigned long SCIF_OSCCTRL0;
unsigned long SCIF_OSCCTRL1;
unsigned long SCIF_BOD;
unsigned long SCIF_BGCR;
unsigned long SCIF_BOD33;
unsigned long SCIF_BOD50;
unsigned long SCIF_VREGCR;
unsigned long SCIF_VREGCTRL;
unsigned long SCIF_RCCR;
unsigned long SCIF_RCCR8;
unsigned long SCIF_OSCCTRL32;
unsigned long ulRes0[2];
unsigned long SCIF_RC120MCR;
unsigned long SCIF_GPLP0;
unsigned long SCIF_GPLP1;
unsigned long SCIF_GCCTRL0;
unsigned long SCIF_GCCTRL1;
unsigned long SCIF_GCCTRL2;
unsigned long ulRes1;
unsigned long SCIF_GCCTRL4;
unsigned long SCIF_GCCTRL5;
unsigned long SCIF_GCCTRL6;
unsigned long SCIF_GCCTRL7;
unsigned long SCIF_GCCTRL8;
unsigned long SCIF_GCCTRL9;
unsigned long SCIF_GCCTRL10;
unsigned long SCIF_GCCTRL11;
unsigned long ulRes2[206];
unsigned long SCIF_PLLVERSION;
unsigned long SCIF_OSCVERSION;
unsigned long SCIF_BODVERSION;
unsigned long SCIF_BODBVERSION;
unsigned long SCIF_VREGVERSION;
unsigned long SCIF_RCCRVERSION;
unsigned long SCIF_RCCR8VERSION;
unsigned long SCIF_OSC32VERSION;
unsigned long ulRes3[2];
unsigned long SCIF_RC120MVERSION;
unsigned long SCIF_GPLPVERSION;
unsigned long SCIF_GCLKVERSION;
unsigned long SCIF_VERSION;
} AVR32_SCIF;
#else
typedef struct stAVR32_RTC                                               // {4}
{
unsigned long RTC_CTRL;
unsigned long RTC_VAL;
unsigned long RTC_TOP;
unsigned long ulRes0;
unsigned long RTC_IER;
unsigned long RTC_IDR;
unsigned long RTC_IMR;
unsigned long RTC_ISR;
unsigned long RTC_ICR;
} AVR32_RTC;
#endif

typedef struct stAVR32_WDT
{
unsigned long WDT_CTRL;
unsigned long WDT_CLR;
} AVR32_WDT;


typedef struct stAVR32_EIC
{
unsigned long EIC_IER;
unsigned long EIC_IDR;
unsigned long EIC_IMR;
unsigned long EIC_ISR;
unsigned long EIC_ICR;
unsigned long EIC_MODE;
unsigned long EIC_EDGE;
unsigned long EIC_LEVEL;
unsigned long EIC_FILTER;
unsigned long EIC_TEST;
unsigned long EIC_SYNC;
unsigned long EIC_SCAN;
unsigned long EIC_EN;
unsigned long EIC_DIS;
unsigned long EIC_CTRL;
} AVR32_EIC;



typedef struct stAVR32_GPIO
{
unsigned long GPER;
unsigned long GPERS;
unsigned long GPERC; 
unsigned long GPERT; 

unsigned long PMR0;  
unsigned long PMR0S; 
unsigned long PMR0C; 
unsigned long PMR0T; 

unsigned long PMR1;  
unsigned long PMR1S; 
unsigned long PMR1C; 
unsigned long PMR1T; 

#ifdef _AT32UC3C                                                         // {3}
    unsigned long PMR2;  
    unsigned long PMR2S; 
    unsigned long PMR2C; 
    unsigned long PMR2T; 
#else
    unsigned long ulRes0[4];
#endif

unsigned long ODER;   
unsigned long ODERS;  
unsigned long ODERC;  
unsigned long ODERT;  

unsigned long OVR;    
unsigned long OVRS;   
unsigned long OVRC;   
unsigned long OVRT;   

unsigned long PVR;    
unsigned long ulRes1[3];

unsigned long PUER;
unsigned long PUERS;
unsigned long PUERC;
unsigned long PUERT;

#ifdef _AT32UC3C                                                         // {5}
    unsigned long PDER;
    unsigned long PDERS;
    unsigned long PDERC;
    unsigned long PDERT;
#else
    unsigned long ODMER;  
    unsigned long ODMERS; 
    unsigned long ODMERC; 
    unsigned long ODMERT;
#endif

unsigned long IER;    
unsigned long IERS;   
unsigned long IERC;   
unsigned long IERT;   

unsigned long IMR0;   
unsigned long IMR0S;  
unsigned long IMR0C;  
unsigned long IMR0T;  

unsigned long IMR1;   
unsigned long IMR1S;  
unsigned long IMR1C;  
unsigned long IMR1T;  

unsigned long GFER;   
unsigned long GFERS;  
unsigned long GFERC;  
unsigned long GFERT;  

unsigned long IFR;    
unsigned long ulRes2;
unsigned long IFRC;   
unsigned long ulRes3[9];

#ifdef _AT32UC3C                                                         // {3}
    unsigned long ODCR0;
    unsigned long ODCR0S;
    unsigned long ODCR0C;
    unsigned long ODCR0T;
    unsigned long ODCR1;
    unsigned long ODCR1S;
    unsigned long ODCR1C;
    unsigned long ODCR1T;
    unsigned long ulRes4[32];
    unsigned long LOCK;
    unsigned long LOCKS;
    unsigned long LOCKC;
    unsigned long LOCKT;
    unsigned long ulRes5[12];
    unsigned long UNLOCKS;
    unsigned long ASR;
    unsigned long ulRes6[4];
    unsigned long PARAMETER;
    unsigned long VERSION;
#endif
} AVR32_GPIO;

typedef struct stAVR32_USART
{
unsigned long       US_CR;
unsigned long       US_MR;      
unsigned long       US_IER;     
unsigned long       US_IDR;     
unsigned long       US_IMR;     
unsigned long       US_CSR;     
unsigned long       US_RHR;     
unsigned long       US_THR;     
unsigned long       US_BRGR;    
unsigned long       US_RTOR;    
unsigned long       US_TTGR;    
unsigned long       US_RES1;    
unsigned long       US_RES2;    
unsigned long       US_RES3;    
unsigned long       US_RES4;    
unsigned long       US_RES5;    
unsigned long       US_FIDI;    
unsigned long       US_NER;     
unsigned long       US_RES6;    
unsigned long       US_IF;           
unsigned long       US_MAN;     

unsigned long       US_RESERVED[43];
unsigned long       US_VERSION;

unsigned long       US_RPR;
unsigned long       US_RCR;
unsigned long       US_TPR;
unsigned long       US_TCR;
unsigned long       US_RNPR;
unsigned long       US_RNCR;
unsigned long       US_TNPR;
unsigned long       US_TNCR;
unsigned long       US_PTCR;
unsigned long       US_PTSR;
} AVR32_USART;


typedef struct stAVR32_SPI
{
unsigned long       SPI_CR;
unsigned long       SPI_MR;
unsigned long       SPI_RDR;
unsigned long       SPI_TDR;
unsigned long       SPI_SR;
unsigned long       SPI_IER;
unsigned long       SPI_IDR;
unsigned long       SPI_IMR;
unsigned long       ucRes1[4];
unsigned long       SPI_CSR0;
unsigned long       SPI_CSR1;
unsigned long       SPI_CSR2;
unsigned long       SPI_CSR3;
} AVR32_SPI;


typedef struct stAVR32_TWI
{
      unsigned long TWI_CR;
const unsigned long TWI_MMR;   
      unsigned long TWI_SMR;
      unsigned long TWI_IADR;   
      unsigned long TWI_CWGR;   
const unsigned long ulRes0[3];
const unsigned long TWI_SR;     
      unsigned long TWI_IER;    
      unsigned long TWI_IDR;    
const unsigned long TWI_IMR;    
const unsigned long TWI_RHR;    
      unsigned long TWI_THR;
const unsigned long ulRes1[49];      
      unsigned long TWI_VER;
} AVR32_TWI;

typedef struct stAVR32_TIMER
{
unsigned long       TC_CCR;
unsigned long       TC_CMR;
unsigned long       Res_1[2];
const unsigned long TC_CV;
unsigned long       TC_RA;
unsigned long       TC_RB;
unsigned long       TC_RC;
unsigned long       TC_SR;
unsigned long       TC_IER;
unsigned long       TC_IDR;
unsigned long       TC_IMR;
unsigned long       Res_2[4];
} AVR32_TIMER;

typedef struct stAVR32_TIMER_CTL
{
unsigned long       TC_BCR;
unsigned long       TC_BMR;
} AVR32_TIMER_CTL;

typedef struct stAVR32_ADC
{
unsigned long       ADC_CR;
unsigned long       ADC_MR;   
unsigned long       ADC_RES1; 
unsigned long       ADC_RES2; 
unsigned long       ADC_CHER; 
unsigned long       ADC_CHDR; 
unsigned long       ADC_CHSR; 
unsigned long       ADC_SR;   
unsigned long       ADC_LCDR; 
unsigned long       ADC_IER;  
unsigned long       ADC_IDR;  
unsigned long       ADC_IMR;  
unsigned long       ADC_CDR0; 
unsigned long       ADC_CDR1; 
unsigned long       ADC_CDR2; 
unsigned long       ADC_CDR3; 
unsigned long       ADC_CDR4; 
unsigned long       ADC_CDR5; 
unsigned long       ADC_CDR6; 
unsigned long       ADC_CDR7; 
} AVR32_ADC;



// A copy of the AVR32 peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)

typedef struct stAVR32_PERIPH
{
    AVR32_USBB       ucUSB;                                              // {1}
    AVR32_HSBMATRIX  ucHSBMatrix;
    AVR32_FLASHC     ucFlashc;
    AVR32_MACB       ucMACB;
    AVR32_SMC        ucSMC[4];
    AVR32_SDRAM      ucSDRAM;                                            // {2}
    AVR32_PDCA       ucPDCA[15];
    AVR32_INTC       ucIntc;
    AVR32_PM         ucPower;
#ifdef _AT32UC3C                                                         // {3}
    AVR32_SCIF       ucSCIF;    
#else
    AVR32_RTC        ucRTC;                                              // {4}
#endif
    AVR32_WDT        ucWDT;
    AVR32_EIC        ucEIC;
    AVR32_GPIO       ucGPIO[4];
#ifdef _AT32UC3C                                                         // {3}
    AVR32_USART      ucSimUart[5];
#else
    AVR32_USART      ucSimUart[4];
#endif
    AVR32_SPI        ucSimSPI[2];
    AVR32_TWI        ucTWI;
    AVR32_TIMER      ucTimer0;
    AVR32_TIMER      ucTimer1;
    AVR32_TIMER      ucTimer2;
    AVR32_TIMER_CTL  ucTimerCtl;
    AVR32_ADC        ucADC;
} AVR32_PERIPH;

extern AVR32_PERIPH  ucAVR32;


// System Registers                                                      accessed directly only by simulator
//                                                                       processor accesses them using the mtsr and mfsr commands
typedef struct stAVR32_SYSTEM_REGISTERS
{
    unsigned long SR;                                                    // 0   - Status Register
    unsigned long EVBA;                                                  // 4   - Exception Vector Base Address
    unsigned long ACBA;                                                  // 8   - Application Call Base Address
    unsigned long CPUCR;                                                 // 12  - CPU Control Register
    unsigned long ECR;                                                   // 16  - Exception Cause Register
    unsigned long RSR_SUP;                                               // 20  - Unused in AVR32UC
    unsigned long RSR_INT0;                                              // 24  - Unused in AVR32UC
    unsigned long RSR_INT1;                                              // 28  - Unused in AVR32UC
    unsigned long RSR_INT2;                                              // 32  - Unused in AVR32UC
    unsigned long RSR_INT3;                                              // 36  - Unused in AVR32UC
    unsigned long RSR_EX;                                                // 40  - Unused in AVR32UC
    unsigned long RSR_NMI;                                               // 44  - Unused in AVR32UC
    unsigned long RSR_DBG;                                               // 48  - Return Status Register for Debug Mode
    unsigned long RAR_SUP;                                               // 52  - Unused in AVR32UC
    unsigned long RAR_INT0;                                              // 56  - Unused in AVR32UC
    unsigned long RAR_INT1;                                              // 60  - Unused in AVR32UC
    unsigned long RAR_INT2;                                              // 64  - Unused in AVR32UC
    unsigned long RAR_INT3;                                              // 68  - Unused in AVR32UC
    unsigned long RAR_EX;                                                // 72  - Unused in AVR32UC
    unsigned long RAR_NMI;                                               // 76  - Unused in AVR32UC
    unsigned long RAR_DBG;                                               // 80  - Return Status Register for Debug Mode
    unsigned long RAR_JECR;                                              // 84  - Unused in AVR32UC
    unsigned long RAR_JOSP;                                              // 88  - Unused in AVR32UC
    unsigned long JAVA_LV0;                                              // 92  - Unused in AVR32UC
    unsigned long JAVA_LV1;                                              // 96  - Unused in AVR32UC
    unsigned long JAVA_LV2;                                              // 100 - Unused in AVR32UC
    unsigned long JAVA_LV3;                                              // 104 - Unused in AVR32UC
    unsigned long JAVA_LV4;                                              // 108 - Unused in AVR32UC
    unsigned long JAVA_LV5;                                              // 112 - Unused in AVR32UC
    unsigned long JAVA_LV6;                                              // 116 - Unused in AVR32UC
    unsigned long JAVA_LV7;                                              // 120 - Unused in AVR32UC
    unsigned long JTBA;                                                  // 124 - Unused in AVR32UC
    unsigned long JBCR;                                                  // 128 - Unused in AVR32UC
    unsigned long ulRes0[30];                                            // 132-254 - reserved for future use
    unsigned long CONFIG0;                                               // 256 - Configuration Register 0
    unsigned long CONFIG1;                                               // 260 - Configuration Register 1
    unsigned long COUNT;                                                 // 264 - Cycle Counter Register
    unsigned long COMPARE;                                               // 268 - Compare Register
    unsigned long TLBEHI;                                                // 272 - Unused in AVR32UC
    unsigned long TLBELO;                                                // 276 - Unused in AVR32UC
    unsigned long PTBR;                                                  // 280 - Unused in AVR32UC
    unsigned long TLBEAR;                                                // 284 - Unused in AVR32UC
    unsigned long MMUCR;                                                 // 288 - Unused in AVR32UC
    unsigned long TLBARLO;                                               // 292 - Unused in AVR32UC
    unsigned long TLBARHI;                                               // 296 - Unused in AVR32UC
    unsigned long PCCNT;                                                 // 300 - Unused in AVR32UC
    unsigned long PCNT0;                                                 // 304 - Unused in AVR32UC
    unsigned long PCNT1;                                                 // 308 - Unused in AVR32UC
    unsigned long PCCR;                                                  // 312 - Unused in AVR32UC
    unsigned long BEAR;                                                  // 316 - Bus Error Address Register
    unsigned long MPUAR0;                                                // 320 - MCU Address Register Region 0
    unsigned long MPUAR1;                                                // 324 - MCU Address Register Region 1
    unsigned long MPUAR2;                                                // 328 - MCU Address Register Region 2
    unsigned long MPUAR3;                                                // 332 - MCU Address Register Region 3
    unsigned long MPUAR4;                                                // 336 - MCU Address Register Region 4
    unsigned long MPUAR5;                                                // 340 - MCU Address Register Region 5
    unsigned long MPUAR6;                                                // 344 - MCU Address Register Region 6
    unsigned long MPUAR7;                                                // 348 - MCU Address Register Region 7
    unsigned long MPUPSR0;                                               // 352 - MCU Privilege Select Register Region 0
    unsigned long MPUPSR1;                                               // 356 - MCU Privilege Select Register Region 1
    unsigned long MPUPSR2;                                               // 360 - MCU Privilege Select Register Region 2
    unsigned long MPUPSR3;                                               // 364 - MCU Privilege Select Register Region 3
    unsigned long MPUPSR4;                                               // 368 - MCU Privilege Select Register Region 4
    unsigned long MPUPSR5;                                               // 372 - MCU Privilege Select Register Region 5
    unsigned long MPUPSR6;                                               // 376 - MCU Privilege Select Register Region 6
    unsigned long MPUPSR7;                                               // 380 - MCU Privilege Select Register Region 7
    unsigned long MPUCRA;                                                // 384 - MCU Cacheable Register A
    unsigned long MPUCRB;                                                // 388 - MCU Cacheable Register B
    unsigned long MPUBRA;                                                // 392 - MCU Bufferable Register A
    unsigned long MPUBRB;                                                // 396 - MCU Bufferable Register B
    unsigned long MPUAPRA;                                               // 400 - MCU Access Permission Register A
    unsigned long MPUAPRB;                                               // 404 - MCU Access Permission Register B
    unsigned long MPUCR;                                                 // 408 - MCU Control Register
    unsigned long ulRes1[88];                                            // 412-764 - reserved for future use
    unsigned long ulRes2[33];                                            // 768-1020 - implementation defined
} AVR32_SYSTEM_REGISTERS;

// Status Register bits
//
#define GM           0x00010000
#define EM           0x00200000
#define MODE1        0x00400000

extern AVR32_SYSTEM_REGISTERS ulAVR32_CPU;

#ifdef _AT32UC3C
    extern unsigned long AVR32_ExceptionRAM[65 + 54];
#else
    extern unsigned long AVR32_ExceptionRAM[65];
#endif


#define _GPIO_0                         0                                // References for display in simulator
#define _GPIO_1                         1
#define _GPIO_2                         2
#define _GPIO_3                         3

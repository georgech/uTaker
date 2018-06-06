/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      SimM5223X.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    10.06.2007 Added Random Number Generator Accelerator support
    25.06.2007 Add Edge port module registers
    11.08.2007 Add RTC registers / correct M5223X_QSPI content size       {1}
    09.09.2007 Add General Purpose Timer registers 
    22.09.2007 Add M5221X support
    12.10.2007 Add USB support
    04.05.2008 Add additional M5221X, M5222X and M5223X discrimination
    13.08.2008 Add M5225X support                                         {2}
    13.03.2009 Add PWM module                                             {3}
    25.03.2009 Add M521XX configuration
    27.03.2009 Add M521X configuration                                    {4}
    10.04.2010 Add M5214, M5216, M5280, M5281 and M5282                   {5}
    21.05.2010 Add M520X                                                  {6}
    17.06.2011 Add M523X                                                  {7}

*/  


typedef struct stM5223X_SCM
{
unsigned long MCF_SCM_IPSBAR;
unsigned long ulRes1;
unsigned long MCF_SCM_RAMBAR;
unsigned long PPMRH;
unsigned char CRSR;
unsigned char CWCR;
unsigned char LPICR;
volatile unsigned char CWSR;
unsigned long DMAREQC;
unsigned long PPMRL;
unsigned long MPARK;
unsigned char MPR;
unsigned char PPMRS;
unsigned char PPMRC;
unsigned char IPSBMT;
unsigned char PACR0;
unsigned char PACR1;
unsigned char PACR2;
unsigned char PACR3;
unsigned char PACR4;
unsigned char PACR5;
unsigned char PACR6;
unsigned char PACR7;
unsigned char PACR8;
unsigned char ucRes2;
unsigned char ucRes3;
unsigned char ucRes4;
unsigned char GPACR0;
unsigned char GPACR1;
} M5223X_SCM;


#if defined _M5225X || defined _M520X || defined _M523X                  // {2}{6}
typedef struct stM5225X_FLEXBUS
{
unsigned long CSAR0;
unsigned long CSMR0;
unsigned long CSCR0;
unsigned long CSAR1;
unsigned long CSMR1;
unsigned long CSCR1;
#if defined _M520X                                                       // {6}
    unsigned long CSAR2;
    unsigned long CSMR2;
    unsigned long CSCR2;
    unsigned long CSAR3;
    unsigned long CSMR3;
    unsigned long CSCR3;
    unsigned long CSAR4;
    unsigned long CSMR4;
    unsigned long CSCR4;
    unsigned long CSAR5;
    unsigned long CSMR5;
    unsigned long CSCR5;
#endif
} M5225X_FLEXBUS;
#endif

typedef struct stM5223X_DMA
{
unsigned long DMA_SAR;
unsigned long DMA_DAR;
unsigned long DMA_SR_BCR;
unsigned long DMA_DCR;
} M5223X_DMA;


typedef struct stM5223X_UART
{
unsigned char UMR1_2;
unsigned char ucRes1[3];
unsigned char USR_UCSR;
unsigned char ucRes2[3];
unsigned char UCR;
unsigned char ucRes3[3];
unsigned char UTB_RB;
unsigned char ucRes4[3];
unsigned char UIPCR_UACR;
unsigned char ucRes5[3];
unsigned char UIMR_UISR;
unsigned char ucRes6[3];
unsigned char UBG1;
unsigned char ucRes7[3];
unsigned char UBG2;
unsigned char ucRes8[23];
volatile const unsigned char UIP;
unsigned char ucRes9[3];
unsigned char UOP1;
unsigned char ucRes10[3];
unsigned char UOP0;
unsigned char ucRes11[3];
#ifdef _M520X
    unsigned char ucRes12[0x4000 - 0x40];
#endif
} M5223X_UART;

typedef struct stM5223X_I2C
{
unsigned char  I2ADR;
unsigned char  ucRes1[3];
unsigned char  I2FDR;
unsigned char  ucRes2[3];
unsigned char  I2CR;
unsigned char  ucRes3[3];
unsigned char  I2SR;
unsigned char  ucRes4[3];
volatile unsigned char  I2DR;
unsigned char  ucRes5[3];
} M5223X_I2C;

typedef struct stM5223X_QSPI                                             // {1}
{
unsigned short QMR;
unsigned short usRes1;
unsigned short QDLYR;
unsigned short usRes2;
unsigned short QWR;
unsigned short usRes3;
unsigned short QIR;
unsigned short usRes4;
unsigned short QAR;
unsigned short usRes5;
unsigned short QDR;
} M5223X_QSPI;

typedef struct stM5223X_RTC
{
unsigned long  HOURMIN;
unsigned long  SECONDS;
unsigned long  ALRM_HM;
unsigned long  ALRM_SEC;
unsigned long  RTCCTL;
unsigned long  RTCISR;
unsigned long  RTCIENR;
unsigned long  STPWCH;
unsigned short usRes1;
unsigned short DAYS;
unsigned long  ALRM_DAY;
#if defined _M5221X || defined _M5225X || defined _M521XX
unsigned long  usRes2[3];
unsigned short RTCGOCU;
unsigned short usRes3;
unsigned short RTCGOCL;
unsigned short usRes4;
#endif
} M5223X_RTC;


typedef struct stM5223X_DMA_TIMER
{
unsigned short DTMR;
unsigned char  DTXMR;
unsigned char  DTER;
unsigned long  DTRR;
unsigned long  DTCR;
const unsigned long DTCN;
unsigned long  ulRes1[12];
} M5223X_DMA_TIMER;

typedef struct stM5223X_INT_CTRL
{
unsigned long IPRH;
unsigned long IPRL;
unsigned long IMRH;
unsigned long IMRL;
unsigned long INTFRCH;
unsigned long INTFRCL;
unsigned char IRLR;
unsigned char IACKLPR;
unsigned char ucRes1[39];
unsigned char ICR[63];
unsigned long ulRes2[24];
unsigned long SWIACK;
unsigned long L1IACK;
unsigned long L2IACK;
unsigned long L3IACK;
unsigned long L4IACK;
unsigned long L5IACK;
unsigned long L6IACK;
unsigned long L7IACK;
} M5223X_INT_CTRL;

#if defined _M52XX_SDRAM                                                 // {5}
typedef struct stM5223X_PORT
{
unsigned char PORTA;
unsigned char PORTB;
unsigned char PORTC;
unsigned char PORTD;
unsigned char PORTE;
unsigned char PORTF;
unsigned char PORTG;
unsigned char PORTH;
unsigned char PORTJ;
unsigned char PORTDD;
    #ifdef _M521X_SDRAM
unsigned char ucRes0;
    #else
unsigned char PORTEH;
    #endif
unsigned char PORTEL;
unsigned char PORTAS;
unsigned char PORTQS;
unsigned char PORTSD;
unsigned char PORTTC;
unsigned char PORTTD;
unsigned char PORTUA;
unsigned char ucRes1[2];
unsigned char DDRA;
unsigned char DDRB;
unsigned char DDRC;
unsigned char DDRD;
unsigned char DDRE;
unsigned char DDRF;
unsigned char DDRG;
unsigned char DDRH;
unsigned char DDRJ;
unsigned char DDRDD;
    #ifdef _M521X_SDRAM
unsigned char ucRes2;
    #else
unsigned char DDREH;
    #endif
unsigned char DDREL;
unsigned char DDRAS;
unsigned char DDRQS;
unsigned char DDRSD;
unsigned char DDRTC;
unsigned char DDRTD;
unsigned char DDRUA;
unsigned char ucRes3[2];
unsigned char PORT_SETA;
unsigned char PORT_SETB;
unsigned char PORT_SETC;
unsigned char PORT_SETD;
unsigned char PORT_SETE;
unsigned char PORT_SETF;
unsigned char PORT_SETG;
unsigned char PORT_SETH;
unsigned char PORT_SETJ;
unsigned char PORT_SETDD;
    #ifdef _M521X_SDRAM
unsigned char ucRes4;
    #else
unsigned char PORT_SETEH;
    #endif
unsigned char PORT_SETEL;
unsigned char PORT_SETAS;
unsigned char PORT_SETQS;
unsigned char PORT_SETSD;
unsigned char PORT_SETTC;
unsigned char PORT_SETTD;
unsigned char PORT_SETUA;
unsigned char ucRes5[2];
unsigned char CLEARA;
unsigned char CLEARB;
unsigned char CLEARC;
unsigned char CLEARD;
unsigned char CLEARE;
unsigned char CLEARF;
unsigned char CLEARG;
unsigned char CLEARH;
unsigned char CLEARJ;
unsigned char CLEARDD;
    #ifdef _M521X_SDRAM
unsigned char ucRes6;
    #else
unsigned char CLEAREH;
    #endif
unsigned char CLEAREL;
unsigned char CLEARAS;
unsigned char CLEARQS;
unsigned char CLEARSD;
unsigned char CLEARTC;
unsigned char CLEARTD;
unsigned char CLEARUA;
unsigned char ucRes7[2];
unsigned char PBCDPAR;
unsigned char PFPAR;
unsigned short PEPAR;
unsigned char PJPAR;
unsigned char PSDPAR;
unsigned short PASPAR;
unsigned char PEHLPAR;
unsigned char PQSPAR;
unsigned char PTCPAR;
unsigned char PTDPAR;
unsigned char PUAPAR;
} M5223X_PORT;
#elif defined _M521X                                                     // {4}
typedef struct stM5223X_PORT
{
unsigned long ulRes1[2];
unsigned char PORTNQ;
unsigned char PORTDD;
unsigned char PORTAN;
unsigned char PORTAS;
unsigned char ucRes2;
unsigned char PORTQS;
unsigned char PORTTA;
unsigned char PORTTC;
unsigned char PORTTD;
unsigned char PORTUA;
unsigned char PORTUB;
unsigned char PORTUC;
unsigned long ulRes3[2];
unsigned char DDRNQ;
unsigned char DDRDD;
unsigned char DDRAN;
unsigned char DDRAS;
unsigned char ucRes4;
unsigned char DDRQS;
unsigned char DDRTA;
unsigned char DDRTC;
unsigned char DDRTD;
unsigned char DDRUA;
unsigned char DDRUB;
unsigned char DDRUC;
unsigned long ulRes5[2];
unsigned char PORT_SETNQ;
unsigned char PORT_SETDD;
unsigned char PORT_SETAN;
unsigned char PORT_SETAS;
unsigned char ucRes6;
unsigned char PORT_SETQS;
unsigned char PORT_SETTA;
unsigned char PORT_SETTC;
unsigned char PORT_SETTD;
unsigned char PORT_SETUA;
unsigned char PORT_SETUB;
unsigned char PORT_SETUC;
unsigned long ulRes7[2];
unsigned char CLEARNQ;
unsigned char CLEARDD;
unsigned char CLEARAN;
unsigned char CLEARAS;
unsigned char ucRes8;
unsigned char CLEARQS;
unsigned char CLEARTA;
unsigned char CLEARTC;
unsigned char CLEARTD;
unsigned char CLEARUA;
unsigned char CLEARUB;
unsigned char CLEARUC;
unsigned char PNQPAR;
unsigned char PDDPAR;
unsigned char PANPAR;
unsigned char PASPAR;
unsigned short PQSPAR;
unsigned char PTAPAR;
unsigned char PTCPAR;
unsigned char PTDPAR;
unsigned char PUAPAR;
unsigned char PUBPAR;
unsigned char PUCPAR;
unsigned long ulRes9[7];
unsigned long PSRR;
unsigned long PDSR;
} M5223X_PORT;
#elif defined _M520X                                                     // {6}
typedef struct stM5223X_PORT
{
unsigned char PODR_BUSCTL;
unsigned char PODR_BE;
unsigned char PODR_CS;
unsigned char PODR_FECI2C;
unsigned char PODR_QSPI;
unsigned char PODR_TIMER;
unsigned char PODR_UART;
unsigned char PODR_FECH;
unsigned char PODR_FECL;
unsigned char ucRes1[3];
unsigned char PDDR_BUSCTL;
unsigned char PDDR_BE;
unsigned char PDDR_CS;
unsigned char PDDR_FECI2C;
unsigned char PDDR_QSPI;
unsigned char PDDR_TIMER;
unsigned char PDDR_UART;
unsigned char PDDR_FECH;
unsigned char PDDR_FECL;
unsigned char ucRes2[5];
unsigned char PORT_SETCS;
unsigned char PORT_SETFECI2C;
unsigned char PORT_SETQSPI;
unsigned char PORT_SETTIMER;
unsigned char PORT_SETUART;
unsigned char PORT_SETFECH;
unsigned char PORT_SETFECL;
unsigned char ucRes3[3];
unsigned char PCLRR_BUSCTL;
unsigned char PCLRR_BE;
unsigned char PCLRR_CS;
unsigned char PCLRR_FECI2C;
unsigned char PCLRR_QSPI;
unsigned char PCLRR_TIMER;
unsigned char PCLRR_UART;
unsigned char PCLRR_FECH;
unsigned char PCLRR_FECL;
unsigned char ucRes4[3];
unsigned char PAR_BUSCTL;
unsigned char PAR_BE;
unsigned char PAR_CS;
unsigned char PAR_FECI2C;
unsigned char PAR_QSPI;
unsigned char PAR_TIMER;
unsigned short PAR_UART;
unsigned char PAR_FEC;
unsigned char PAR_IRQ;
unsigned char MSCR_FLEXBUS;
unsigned char MSCR_SDRAM;
unsigned char DSCR_I2C;
unsigned char DSCR_MISC;
unsigned char DSCR_FEC;
unsigned char DSCR_UART;
unsigned char DSCR_QSPI;
} M5223X_PORT;

#elif defined _M523X                                                               // {7}
typedef struct stM5223X_PORT
{
unsigned char PORTADDR;
unsigned char PORTDATAH;
unsigned char PORTDATAL;
unsigned char PORTBUSCTL;
unsigned char PORTBS;
unsigned char PORTCS;
unsigned char PORTSDRAM;
unsigned char PORTFECI2C;
unsigned char PORTUARTH;
unsigned char PORTUARTL;
unsigned char PORTQSPI;
unsigned char PORTTIMER;
unsigned char PORTETPU;
unsigned char ucRes0[3];
unsigned char DDRADDR;
unsigned char DDRDATAH;
unsigned char DDRDATAL;
unsigned char DDRBUSCTL;
unsigned char DDRBS;
unsigned char DDRCS;
unsigned char DDRSDRAM;
unsigned char DDRFECI2C;
unsigned char DDRUARTH;
unsigned char DDRUARTL;
unsigned char DDRQSPI;
unsigned char DDRTIMER;
unsigned char DDRETPU;
unsigned char ucRes1[3];
unsigned char PORT_SETADDR;
unsigned char PORT_SETDATAH;
unsigned char PORT_SETDATAL;
unsigned char PORT_SETBUSCTL;
unsigned char PORT_SETBS;
unsigned char PORT_SETCS;
unsigned char PORT_SETSDRAM;
unsigned char PORT_SETFECI2C;
unsigned char PORT_SETUARTH;
unsigned char PORT_SETUARTL;
unsigned char PORT_SETQSPI;
unsigned char PORT_SETTIMER;
unsigned char PORT_SETETPU;
unsigned char ucRes2[3];
unsigned char PCLRR_ADDR;
unsigned char PCLRR_DATAH;
unsigned char PCLRR_DATAL;
unsigned char PCLRR_BUSCTL;
unsigned char PCLRR_BS;
unsigned char PCLRR_CS;
unsigned char PCLRR_SDRAM;
unsigned char PCLRR_FECI2C;
unsigned char PCLRR_UARTH;
unsigned char PCLRR_UARTL;
unsigned char PCLRR_QSPI;
unsigned char PCLRR_TIMER;
unsigned char PCLRR_ETPU;
unsigned char ucRes3[3];
unsigned char PAR_AD;
unsigned char ucRes4;
unsigned short PAR_BUSCTL;
unsigned char PAR_BS;
unsigned char PAR_CS;
unsigned char PAR_SDRAM;
unsigned char PAR_FECI2C;
unsigned short PAR_UART;
unsigned char PAR_QSPI;
unsigned char ucRes5;
unsigned short PAR_TIMER;
unsigned char PAR_ETPU;
unsigned char ucRes6[3];
unsigned char DSCR_EIM;
unsigned char DSCR_ETPU;
unsigned char DSCR_FECI2C;
unsigned char DSCR_UART;
unsigned char DSCR_QSPI;
unsigned char DSCR_TIMER;
} M5223X_PORT;
#else         // M5223X
typedef struct stM5223X_PORT
{
#if defined _M5225X                                                      // {2}
    unsigned char PORTTE;
    unsigned char PORTTF;
    unsigned char PORTTG;
    unsigned char PORTTH;
    unsigned char PORTTI;
    unsigned char ulRes1;
    unsigned char PORTTJ;
    unsigned char ulResa;
#else
    unsigned long ulRes1[2];
#endif
unsigned char PORTNQ;
unsigned char ucRes2;
unsigned char PORTAN;
unsigned char PORTAS;
unsigned char PORTQS;
unsigned char ucRes3;
unsigned char PORTTA;
unsigned char PORTTC;
#if defined _M5225X                                                      // {2}
    unsigned char ulRes3a;
#else
    unsigned char PORTTD;
#endif
unsigned char PORTUA;
unsigned char PORTUB;
unsigned char PORTUC;
unsigned char PORTDD;
#if defined _M5225X                                                      // {2}
    unsigned char ulRes3b;
    unsigned char ulRes3c;
#else
    unsigned char PORTLD;
    unsigned char PORTGP;
#endif
unsigned char ucRes4;
#if defined _M5225X                                                      // {2}
    unsigned char DDRTE;
    unsigned char DDRTF;
    unsigned char DDRTG;
    unsigned char DDRTH;
    unsigned char DDRTI;
    unsigned char ucRes4a;
    unsigned char DDRTJ;
    unsigned char ulRes4b;
#else
    unsigned long ulRes5[2];
#endif
unsigned char DDRNQ;
unsigned char ucRes6;
unsigned char DDRAN;
unsigned char DDRAS;
unsigned char DDRQS;
unsigned char ucRes7;
unsigned char DDRTA;
unsigned char DDRTC;
#if defined _M5225X                                                      // {2}
    unsigned char ulRes7a;
#else
    unsigned char DDRTD;
#endif
unsigned char DDRUA;
unsigned char DDRUB;
unsigned char DDRUC;
unsigned char DDRDD;
#if defined _M5225X                                                      // {2}
    unsigned char ulRes7b;
    unsigned char ulRes7c;
#else
unsigned char DDRLD;
unsigned char DDRGP;
#endif
unsigned char ucRes8;
#if defined _M5225X                                                      // {2}
    unsigned char PORT_SETTE;
    unsigned char PORT_SETTF;
    unsigned char PORT_SETTG;
    unsigned char PORT_SETTH;
    unsigned char PORT_SETTI;
    unsigned char ulRes8a;
    unsigned char PORT_SETTJ;
    unsigned char ulRes8b;
#else
    unsigned long ulRes9[2];
#endif
unsigned char PORT_SETNQ;
unsigned char ucRes10;
unsigned char PORT_SETAN;
unsigned char PORT_SETAS;
unsigned char PORT_SETQS;
unsigned char ucRes11;
unsigned char PORT_SETTA;
unsigned char PORT_SETTC;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes11a;
#else
    unsigned char PORT_SETTD;
#endif
unsigned char PORT_SETUA;
unsigned char PORT_SETUB;
unsigned char PORT_SETUC;
unsigned char PORT_SETDD;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes11b;
    unsigned char ucRes11c;
#else
    unsigned char PORT_SETLD;
    unsigned char PORT_SETGP;
#endif
unsigned char ucRes12;
#if defined _M5225X                                                      // {2}
    unsigned char CLEARTE;
    unsigned char CLEARTF;
    unsigned char CLEARTG;
    unsigned char CLEARTH;
    unsigned char CLEARTI;
    unsigned char ucRes12a;
    unsigned char CLEARTJ;
    unsigned char ulRes12b;
#else
    unsigned long ulRes13[2];
#endif
unsigned char CLEARNQ;
unsigned char ucRes14;
unsigned char CLEARAN;
unsigned char CLEARAS;
unsigned char CLEARQS;
unsigned char ucRes15;
unsigned char CLEARTA;
unsigned char CLEARTC;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes15a;
#else
    unsigned char CLEARTD;
#endif
unsigned char CLEARUA;
unsigned char CLEARUB;
unsigned char CLEARUC;
unsigned char CLEARDD;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes15b;
    unsigned char ucRes15c;
#else
    unsigned char CLEARLD;
    unsigned char CLEARGP;
#endif
unsigned char ucRes16;
#if defined _M5225X                                                      // {2}
    unsigned char PTEPAR;
    unsigned char PTFPAR;
    unsigned char PTGPAR;
    unsigned char ucRes_pthpar;
    unsigned char PTIPAR;
    unsigned char ucRes16a;
    unsigned char PTJPAR;
    unsigned char ulRes16b;
#else
    unsigned long ulRes17[2];
#endif
unsigned short PNQPAR;
unsigned char PANPAR;
unsigned char PASPAR;
unsigned short PQSPAR;
unsigned char PTAPAR;
unsigned char PTCPAR;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes17a;
#else
    unsigned char PTDPAR;
#endif
unsigned char PUAPAR;
unsigned char PUBPAR;
unsigned char PUCPAR;
unsigned char PDDPAR;
#if defined _M5225X                                                      // {2}
    unsigned char ucRes17b;
    unsigned char ucRes17c;
    unsigned char CLKOUTPAR;
    unsigned long PSRR;
    unsigned long PDSR;
    unsigned long PSRRL;
    unsigned short ucRes17d;
    unsigned short PSRRH;
    unsigned long PDSRL;
    unsigned short PDSRH;
    unsigned short ucRes17e;    
    unsigned short PTHPAR;
#else
    unsigned char PLDPAR;
    unsigned char PGPPAR;
    unsigned char ucRes18;
    unsigned short PWOR;
    unsigned short PDRR;
    unsigned long PDSR;
#endif
} M5223X_PORT;
#endif


typedef struct stM5223X_RESET
{
unsigned char RESET_RCR;
unsigned char RESET_RSR;
unsigned char res0[2];
unsigned short CCM_CCR;
#ifdef _M52XX_SDRAM
    unsigned short CCM_LPCR;
#else
    unsigned char res1;
    unsigned char CCM_LPCR;
#endif
unsigned short CCM_RCON;
unsigned short CCM_CIR;
#ifdef _M5225X
    unsigned short res2;
    unsigned short CCM_CCE;
#endif
} M5223X_RESET;


typedef struct stM5223X_CMR
{
#if defined _M523X                                                       // {7}
unsigned long  MCF_CLOCK_SYNCR;
unsigned long  MCF_CLOCK_SYNSR;
#else
unsigned short MCF_CLOCK_SYNCR;
unsigned char  MCF_CLOCK_SYNSR;
#if !defined _M521X                                                      // {4}
    #if defined _M5221X || defined _M5222X || defined _M5225X || defined _M521XX
 unsigned char  ucPad0[1];
 unsigned short MCF_CLOCK_ROCR;
    #else
 unsigned char  ucPad0[3];
    #endif
 unsigned char  ucPad1;
 unsigned char  LPDR;
 unsigned char  MCF_CLOCK_CCHR;
    #if defined _M5221X || defined _M5222X || defined _M5225X || defined _M521XX
 unsigned char  MCF_CLOCK_CCLR;
 unsigned char  MCF_CLOCK_OCHR;
 unsigned char  MCF_CLOCK_OCLR;
    #else
 unsigned char  ucPad2[3];
    #endif
    #if defined _M5221X || defined _M5225X || defined _M521XX
 unsigned char  ucPad3[6];
 unsigned char MCF_CLOCK_RTCCR;
 unsigned char MCF_CLOCK_BWCR;
    #else
 unsigned long  MCF_CLOCK_RTCDR;
    #endif
#endif
#endif
} M5223X_CMR;

typedef struct stM5223X_EPORT
{
unsigned short  EPPAR;
unsigned char   EPDDR;
unsigned char   EPIER;
unsigned char   EPDR;
unsigned char   EPPDR;
unsigned char   EPFR;
} M5223X_EPORT;

#if defined _M5222X || defined _M5221X || defined _M5225X || defined _M521XX || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {5}{6}{7}
typedef struct stM5223X_BWTR
{
unsigned short  WCR;
unsigned short  WMR;
unsigned short  WCNTR;
unsigned short  WSR;
} M5223X_BWTR;
#endif



typedef struct stM5223X_PIT
{
unsigned short PCSR;
unsigned short PMR;
unsigned short PCNTR;
} M5223X_PIT;


#ifdef _M52XX_SDRAM                                                      // {5}
typedef struct stM5223X_QADC
{
unsigned short QADCMCR;
unsigned short QADCTEST;
unsigned short usRes0;
unsigned char  PORTQA;
unsigned char  PORTQB;
unsigned char  DDRQA;
unsigned char  DDRQB;
unsigned short QACR0;
unsigned short QACR1;
unsigned short QACR2;
unsigned short QASR0;
unsigned short QASR1;
unsigned char  ucRes1[492];
unsigned short QADC_CCW[64];
unsigned short QADC_RJURR[64];
unsigned short QADC_LJSRR[64];
unsigned short QADC_LJURR[64];
} M5223X_QADC;
#else
typedef struct stM5223X_ADC
{
unsigned short ADC_CTRL1;
unsigned short ADC_CTRL2;
unsigned short ADC_ADZCC;
unsigned short ADC_ADLST1;
unsigned short ADC_ADLST2;
unsigned short ADC_ADSDIS;
unsigned short ADC_ADSTAT;
unsigned short ADC_ADLSTAT;
unsigned short ADC_ADZCSTAT;
unsigned short ADC_ADRSLT0;
unsigned short ADC_ADRSLT1;
unsigned short ADC_ADRSLT2;
unsigned short ADC_ADRSLT3;
unsigned short ADC_ADRSLT4;
unsigned short ADC_ADRSLT5;
unsigned short ADC_ADRSLT6;
unsigned short ADC_ADRSLT7;
unsigned short ADC_ADLLR0;
unsigned short ADC_ADLLR1;
unsigned short ADC_ADLLR2;
unsigned short ADC_ADLLR3;
unsigned short ADC_ADLLR4;
unsigned short ADC_ADLLR5;
unsigned short ADC_ADLLR6;
unsigned short ADC_ADLLR7;
unsigned short ADC_ADHLR0;
unsigned short ADC_ADHLR1;
unsigned short ADC_ADHLR2;
unsigned short ADC_ADHLR3;
unsigned short ADC_ADHLR4;
unsigned short ADC_ADHLR5;
unsigned short ADC_ADHLR6;
unsigned short ADC_ADHLR7;
unsigned short ADC_ADOFS0;
unsigned short ADC_ADOFS1;
unsigned short ADC_ADOFS2;
unsigned short ADC_ADOFS3;
unsigned short ADC_ADOFS4;
unsigned short ADC_ADOFS5;
unsigned short ADC_ADOFS6;
unsigned short ADC_ADOFS7;
unsigned short ADC_POWER;
unsigned short ADC_CAL;
} M5223X_ADC;
#endif

typedef struct stM5223X_GPT
{
unsigned char  GPTIOS;
unsigned char  GPTCFORC;
unsigned char  GTPOC3M;
unsigned char  GPTOC3D;
volatile unsigned short GPTCNT;
unsigned char  GPTSCR1;
unsigned char  ucRes1;
unsigned char  GPTTOV;
unsigned char  GPTCTL1;
unsigned char  ucRes2;
unsigned char  GPTCTL2;
unsigned char  GPTIE;
unsigned char  GPTSCR2;
volatile unsigned char  GPTFLG1;
volatile unsigned char  GPTFLG2;
volatile unsigned short GPTC0;
volatile unsigned short GPTC1;
volatile unsigned short GPTC2;
volatile unsigned short GPTC3;
unsigned char  GPTPACTL;
volatile unsigned char  GPTPAFLG;
volatile unsigned short GPTPACNT;
unsigned char  ucRes3;
volatile unsigned char  GPTPORT;
unsigned char  GPTDDR;
unsigned char  GPTTST;
} M5223X_GPT;

typedef struct stM5223X_PWM                                              // {3}
{
unsigned char PWME;
unsigned char PWMPOL;
unsigned char PWMCLK;
unsigned char PWMPRCLK;
unsigned char PWMCAE;
unsigned char PWMCTL; 
unsigned char ucRes0[2];
unsigned char PWMSCLA;
unsigned char PWMSCLB;
unsigned char ucRes1[2];
unsigned char PWMCNT0;
unsigned char PWMCNT1;
unsigned char PWMCNT2;
unsigned char PWMCNT3;
unsigned char PWMCNT4;
unsigned char PWMCNT5;
unsigned char PWMCNT6;
unsigned char PWMCNT7;
unsigned char PWMPER0;
unsigned char PWMPER1;
unsigned char PWMPER2;
unsigned char PWMPER3;
unsigned char PWMPER4;
unsigned char PWMPER5;
unsigned char PWMPER6;
unsigned char PWMPER7;
unsigned char PWMDTY0;
unsigned char PWMDTY1;
unsigned char PWMDTY2;
unsigned char PWMDTY3;
unsigned char PWMDTY4;
unsigned char PWMDTY5;
unsigned char PWMDTY6;
unsigned char PWMDTY7;
unsigned char PWMSDN;
#ifdef _M5225X
    unsigned char ucRes2;
    unsigned char PCME;
#endif
} M5223X_PWM;


typedef struct stM5222X_USB
{
unsigned char PER_ID;
unsigned char ucRes1[3];
unsigned char ID_COMP;
unsigned char ucRes2[3];
unsigned char REV;
unsigned char ucRes3[3];
unsigned char ADD_INFO;
unsigned char ucRes4[3];
unsigned char OTG_INT_STAT;
unsigned char ucRes5[3];
unsigned char OTG_INT_EN;
unsigned char ucRes6[3];
unsigned char OTG_STATUS;
unsigned char ucRes7[3];
unsigned char OTG_CTRL;
unsigned char ucRes8[0x63];
unsigned char INT_STAT;
unsigned char ucRes9[3];
unsigned char INT_ENB;
unsigned char ucRes10[3];
unsigned char ERR_STAT;
unsigned char ucRes11[3];
unsigned char ERR_ENG;
unsigned char ucRes12[3];
unsigned char STAT;
unsigned char ucRes13[3];
unsigned char CTL;
unsigned char ucRes14[3];
unsigned char ADDR;
unsigned char ucRes15[3];
unsigned char BDT_PAGE_01;
unsigned char ucRes16[3];
unsigned char FRM_NUML;
unsigned char ucRes17[3];
unsigned char FRM_NUMH;
unsigned char ucRes18[3];
unsigned char TOKEN;
unsigned char ucRes19[3];
unsigned char SOF_THLD;
unsigned char ucRes20[3];
unsigned char BDT_PAGE_02;
unsigned char ucRes21[3];
unsigned char BDT_PAGE_03;
unsigned char ucRes22[11];
unsigned char ENDPT0;
unsigned char ucRes23[3];
unsigned char ENDPT1;
unsigned char ucRes24[3];
unsigned char ENDPT2;
unsigned char ucRes25[3];
unsigned char ENDPT3;
unsigned char ucRes26[3];
unsigned char ENDPT4;
unsigned char ucRes27[3];
unsigned char ENDPT5;
unsigned char ucRes28[3];
unsigned char ENDPT6;
unsigned char ucRes29[3];
unsigned char ENDPT7;
unsigned char ucRes30[3];
unsigned char ENDPT8;
unsigned char ucRes31[3];
unsigned char ENDPT9;
unsigned char ucRes32[3];
unsigned char ENDPT10;
unsigned char ucRes33[3];
unsigned char ENDPT11;
unsigned char ucRes34[3];
unsigned char ENDPT12;
unsigned char ucRes35[3];
unsigned char ENDPT13;
unsigned char ucRes36[3];
unsigned char ENDPT14;
unsigned char ucRes37[3];
unsigned char ENDPT15;
unsigned char ucRes38[3];
unsigned char USB_CTRL;
unsigned char ucRes39[3];
unsigned char USB_OTG_OBSERVE;
unsigned char ucRes40[3];
unsigned char USB_OTG_CTRL;
} M5222X_USB;


typedef struct stM5223X_CAN
{
unsigned long  CANMCR;
unsigned long  CANCTRL;
unsigned long  CAN_TIMER;
unsigned long  ulRes2;
unsigned long  RXGMASK;
unsigned long  RX14MASK;
unsigned long  RX15MASK;
unsigned long  CAN_ERRCNT;
volatile unsigned long  CAN_ERRSTAT;
unsigned long  ulRes3;
unsigned long  CAN_IMASK;
unsigned long  ulRes4;
unsigned long  CAN_IFLAG;
unsigned long  ulRes5[19];
COLDFIRE_CAN_BUF  MBUFF0;
COLDFIRE_CAN_BUF  MBUFF1;
COLDFIRE_CAN_BUF  MBUFF2;
COLDFIRE_CAN_BUF  MBUFF3;
COLDFIRE_CAN_BUF  MBUFF4;
COLDFIRE_CAN_BUF  MBUFF5;
COLDFIRE_CAN_BUF  MBUFF6;
COLDFIRE_CAN_BUF  MBUFF7;
COLDFIRE_CAN_BUF  MBUFF8;
COLDFIRE_CAN_BUF  MBUFF9;
COLDFIRE_CAN_BUF  MBUFF10;
COLDFIRE_CAN_BUF  MBUFF11;
COLDFIRE_CAN_BUF  MBUFF12;
COLDFIRE_CAN_BUF  MBUFF13;
COLDFIRE_CAN_BUF  MBUFF14;
COLDFIRE_CAN_BUF  MBUFF15;
} M5223X_CAN;


typedef struct stM5223X_CFM
{
unsigned short CFMMCR;
unsigned char  CFMCLKD;
unsigned char  ucRes1[5];
unsigned long  CFMSEC;
unsigned long  CFMPROT;
unsigned long  CFMSACC;
unsigned long  CFMDACC;
unsigned long  ulRes1;
unsigned char  CFMUSTAT;
unsigned char  ucRes2[3];
unsigned char  CFMCMD;
unsigned char  ucRes3[37];
unsigned short CFMCLKSEL;
} M5223X_CFM;


typedef struct stM5223X_EPHY
{
unsigned char EPHYCTL0;
unsigned char EPHYCTL1;
unsigned char EPHYSR;
} M5223X_EPHY;


typedef struct stM5223X_RNG
{
volatile unsigned long RNGCR;
volatile unsigned long RNGSR;
unsigned long RNGER;
volatile unsigned long RNGOUT;
} M5223X_RNG;


// FLASH controller
//
typedef struct stM5223X_CFC
{
unsigned char BACKDOOR_COMP_KEY;
unsigned char BACKDOOR_COMP_KEY1;
unsigned char BACKDOOR_COMP_KEY2;
unsigned char BACKDOOR_COMP_KEY3;
unsigned char BACKDOOR_COMP_KEY4;
unsigned char BACKDOOR_COMP_KEY5;
unsigned char BACKDOOR_COMP_KEY6;
unsigned char BACKDOOR_COMP_KEY7;
unsigned char BACKDOOR_COMP_KEY8;

unsigned char FLASH_PROTECTION;
unsigned char FLASH_PROTECTION1;
unsigned char FLASH_PROTECTION2;
unsigned char FLASH_PROTECTION3;

unsigned char FLASH_SUPV;
unsigned char FLASH_SUPV1;
unsigned char FLASH_SUPV2;
unsigned char FLASH_SUPV3;

unsigned char FLASH_DATA;
unsigned char FLASH_DATA1;
unsigned char FLASH_DATA2;
unsigned char FLASH_DATA3;

unsigned char FLASH_SECURITY;
unsigned char FLASH_SECURITY1;
unsigned char FLASH_SECURITY2;
unsigned char FLASH_SECURITY3;
} M5223X_CFC;

#if defined _M520X                                                       // {6}
    typedef struct stM5223X_SDRAMC
    {
    unsigned long SDMR;
    unsigned long SDCR;
    unsigned long SDCFG1;
    unsigned long SDCFG2;
    unsigned char ucRes1[0x100];
    unsigned long SDCS0;
    unsigned long SDCS1;
    } M5223X_SDRAMC;
#elif defined _M523X
    typedef struct stM5223X_SDRAMC
    {
    unsigned long DCR;
    unsigned long ulRes0;
    unsigned long DACR0;
    unsigned long DMR0;
    } M5223X_SDRAMC;
#endif

typedef struct stM5223X_EMAC
{
unsigned long ulRes;
unsigned long EIR;
unsigned long EIMR;
unsigned long ulRes0;
unsigned long RDAR;
unsigned long TDAR;
unsigned long ulRes1[3];
unsigned long ECR;
unsigned long ulRes2[6];
unsigned long MDATA;
unsigned long MSCR;
unsigned long ulRes3[7];
unsigned long MIBC;
unsigned long ulRes4[7];
unsigned long RCR;
unsigned long ulRes5[15];
unsigned long TCR;
unsigned long ulRes6[7];
unsigned long PALR;
unsigned long PAUR;
unsigned long OPD;
unsigned long ulRes7[10];
unsigned long IAUR;
unsigned long IALR;
unsigned long GAUR;
unsigned long GALR;
unsigned long ulRes8[7];
unsigned long TFWR;
unsigned long ulRes9;
unsigned long FRBR;
unsigned long FRSR;
unsigned long ulRes10[11];
unsigned long ERDSR;
unsigned long ETDSR;
unsigned long EMRBR;
unsigned long ulRes11[29];
unsigned long ulMIB[512/sizeof(unsigned long)];                          // counters
} M5223X_EMAC;

typedef struct stM5223X_EMAC_FIFO
{
unsigned char EPHYCTL0;
unsigned char EPHYCTL1;
unsigned char EPHYSR;
} M5223X_EMAC_FIFO;








// A copy of the M5223X peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)

typedef struct stM52235_PERIPH
{
    M5223X_SCM           SimSCM;
#if defined _M5225X || defined _M520X || defined _M523X                  // {6}
    M5225X_FLEXBUS       SimFlexBus;                                     // {2}
#endif
    M5223X_DMA           SimDMAS[4];
    M5223X_UART          SimUARTS[3];
    M5223X_I2C           SimI2C;
#if defined  _M5221X || defined _M5225X || defined _M521XX               // {2}
    M5223X_I2C           SimI2C_1;
#endif
    M5223X_QSPI          SimQSPI;
    M5223X_RTC           SimRTC;
    M5223X_DMA_TIMER     DMATimer0;
    M5223X_DMA_TIMER     DMATimer1;
    M5223X_DMA_TIMER     DMATimer2;
    M5223X_DMA_TIMER     DMATimer3;
    M5223X_INT_CTRL      SimINT_CTRL0;
#if !defined _M5222X
    M5223X_INT_CTRL      SimINT_CTRL1;
#endif
#if !defined _M5222X && !defined _M5221X && !defined _M521XX
    M5223X_EMAC          SimEMAC;
    M5223X_EMAC_FIFO     SimEMACFifo;
#endif
    M5223X_PORT          SimPORT;
    M5223X_RESET         SimRESET;
    M5223X_CMR           SimCMR;
    M5223X_EPORT         SimEPORT;
#if defined _M5222X || defined _M5221X || defined _M5225X || defined _M521XX || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {2}{5}{6}{7}
    M5223X_BWTR          SimBWTR;
#else
    M5223X_EPORT         SimEPORT1;
#endif
    M5223X_PIT           SimPIT0;
    M5223X_PIT           SimPIT1;
    M5223X_PIT           SimPIT2;
    M5223X_PIT           SimPIT3;
#ifdef _M52XX_SDRAM                                                      // {5}
    M5223X_QADC          SimADC;
#else
    M5223X_ADC           SimADC;
#endif
    M5223X_GPT           SimGTP;                                         // {3}
#ifdef _M52XX_SDRAM                                                      // {5}
    M5223X_GPT           SimGTP1;
#else
    M5223X_PWM           SimPWM;
#endif
#if defined _M5222X || defined _M5221X || defined _M5225X                // {2}
    M5222X_USB           SimUSB;
#endif
#if !defined _M5222X && !defined _M5221X && !defined _M521XX
    M5223X_CAN           SimCAN;
#endif
    M5223X_CFM           SimCFM;
#ifndef _M5222X
    #ifndef _M5225X                                                      // {2}
    M5223X_EPHY          SimEPHY;
    #endif
    M5223X_RNG           SimRNG;
#endif
    M5223X_CFC           SimCFC;
#if defined _M520X || defined _M523X                                     // {6}
    M5223X_SDRAMC        SimSDRAMC;
#endif
} M52235_PERIPH;


extern M52235_PERIPH ucM5223X;

#if defined _M5225X                                                      // {2}
    #define _PORT_QS            0                                        // port referencing in simulator
    #define _PORT_AN            1
    #define _PORT_NQ            2
    #define _PORT_TA            3
    #define _PORT_TC            4
    #define _PORT_UA            5
    #define _PORT_UB            6
    #define _PORT_UC            7
    #define _PORT_AS            8
    #define _PORT_DD            9
    #define _PORT_TE            10
    #define _PORT_TF            11
    #define _PORT_TG            12
    #define _PORT_TH            13
    #define _PORT_TI            14
    #define _PORT_TJ            15
#elif defined _M52XX_SDRAM                                               // {5}
    #define _PORT_A             0                                        // port referencing in simulator
    #define _PORT_B             1
    #define _PORT_C             2
    #define _PORT_D             3
    #define _PORT_E             4
    #define _PORT_F             5
    #define _PORT_G             6
    #define _PORT_H             7
    #define _PORT_J             8
    #define _PORT_DD            9
    #define _PORT_EH            10
    #define _PORT_EL            11
    #define _PORT_AS            12
    #define _PORT_NQ            13
    #define _PORT_QA            14
    #define _PORT_QB            15
    #define _PORT_QS            16
    #define _PORT_SD            17
    #define _PORT_TA            18
    #define _PORT_TB            19
    #define _PORT_TC            20
    #define _PORT_TD            21
    #define _PORT_UA            22
#elif defined _M520X                                                     // {6}
    #define _PORT_FECH          0                                        // port referencing in simulator
    #define _PORT_FECL          1
    #define _PORT_BUSCTL        2
    #define _PORT_BE            3
    #define _PORT_CS            4
    #define _PORT_FECI2C        5
    #define _PORT_UART          6
    #define _PORT_QSPI          7
    #define _PORT_TIMER         8
    #define _PORT_IRQ           9
#elif defined _M523X                                                     // {7}
    #define _PORT_ADDR          0                                        // port referencing in simulator
    #define _PORT_BS            1
    #define _PORT_BUSCTL        2
    #define _PORT_CS            3
    #define _PORT_DATAH         4
    #define _PORT_DATAL         5
    #define _PORT_FECI2C        6
    #define _PORT_ETPU          7
    #define _PORT_IRQ           8
    #define _PORT_QSPI          9
    #define _PORT_SDRAM         10
    #define _PORT_TIMER         11
    #define _PORT_UARTH         12
    #define _PORT_UARTL         13
#else
    #define _PORT_QS            0                                        // port referencing in simulator
    #define _PORT_AN            1
    #define _PORT_LD            2
    #define _PORT_NQ            3
    #define _PORT_TA            4
    #define _PORT_TC            5
    #define _PORT_TD            6
    #define _PORT_UA            7
    #define _PORT_UB            8
    #define _PORT_UC            9
    #define _PORT_AS            10
    #define _PORT_GP            11
    #define _PORT_DD            12
#endif


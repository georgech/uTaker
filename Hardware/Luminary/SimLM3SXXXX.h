/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:   	    SimLM3SXXXX.h
   Project: 	Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   *********************************************************************
   16.12.2008 Add USB support                                            {1}
   05.02.2009 Add General Purpose Timer support                          {2}
   31.03.2009 Add ADC support                                            {3}
   05.12.2009 Add Tempest class                                          {4}
   27.12.2010 Add PWM module                                             {5}
   19.06.2018 Change _GPIO_x to _PORTx for global compatibiity           {6}
   20.06.2018 And external ports                                         {7}

*/  


typedef struct stLM3SXXXX_WATCHDOG
{
unsigned long WDTLOAD;
unsigned long WDTVALUE;
unsigned long WDTCTL;
unsigned long WD_RESEN;
unsigned long WD_INTEN;
unsigned long WDTICR;
unsigned long WDTRIS;
unsigned long WTDRIS;
unsigned long WDTMIS;
unsigned long WTDMIS;
unsigned long WDTTEST;
unsigned long WD_STALL;
unsigned long WDTLOCK;
unsigned long WD_LOCKED;
unsigned long WD_UNLOCKED;
unsigned long WD_ACCESS;
unsigned long WDTPeriphID4;
unsigned long WDTPeriphID5;
unsigned long WDTPeriphID6;
unsigned long WDTPeriphID7;
unsigned long WDTPeriphID0;
unsigned long WDTPeriphID1;
unsigned long WDTPeriphID2;
unsigned long WDTPeriphID3;
unsigned long WDTPCellID0;
unsigned long WDTPCellID1;
unsigned long WDTPCellID2;
unsigned long WDTPCellID3;
} LM3SXXXX_WATCHDOG;

typedef struct stLM3SXXXX_GPT                                            // {2}
{
unsigned long GPTMCFG;        
unsigned long GPTMTAMR;       
unsigned long GPTMTBMR;       
unsigned long GPTMCTL;        
unsigned long ulRes1[2];
unsigned long GPTMIMR;        
volatile unsigned long GPTMRIS;        
volatile unsigned long GPTMMIS;        
volatile unsigned long _GPTMICR;        
unsigned long GPTMTAILR;      
unsigned long GPTMTBILR;      
unsigned long GPTMTAMATCHR;   
unsigned long GPTMTBMATCHR;   
unsigned long GPTMTAPR;       
unsigned long GPTMTBPR;       
unsigned long GPTMTAPMR;      
unsigned long GPTMTBPMR;      
volatile unsigned long GPTMTAR;        
volatile unsigned long GPTMTBR;        
unsigned long ulRes2[1024 - 20];                                         // pack to 4k boundary
} LM3SXXXX_GPT;


typedef struct stLM3SXXXX_ADC                                            // {3}
{
unsigned long ADCACTSS;
volatile unsigned long ADCRIS;
unsigned long ADCIM;
volatile unsigned long ADCISC;
volatile unsigned long ADCOSTAT;
unsigned long ADCEMUX;
volatile unsigned long ADCUSTAT;
unsigned long ulRes1;
unsigned long ADCSSPRI;
unsigned long ulRes2;
volatile unsigned long ADCPSSI;
unsigned long ulRes3;
unsigned long ADCSAC;
volatile unsigned long ADCDCISC;
unsigned long ADCCTL;
unsigned long ulRes4;
unsigned long ADCSSMUX0;
unsigned long ADCSSCLT0;
volatile unsigned long ADCSSFIFO0;
volatile unsigned long ADCSSFSTAT0;
unsigned long ADCSSOP0;
unsigned long ADCSSDC0;
unsigned long ulRes5[2];
unsigned long ADCSSMUX1;
unsigned long ADCSSCLT1;
volatile unsigned long ADCSSFIFO1;
volatile unsigned long ADCSSFSTAT1;
unsigned long ADCSSOP1;
unsigned long ADCSSDC1;
unsigned long ulRes6[2];
unsigned long ADCSSMUX2;
unsigned long ADCSSCLT2;
volatile unsigned long ADCSSFIFO2;
volatile unsigned long ADCSSFSTAT2;
unsigned long ADCSSOP2;
unsigned long ADCSSDC2;
unsigned long ulRes7[2];
unsigned long ADCSSMUX3;
unsigned long ADCSSCLT3;
volatile unsigned long ADCSSFIFO3;
volatile unsigned long ADCSSFSTAT3;
unsigned long ADCSSOP3;
unsigned long ADCSSDC3;
unsigned long ulRes8[786];
unsigned long ADCDCRIC;
unsigned long ulRes9[63];
unsigned long ADCDCCTL0;
unsigned long ADCDCCTL1;
unsigned long ADCDCCTL2;
unsigned long ADCDCCTL3;
unsigned long ADCDCCTL4;
unsigned long ADCDCCTL5;
unsigned long ADCDCCTL6;
unsigned long ADCDCCTL7;
unsigned long ulRes10[8];
unsigned long ADCDCCMP0;
unsigned long ADCDCCMP1;
unsigned long ADCDCCMP2;
unsigned long ADCDCCMP3;
unsigned long ADCDCCMP4;
unsigned long ADCDCCMP5;
unsigned long ADCDCCMP6;
unsigned long ADCDCCMP7;
} LM3SXXXX_ADC;



typedef struct stLM3SXXXX_PIO
{
unsigned long data_mask[255];
unsigned long GPIODATA;
unsigned long GPIODIR;
unsigned long GPIOIS;
unsigned long GPIOIBE;
unsigned long GPIOIEV;
unsigned long GPIOIM;
unsigned long GPIORIS;
unsigned long GPIOMIS;
unsigned long GPIOICR;
unsigned long GPIOAFSEL;
unsigned char ucRes1[220];
unsigned long GPIODR2R;
unsigned long GPIODR4R;
unsigned long GPIODR8R;
unsigned long GPIOODR;
unsigned long GPIOPUR;
unsigned long GPIOPDR;
unsigned long GPIOSLR;
unsigned long GPIODEN;
unsigned long GPIOLOCK;
unsigned long GPIOCR;
unsigned long GPIOAMSEL;
unsigned long GPIOPCTL;
unsigned char ucRes2[2720];
unsigned long GPIOPeriphID4;
unsigned long GPIOPeriphID5;
unsigned long GPIOPeriphID6;
unsigned long GPIOPeriphID7;
unsigned long GPIOPeriphID0;
unsigned long GPIOPeriphID1;
unsigned long GPIOPeriphID2;
unsigned long GPIOPeriphID3;
unsigned long GPIOPcellID0;
unsigned long GPIOPcellID1;
unsigned long GPIOPcellID2;
unsigned long GPIOPcellID3;
} LM3SXXXX_PIO;



typedef struct stLM3SXXXX_PWM
{
unsigned long PWMCTL;
unsigned long PWMSYNC;
unsigned long PWMENABLE;
unsigned long PWMINVERT;
unsigned long PWMFAULT;
unsigned long PWMINTEN;
unsigned long PWMRIS;
unsigned long PWMISC;
unsigned long PWMSTATUS;
unsigned long PWMFAULTVAL;
unsigned long ucRes0[6];
unsigned long PWM0CTL;
unsigned long PWM0INTEN;
unsigned long PWM0RIS;
unsigned long PWM0ISC;
unsigned long PWM0LOAD;
unsigned long PWM0COUNT;
unsigned long PWM0CMPA;
unsigned long PWM0CMPB;
unsigned long PWM0GENA;
unsigned long PWM0GENB;
unsigned long PWM0DBCTL;
unsigned long PWM0DBRISE;
unsigned long PWM0DBRALL;
unsigned long PWM0FLTSRC0;
unsigned long ucRes1;
unsigned long PWM0MINFLTPER;
unsigned long PWM1CTL;
unsigned long PWM1INTEN;
unsigned long PWM1RIS;
unsigned long PWM1ISC;
unsigned long PWM1LOAD;
unsigned long PWM1COUNT;
unsigned long PWM1CMPA;
unsigned long PWM1CMPB;
unsigned long PWM1GENA;
unsigned long PWM1GENB;
unsigned long PWM1DBCTL;
unsigned long PWM1DBRISE;
unsigned long PWM1DBRALL;
unsigned long PWM1FLTSRC0;
unsigned long ucRes2;
unsigned long PWM1MINFLTPER;
unsigned long PWM2CTL;
unsigned long PWM2INTEN;
unsigned long PWM2RIS;
unsigned long PWM2ISC;
unsigned long PWM2LOAD;
unsigned long PWM2COUNT;
unsigned long PWM2CMPA;
unsigned long PWM2CMPB;
unsigned long PWM2GENA;
unsigned long PWM2GENB;
unsigned long PWM2DBCTL;
unsigned long PWM2DBRISE;
unsigned long PWM2DBRALL;
unsigned long PWM2FLTSRC0;
unsigned long ucRes3;
unsigned long PWM2MINFLTPER;
unsigned long PWM3CTL;
unsigned long PWM3INTEN;
unsigned long PWM3RIS;
unsigned long PWM3ISC;
unsigned long PWM3LOAD;
unsigned long PWM3COUNT;
unsigned long PWM3CMPA;
unsigned long PWM3CMPB;
unsigned long PWM3GENA;
unsigned long PWM3GENB;
unsigned long PWM3DBCTL;
unsigned long PWM3DBRISE;
unsigned long PWM3DBRALL;
unsigned long PWM3FLTSRC0;
unsigned long ucRes4;
unsigned long PWM3MINFLTPER;
unsigned long ucRes5[432];
unsigned long PWM0FLTSEN;
unsigned long PWM0FLTSTAT0;
unsigned long ucRes6[30];
unsigned long PWM1FLTSEN;
unsigned long PWM1FLTSTAT0;
unsigned long ucRes7[30];
unsigned long PWM2FLTSEN;
unsigned long PWM2FLTSTAT0;
unsigned long ucRes8[31];
unsigned long PWM3FLTSTAT0;
} LM3SXXXX_PWM;

typedef struct stLM3SXXXX_CAN
{
unsigned long CANCTL;    
unsigned long CANSTS;    
unsigned long CANERR;    
unsigned long CANBIT;    
unsigned long CANINT;    
unsigned long CANTST;    
unsigned long CANBRPE;   
unsigned long ulRes1;
unsigned long CANIF1CRQ; 
unsigned long CANIF1CMSK;
unsigned long CANIF1MSK1;
unsigned long CANIF1MSK2;
unsigned long CANIF1ARB1;
unsigned long CANIF1ARB2;
unsigned long CANIF1MCTL;
unsigned long CANIF1DA1;
unsigned long CANIF1DA2;
unsigned long CANIF1DB1;
unsigned long CANIF1DB2;
unsigned long ulRes2[13];
unsigned long CANIF2CRQ;
unsigned long CANIF2CMSK;
unsigned long CANIF2MSK1;
unsigned long CANIF2MSK2;
unsigned long CANIF2ARB1;
unsigned long CANIF2ARB2;
unsigned long CANIF2MCTL;
unsigned long CANIF2DA1;
unsigned long CANIF2DA2;
unsigned long CANIF2DB1;
unsigned long CANIF2DB2;
unsigned long ulRes3[21];
unsigned long CANTXRQ1;
unsigned long CANTXRQ2;
unsigned long ulRes4[6]; 
unsigned long CANTXNWDA1;
unsigned long CANTXNWDA2;
unsigned long ulRes5[6];
unsigned long CANMSG1INT;
unsigned long CANMSG2INT;
unsigned long ulRes6[6];
unsigned long CANMSG1VAL;
unsigned long CANMSG2VAL;
} LM3SXXXX_CAN;

typedef struct stLM3SXXXX_ETH
{
unsigned long MACRIS_IACK;
unsigned long MACIM;
unsigned long MACRCTL;
unsigned long MACTCTL;
unsigned long MACDATA;
unsigned long MACIA0;
unsigned long MACIA1;
unsigned long MACTHR;
unsigned long MACMCTL;
unsigned long MACMDV;
unsigned long ulRes1;
unsigned long MACMTXD;
unsigned long MACMRXD;
const unsigned long MACNP;
unsigned long MACTR;
#if defined _LM3S8962
    unsigned long MACTS;
#endif
#if defined _TEMPEST_CLASS                                               // {4}
    unsigned long MACLED;
    unsigned long MDIX;
#endif
} LM3SXXXX_ETH;

typedef struct stLM3SXXXX_USB                                            // {1}
{
unsigned char USBFADDR;
unsigned char USBPOWER;
unsigned short USBTXIS;
unsigned short USBRXIS;
unsigned short USBTXIE;
unsigned short USBRXIE;
unsigned char USBIS;
unsigned char USBIE;
unsigned short USBFRAME;
unsigned char USBEPIDX;
unsigned char USBTEST;
unsigned char ucRes0[16];
unsigned long USBFIFO0;
unsigned long USBFIFO1;
unsigned long USBFIFO2;
unsigned long USBFIFO3;
unsigned char ucRes1[48];
unsigned char USBDEVCTL;
unsigned char ucRes2;
unsigned char USBTXFIFOSZ;
unsigned char USBRXFIFOSZ;
unsigned short USBTXFIFOADD;
unsigned short USBRXFIFOADD;
unsigned char ucRes3[18];
unsigned char USBCONTIM;   
#ifdef _LM3S3768
    unsigned char USBVPLEN;
#else
    unsigned char ucRes3a;
#endif
unsigned char ucRes4;
unsigned char USBFSEOF;
unsigned char USBLSEOF;
unsigned char ucRes5;
unsigned char USBTXFUNCADDR0;
unsigned char ucRes6;
unsigned char USBTXHUBADDR0;
unsigned char USBTXHUBPORT0;
unsigned char ucRes7[4];
unsigned char USBTXFUNCADDR1;
unsigned char ucRes8;
unsigned char USBTXHUBADDR1;
unsigned char USBTXHUBPORT1;
unsigned char USBRXFUNCADDR1;
unsigned char ucRes9;
unsigned char USBRXHUBADDR1;
unsigned char USBRXHUBPORT1;
unsigned char USBTXFUNCADDR2;
unsigned char ucRes10;
unsigned char USBTXHUBADDR2;
unsigned char USBTXHUBPORT2;
unsigned char USBRXFUNCADDR2;
unsigned char ucRes11;
unsigned char USBRXHUBADDR2;
unsigned char USBRXHUBPORT2;
unsigned char USBTXFUNCADDR3;
unsigned char ucRes12;
unsigned char USBTXHUBADDR3;
unsigned char USBTXHUBPORT3;
unsigned char USBRXFUNCADDR3;
unsigned char ucRes13;
unsigned char USBRXHUBADDR3;
unsigned char USBRXHUBPORT3;
unsigned char ucRes14[98];
unsigned char USBCSRL0;
unsigned char USBCSRH0;
unsigned char ucRes15[4];
unsigned char USBCOUNT0;
unsigned char ucRes16;
unsigned char USBTYPE0;
unsigned char USBNAKLMT;
unsigned char ucRes17[4];
unsigned short USBTXMAXP1;
unsigned char USBTXCSRL1;
unsigned char USBTXCSRH1;
unsigned short USBRXMAXP1;
unsigned char USBRXCSRL1;
unsigned char USBRXCSRH1;
unsigned short USBRXCOUNT1;
unsigned char USBTXTYPE1;
unsigned char USBTXINTERVAL1;
unsigned char USBRCTYPE1;
unsigned char USBRXINTERVAL1;
unsigned char ucRes18[2];
unsigned short USBTXMAXP2;
unsigned char USBTXCSRL2;
unsigned char USBTXCSRH2;
unsigned short USBRXMAXP2;
unsigned char USBRXCSRL2;
unsigned char USBRXCSRH2;
unsigned short USBRXCOUNT2;
unsigned char USBTXTYPE2;
unsigned char USBTXINTERVAL2;
unsigned char USBRCTYPE2;
unsigned char USBRXINTERVAL2;
unsigned char ucRes19[2];
unsigned short USBTXMAXP3;
unsigned char USBTXCSRL3;
unsigned char USBTXCSRH3;
unsigned short USBRXMAXP3;
unsigned char USBRXCSRL3;
unsigned char USBRXCSRH3;
unsigned short USBRXCOUNT3;
unsigned char USBTXTYPE3;
unsigned char USBTXINTERVAL3;
unsigned char USBRCTYPE3;
unsigned char USBRXINTERVAL3;
unsigned char ucRes20[454];
unsigned short USBRQPKTCOUNT1;
unsigned char ucRes21[2];
unsigned short USBRQPKTCOUNT2; 
unsigned char ucRes22[2];
unsigned short USBRQPKTCOUNT3; 
unsigned char ucRes23[50];
unsigned short USBRXDPKTBUFDIS;
unsigned short USBTXDPKTBUFDIS;
unsigned char ucRes24[188];
unsigned long USBEPC;
unsigned long USBEPCRIS;
unsigned long USBEPCIM;
unsigned long USBEPCISC;
unsigned long USBDRRIS;
unsigned long USBDRIM;
unsigned long USBDRISC;
#if !defined _LM3S3768
    unsigned long USBGPCS;
#endif
} LM3SXXXX_USB;

typedef struct stLM3SXXXX_SSI
{
unsigned long SSICR0;      
unsigned long SSICR1;      
unsigned long SSIDR;       
unsigned long SSISR;       
unsigned long SSICPSR;     
unsigned long SSIIM;       
unsigned long SSIRIS;      
unsigned long SSIMIS;      
unsigned long SSIICR;
unsigned long ulRes[1004];
unsigned long SSIPeriphID4;
unsigned long SSIPeriphID5;
unsigned long SSIPeriphID6;
unsigned long SSIPeriphID7;
unsigned long SSIPeriphID0;
unsigned long SSIPeriphID1;
unsigned long SSIPeriphID2;
unsigned long SSIPeriphID3;
unsigned long SSIPCellID0; 
unsigned long SSIPCellID1; 
unsigned long SSIPCellID2; 
unsigned long SSIPCellID3; 
} LM3SXXXX_SSI;

typedef struct stLM3SXXXX_UART
{
unsigned long UARTDR;       
unsigned long UARTRSR_ECR;  
unsigned long ulRes1[4];
unsigned long UARTFR;  
unsigned long ulRes2;     
unsigned long UARTILPR;     
unsigned long UARTIBRD;     
unsigned long UARTFBRD;     
unsigned long UARTLCRH;     
unsigned long UARTCTL;      
unsigned long UARTIFLS;     
unsigned long UARTIM;       
unsigned long UARTRIS;      
unsigned long UARTMIS;      
unsigned long UARTICR;
unsigned long UARTDMACTL;
unsigned long ulRes3[993];
unsigned long UARTPeriphID4;
unsigned long UARTPeriphID5;
unsigned long UARTPeriphID6;
unsigned long UARTPeriphID7;
unsigned long UARTPeriphID0;
unsigned long UARTPeriphID1;
unsigned long UARTPeriphID2;
unsigned long UARTPeriphID3;
unsigned long UARTPCellID0; 
unsigned long UARTPCellID1; 
unsigned long UARTPCellID2; 
unsigned long UARTPCellID3; 
} LM3SXXXX_UART;


typedef struct stLM3XXXXX_I2C
{
unsigned long I2CMSA;
unsigned long I2CMCS;
unsigned long I2CMDR;
unsigned long I2CMTPR;
unsigned long I2CMIMR;
unsigned long I2CMRIS;
unsigned long I2CMMIS;
unsigned long I2CMICR;
unsigned long I2CMCR;  
unsigned long ulRes[503];
unsigned long I2CSOAR;
unsigned long I2CSCSR;
unsigned long I2CSDR;
unsigned long I2CSIMR;
unsigned long I2CSRIS;
unsigned long I2CSMIS;
unsigned long I2CSICR;
} LM3XXXXX_I2C;


typedef struct stLM3SXXXX_FLASH_CONTROL
{
unsigned long FMA;
unsigned long FMD;
unsigned long FMC;
unsigned long FCRIS;
unsigned long FCIM;
unsigned long FCMISC;
#ifdef _TEMPEST_CLASS                                                    // {4}
    unsigned long ulRes1[2];
    unsigned long FMC2;
    unsigned long ulRes2[3];
    unsigned long FWBVAL;
    unsigned long ulRes3[31];
    unsigned long FCTL;
    unsigned long ulRes4;
    unsigned long FWB0;
    unsigned long FWB1;
    unsigned long FWB2;
    unsigned long FWB3;
    unsigned long FWB4;
    unsigned long FWB5;
    unsigned long FWB6;
    unsigned long FWB7;
    unsigned long FWB8;
    unsigned long FWB9;
    unsigned long FWB10;
    unsigned long FWB11;
    unsigned long FWB12;
    unsigned long FWB13;
    unsigned long FWB14;
    unsigned long FWB15;
    unsigned long FWB16;
    unsigned long FWB17;
    unsigned long FWB18;
    unsigned long FWB19;
    unsigned long FWB20;
    unsigned long FWB21;
    unsigned long FWB22;
    unsigned long FWB23;
    unsigned long FWB24;
    unsigned long FWB25;
    unsigned long FWB26;
    unsigned long FWB27;
    unsigned long FWB28;
    unsigned long FWB29;
    unsigned long FWB30;
    unsigned long FWB31;
#endif

} LM3SXXXX_FLASH_CONTROL;

typedef struct stLM3SXXXX_SYS_CONTROL
{
const unsigned long DID0;
const unsigned long DID1;
const unsigned long DC0;
const unsigned long ulRes1;
const unsigned long DC1;
const unsigned long DC2;
const unsigned long DC3;
const unsigned long DC4;
const unsigned long DC5;
const unsigned long DC6;
const unsigned long DC7;
const unsigned long DC8;
unsigned long PBORCTL;
unsigned long LDOPCTL;
const unsigned long ulRes3[2];
unsigned long SRCR0;
unsigned long SRCR1;
unsigned long SRCR2;
const unsigned long ulRes4;
volatile unsigned long RIS;
unsigned long IMC;
unsigned long MISC;
unsigned long RESC;
unsigned long RCC;
volatile unsigned long PLLCFG;
const unsigned long ulRes5[1];
unsigned long GPIOHSCTL;
unsigned long RCC2;
const unsigned long ulRes6[2];
unsigned long MOSCCTL;                                                   // only Dust-Devil and Tempest classes
#ifdef _TEMPEST_CLASS                                                    // {4}
    const unsigned long ulRes6a[28];
    unsigned long RMCTL;
    unsigned long RMVER;
    const unsigned long ulRes6b;
#else
    const unsigned long ulRes6a[32];
#endif
unsigned long RCGC0;
unsigned long RCGC1;
unsigned long RCGC2;
const unsigned long ulRes7;
unsigned long SCGC0;
unsigned long SCGC1;
unsigned long SCGC2;
const unsigned long ulRes8;
unsigned long DCGC0;
unsigned long DCGC1;
unsigned long DCGC2;
const unsigned long ulRes9;
unsigned long FMPRE0;
unsigned long FMPPE0;
const unsigned long ulRes10[3];
unsigned long DSLPCLKCFG;
const unsigned long ulRes11[34];
unsigned long USECRL;
unsigned long USER_DBG;
const unsigned long ulRes12[3];
unsigned long USER_REG0;
unsigned long USER_REG1;
#ifdef _TEMPEST_CLASS                                                    // {4}
    unsigned long USER_REG2;
    unsigned long USER_REG3;
    const unsigned long ulRes13[5];
#else
    const unsigned long ulRes13[7];
#endif
unsigned long FMPRE1;
unsigned long FMPRE2;
unsigned long FMPRE3;
const unsigned long ulRes14[125];
unsigned long FMPPE1;
unsigned long FMPPE2;
unsigned long FMPPE3;
} LM3SXXXX_SYS_CONTROL;

typedef struct stLM3SXXXX_uDMA
{
unsigned long DMASTAT;
unsigned long DMACFG;
unsigned long DMACTLBASE;
unsigned long DMAALTBASE;
unsigned long DMAWAITSTAT;
unsigned long DMASWREQ;
unsigned long DMAUSEBURSTSET;
unsigned long DMAUSEBURSTCLR;
unsigned long DMAREQMASKSET;
unsigned long DMAREQMASKCLR;
unsigned long DMAENASET;
unsigned long DMAENACLR;
unsigned long DMAALTSET;
unsigned long DMAALTCLR;
unsigned long DMAPRIOSET;
unsigned long DMAPRIOCLR;
unsigned long ulRes1[3];
unsigned long DMAERRCLR;
unsigned long ulRes2[992];
unsigned long DMAPeriphID4;
unsigned long ulRes3[3];
unsigned long DMAPeriphID0;
unsigned long DMAPeriphID1;
unsigned long DMAPeriphID2;
unsigned long DMAPeriphID3;
unsigned long DMACellID0;
unsigned long DMACellID1;
unsigned long DMACellID2;
unsigned long DMACellID3;
} LM3SXXXX_uDMA;


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
    unsigned long ulR0;                                                  // low registers
    unsigned long ulR1;
    unsigned long ulR2;
    unsigned long ulR3;
    unsigned long ulR4;
    unsigned long ulR5;
    unsigned long ulR6;
    unsigned long ulR7;
    unsigned long ulR8;                                                  // high registers
    unsigned long ulR9;
    unsigned long ulR10;
    unsigned long ulR11;
    unsigned long ulR12;
    unsigned long ulR13_SP;                                              // stack pointer
    unsigned long ulPSP;                                                 // banked versions of stack pointer
    unsigned long ulMSP;
    unsigned long ulR14_LR;                                              // link register
    unsigned long ulR15_PC;                                              // program counter
                                                                         // Special registers
                                                                         //
    unsigned long ulPSR;                                                 // program status register
    unsigned long ulPRIMASK;
    unsigned long ulFAULTMASK;
    unsigned long ulBASEPRI;
    unsigned long ulCONTROL;        
} LM3SXXXX_CORTEX_M3_REGS;

#define INTERRUPT_MASKED 0x00000001







// A copy of the LM3SXXXX peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)

typedef struct stLM3SXXXX_PERIPH
{
    LM3SXXXX_WATCHDOG WATCHDOG;
    LM3SXXXX_GPT GPT[4];                                                 // {2}
    LM3SXXXX_ADC ADC[2];                                                 // {3}
    LM3SXXXX_PIO GPIO_PORT_A;
    LM3SXXXX_PIO GPIO_PORT_B;
    LM3SXXXX_PIO GPIO_PORT_C;
    LM3SXXXX_PIO GPIO_PORT_D;
    LM3SXXXX_SSI SSI[1];
    LM3SXXXX_UART UART[3];
    LM3XXXXX_I2C I2C[2];
    LM3SXXXX_PIO GPIO_PORT_E;
    LM3SXXXX_PIO GPIO_PORT_F;
    LM3SXXXX_PIO GPIO_PORT_G;
    LM3SXXXX_PIO GPIO_PORT_H;
    LM3SXXXX_PIO GPIO_PORT_J;
    LM3SXXXX_PWM PWM;                                                    // {5}
    LM3SXXXX_CAN CAN;
    LM3SXXXX_ETH ETHERNET;
    LM3SXXXX_USB USB;                                                    // {1} 
    LM3SXXXX_FLASH_CONTROL  FLASH_CONTROL;
    LM3SXXXX_SYS_CONTROL    SYSTEM_CONTROL;                              // {1}
    LM3SXXXX_uDMA           uDMA;

    LM3SXXXX_CORTEX_M3      CORTEX_M3;

    LM3SXXXX_CORTEX_M3_REGS CORTEX_M3_REGS;                              // only for simulation use

} LM3SXXXX_PERIPH;

extern LM3SXXXX_PERIPH  LM3Sxxxx;

#define _PORTS_AVAILABLE_       PORTS_AVAILABLE

#define _PORTA                  0                                        // {6} references for display in simulator
#define _PORTB                  1
#define _PORTC                  2
#define _PORTD                  3
#define _PORTE                  4
#define _PORTF                  5
#define _PORTG                  6
#define _PORTH                  7
#define _PORTJ                  8

#define _GPIO_ADC               (_PORTS_AVAILABLE_)                      // dedicated ADC pins

#define _PORT_EXT_0             (_PORTS_AVAILABLE_ + 1)                  // {7}
#define _PORT_EXT_1             (_PORTS_AVAILABLE_ + 2)                  // these port numbers should be in order following the internal ports and a fictive ADC port
#define _PORT_EXT_2             (_PORTS_AVAILABLE_ + 3)
#define _PORT_EXT_3             (_PORTS_AVAILABLE_ + 4)
#define _PORT_EXT_4             (_PORTS_AVAILABLE_ + 5)
#define _PORT_EXT_5             (_PORTS_AVAILABLE_ + 6)
#define _PORT_EXT_6             (_PORTS_AVAILABLE_ + 7)
#define _PORT_EXT_7             (_PORTS_AVAILABLE_ + 8)







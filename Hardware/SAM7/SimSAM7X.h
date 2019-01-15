/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    SimSAM7X.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************

    23.12.2007 Extend USART with Peripheral DMA Registers                {1}
    11.05.2008 Remove const definition of MC_FSR                         {2}
    14.06.2008 Add USB registers                                         {3}
    05.10.2008 Add DBGU UART interface                                   {4}
    23.12.2008 Add ADC DMA support                                       {5}
    17.04.2009 Correct UART DMA offset                                   {6}
    16.08.2009 Add SSC block                                             {7}
    14.01.2010 Add PWMC block                                            {8}
    13.02.2010 Add SAM7Sxxx                                              {9}
    30.03.2010 Add FLASH controller 1 for 512k devices                   {10}
    12.10.2010 Add SAM7SExxx                                             {11}
    09.02.2011 Add RTT                                                   {12}
    26.01.2013 Add SAM7L                                                 {13}

*/


typedef struct stSAM7X_TIMER
{
unsigned long       TC_CCR;
unsigned long       TC_CMR;
unsigned long       Res_1[2];
const unsigned long TC_CV;
unsigned long       TC_RA;
unsigned long       TC_RB;
unsigned long       TC_RC;
const unsigned long TC_SR;
unsigned long       TC_IER;
unsigned long       TC_IDR;
const unsigned long TC_IMR;
} SAM7X_TIMER;

typedef struct stSAM7X_TIMER_CTL
{
unsigned long       TC_BCR;
unsigned long       TC_BMR;
} SAM7X_TIMER_CTL;


typedef struct stSAM7X_PIT
{
unsigned long       PIT_MR;
const unsigned long PIT_SR;
const unsigned long PIT_PIVR;
const unsigned long PIT_PIIR;
} SAM7X_PIT;


typedef struct stSAM7X_USB                                               // {3}
{
unsigned long UDP_FRM_NUM;
unsigned long UDP_GBL_STAT;
unsigned long UDP_FADDR;
unsigned long ulRes0;
unsigned long UDP_IER;
unsigned long UDP_IDR;    
unsigned long UDP_IMR;    
unsigned long UDP_ISR;    
unsigned long UDP_ICR;
unsigned long ulRes1;    
unsigned long UDP_RST_EP;
unsigned long ulRes2;
unsigned long UDP_CSR0; 
unsigned long UDP_CSR1;   
unsigned long UDP_CSR2;   
unsigned long UDP_CSR3;   
unsigned long UDP_CSR4;   
unsigned long UDP_CSR5;
#if defined _HW_SAM7SE
    unsigned long UDP_CSR6;   
    unsigned long UDP_CSR7;
#else
    unsigned long ulRes3[2];
#endif
unsigned long UDP_FDR0;   
unsigned long UDP_FDR1;   
unsigned long UDP_FDR2;   
unsigned long UDP_FDR3;   
unsigned long UDP_FDR4;   
unsigned long UDP_FDR5;
#if defined _HW_SAM7SE
    unsigned long UDP_FDR6;   
    unsigned long UDP_FDR7;  
#else
    unsigned long ulRes4[3];  
#endif
unsigned long UDP_TXVC;   
} SAM7X_USB;

typedef struct stSAM7X_TWI
{
      unsigned long TWI_CR;
const unsigned long TWI_MMR;   
const unsigned long ulRes1;
      unsigned long TWI_IADR;   
      unsigned long TWI_CWGR;   
const unsigned long ulRes2[3];
const unsigned long TWI_SR;     
      unsigned long TWI_IER;    
      unsigned long TWI_IDR;    
const unsigned long TWI_IMR;    
const unsigned long TWI_RHR;    
      unsigned long TWI_THR;    
} SAM7X_TWI;



typedef struct stSAM7X_UART
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

unsigned long       US_RESERVED[43];                                     // {6}

unsigned long       US_RPR;                                              // {2}
unsigned long       US_RCR;
unsigned long       US_TPR;
unsigned long       US_TCR;
unsigned long       US_RNPR;
unsigned long       US_RNCR;
unsigned long       US_TNPR;
unsigned long       US_TNCR;
unsigned long       US_PTCR;
unsigned long       US_PTSR;
} SAM7X_UART;



typedef struct stSAM7X_PWMC                                              // {8}
{
unsigned long       PWM_MR;
unsigned long       PWM_ENA;
unsigned long       PWM_DIS;
unsigned long       PWM_SR;
unsigned long       PWM_IER;
unsigned long       PWM_IDR;
unsigned long       PWM_IMR;
unsigned long       PWM_ISR;

unsigned long       ulRes1[120];

unsigned long       PWM_CMR_0;
unsigned long       PWM_CDTY_0;
unsigned long       PWM_CPRD_0;
unsigned long       PWM_CCNT_0;
unsigned long       PWM_CUPD_0;

unsigned long       ulRes2[3];

unsigned long       PWM_CMR_1;

unsigned long       PWM_CDTY_1;
unsigned long       PWM_CPRD_1;
unsigned long       PWM_CCNT_1;
unsigned long       PWM_CUPD_1;

unsigned long       ulRes3[3];

unsigned long       PWM_CMR_2;
unsigned long       PWM_CDTY_2;
unsigned long       PWM_CPRD_2;
unsigned long       PWM_CCNT_2;
unsigned long       PWM_CUPD_2;

unsigned long       ulRes4[3];

unsigned long       PWM_CMR_3;
unsigned long       PWM_CDTY_3;
unsigned long       PWM_CPRD_3;
unsigned long       PWM_CCNT_3;
unsigned long       PWM_CUPD_3;
} SAM7X_PWMC;



typedef struct stSAM7X_DBGU                                              // {4}
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
unsigned long       usReserve1[7];
unsigned long       DBGU_CIDR;
unsigned long       DBGU_EXID;
unsigned long       DBGU_FNR;
unsigned long       usReserve2[45];
unsigned long       usReservePDC[10];
} SAM7X_DBGU;


typedef struct stSAM7X_CAN
{
unsigned long       CAN_MR;      
unsigned long       CAN_IER;
unsigned long       CAN_IDR;  
const unsigned long CAN_IMR;      
const unsigned long CAN_SR;       
unsigned long       CAN_BR;       
const unsigned long CAN_TIM;      
const unsigned long CAN_TIMESTP;  
const unsigned long CAN_ECR;      
unsigned long       CAN_TCR;      
unsigned long       CAN_ACR;      

unsigned long       usReserve[177];

unsigned long       CAN_MMR0;     
unsigned long       CAN_MAM0;
unsigned long       CAN_MID0;     
unsigned long       CAN_MFID0;    
unsigned long       CAN_MSR0;     
unsigned long       CAN_MDL0;     
unsigned long       CAN_MDH0;     
unsigned long       CAN_MCR0;     

unsigned long       CAN_MMR1;     
unsigned long       CAN_MAM1;
unsigned long       CAN_MID1;     
unsigned long       CAN_MFID1;    
unsigned long       CAN_MSR1;     
unsigned long       CAN_MDL1;     
unsigned long       CAN_MDH1;     
unsigned long       CAN_MCR1;     

unsigned long       CAN_MMR2;     
unsigned long       CAN_MAM2;
unsigned long       CAN_MID2;     
unsigned long       CAN_MFID2;    
unsigned long       CAN_MSR2;    
unsigned long       CAN_MDL2;     
unsigned long       CAN_MDH2;     
unsigned long       CAN_MCR2;     

unsigned long       CAN_MMR3;     
unsigned long       CAN_MAM3;
unsigned long       CAN_MID3;     
unsigned long       CAN_MFID3;    
unsigned long       CAN_MSR3;     
unsigned long       CAN_MDL3;     
unsigned long       CAN_MDH3;     
unsigned long       CAN_MCR3;     

unsigned long       CAN_MMR4;    
unsigned long       CAN_MAM4;
unsigned long       CAN_MID4;     
unsigned long       CAN_MFID4;    
unsigned long       CAN_MSR4;    
unsigned long       CAN_MDL4;     
unsigned long       CAN_MDH4;     
unsigned long       CAN_MCR4;     

unsigned long       CAN_MMR5;     
unsigned long       CAN_MAM5;
unsigned long       CAN_MID5;     
unsigned long       CAN_MFID5;    
unsigned long       CAN_MSR5;    
unsigned long       CAN_MDL5;    
unsigned long       CAN_MDH5;    
unsigned long       CAN_MCR5;    

unsigned long       CAN_MMR6;    
unsigned long       CAN_MAM6;
unsigned long       CAN_MID6;    
unsigned long       CAN_MFID6;    
unsigned long       CAN_MSR6;   
unsigned long       CAN_MDL6;    
unsigned long       CAN_MDH6;    
unsigned long       CAN_MCR6;    

unsigned long       CAN_MMR7;    
unsigned long       CAN_MAM7;
unsigned long       CAN_MID7;    
unsigned long       CAN_MFID7;    
unsigned long       CAN_MSR7;   
unsigned long       CAN_MDL7;    
unsigned long       CAN_MDH7;    
unsigned long       CAN_MCR7;    
} SAM7X_CAN;

typedef struct stSAM7X_SSC                                               // {7}
{
unsigned long       SSC_CR;
unsigned long       SSC_CMR;
unsigned long       res0[2];
unsigned long       SSC_RCMR;
unsigned long       SSC_RFMR;
unsigned long       SSC_TCMR;
unsigned long       SSC_TFMR;
unsigned long       SSC_RHR;
unsigned long       SSC_THR;
unsigned long       res1[2];
unsigned long       SSC_RSHR;
unsigned long       SSC_TSHR;
unsigned long       SSC_RC0R;
unsigned long       SSC_RC1R;
unsigned long       SSC_SR;
unsigned long       SSC_IER;
unsigned long       SSC_IDR;
unsigned long       SSC_IMR;
unsigned long       res2[44];
unsigned long       SSC_RPR;
unsigned long       SSC_RCR;
unsigned long       SSC_TPR;
unsigned long       SSC_TCR;
unsigned long       SSC_RNPR;
unsigned long       SSC_RNCR;
unsigned long       SSC_TNPR;
unsigned long       SSC_TNCR;
unsigned long       SSC_PTCR;
unsigned long       SSC_PTSR;
} SAM7X_SSC;


typedef struct stSAM7X_ADC
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

const unsigned long res1[44];                                            // {5}
unsigned long       ADC_RPR;
unsigned long       ADC_RCR;
unsigned long       ADC_TPR;
unsigned long       ADC_TCR;
unsigned long       ADC_RNPR;
unsigned long       ADC_RNCR;
unsigned long       ADC_TNPR;
unsigned long       ADC_TNCR;
unsigned long       ADC_PTCR;
unsigned long       ADC_PTSR;
} SAM7X_ADC;



typedef struct stSAM7X_MAC
{
unsigned long       EMAC_NCR;     
unsigned long       EMAC_NCFG;     
unsigned long       EMAC_NSR;      
unsigned long       EMAC_RES1;     
unsigned long       EMAC_RES2;     
unsigned long       EMAC_TSR;      
unsigned long       EMAC_RBQP;     
unsigned long       EMAC_TBQP;     
unsigned long       EMAC_RSR;      
unsigned long       EMAC_ISR;      
unsigned long       EMAC_IER;      
unsigned long       EMAC_IDR;      
unsigned long       EMAC_IMR;      
unsigned long       EMAC_MAN;
unsigned long       EMAC_PTR;   
unsigned long       EMAC_PFR;   
unsigned long       EMAC_FTO;   
unsigned long       EMAC_SCF;   
unsigned long       EMAC_MCF;   
unsigned long       EMAC_FRO;   
unsigned long       EMAC_FCSE;  
unsigned long       EMAC_ALE;   
unsigned long       EMAC_DTF;   
unsigned long       EMAC_LCOL;  
unsigned long       EMAC_ECOL;  
unsigned long       EMAC_TUND;  
unsigned long       EMAC_CSE;   
unsigned long       EMAC_RRE;   
unsigned long       EMAC_ROV;   
unsigned long       EMAC_RSE;   
unsigned long       EMAC_ELE;   
unsigned long       EMAC_RJA;   
unsigned long       EMAC_USF;   
unsigned long       EMAC_STE;   
unsigned long       EMAC_RLE;   
unsigned long       EMAC_RES3;  
unsigned long       EMAC_HRB;   
unsigned long       EMAC_HRT;   
unsigned long       EMAC_SA1B;  
unsigned long       EMAC_SA1T;  
unsigned long       EMAC_SA2B;  
unsigned long       EMAC_SA2T;  
unsigned long       EMAC_SA3B;  
unsigned long       EMAC_SA3T;  
unsigned long       EMAC_SA4B;  
unsigned long       EMAC_SA4T;  
unsigned long       EMAC_TID;   
unsigned long       EMAC_RES4;  
unsigned long       EMAC_USRIO; 
} SAM7X_MAC;

typedef struct stSAM7X_SPI
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
} SAM7X_SPI;


typedef struct stSAM7X_RTT                                               // {12}
{
unsigned long       RTT_MR;
unsigned long       RTT_AR;
unsigned long       RTT_VR;
unsigned long       RTT_SR;
} SAM7X_RTT;


typedef struct stSAM7X_WDT
{
unsigned long       WDT_CR;
unsigned long       WDT_MR;
const unsigned long WDT_SR;
} SAM7X_WDT;


typedef struct stSAM7X_PMC
{
unsigned long       PMC_SCER;
unsigned long       PMC_SCDR;
const unsigned long PMC_SCSR;
unsigned long       PMC_RES1;
unsigned long       PMC_PCER;
unsigned long       PMC_PCDR;
const unsigned long PMC_PCSR;
unsigned long       PMC_RES2;
unsigned long       CKGR_MOR;
const unsigned long CKGR_MCFR;
unsigned long       PMC_RES3;
unsigned long       CKGR_PLLR;
unsigned long       PMC_MCKR;
unsigned long       PMC_ACKR;
unsigned long       PMC_RES4;
unsigned long       PMC_RES5;
unsigned long       PMC_PCK0;
unsigned long       PMC_PCK1;
unsigned long       PMC_RESA;
unsigned long       PMC_RESB;
unsigned long       PMC_RESC;
unsigned long       PMC_RESD;
unsigned long       PMC_RESE;
unsigned long       PMC_RESF;
unsigned long       PMC_IER; 
unsigned long       PMC_IDR; 
unsigned long       PMC_SR;  
unsigned long       PMC_IMR; 
} SAM7X_PMC;



typedef struct stSAM7X_RST
{
unsigned long       RSTC_CR;
const unsigned long RSTC_SR;
const unsigned long RSTC_MR;
} SAM7X_RST;


typedef struct stSAM7X_MC
{
const unsigned long MC_RES[24];
unsigned long       MC_FMR;
unsigned long       MC_FCR;
unsigned long       MC_FSR;                                              // {2}
const unsigned long MC_RES1;                                             // {10}
unsigned long       MC_FMR1;
unsigned long       MC_FCR1;
unsigned long       MC_FSR1;
} SAM7X_MC;

#ifdef _HW_SAM7SE                                                        // {11}
typedef struct stSAM7X_EBI
{
unsigned long       EBI_CSA;
unsigned long       ulRes0[3];
unsigned long       EBI_SMC0;
unsigned long       EBI_SMC1;
unsigned long       EBI_SMC2;
unsigned long       EBI_SMC3;
unsigned long       EBI_SMC4;
unsigned long       EBI_SMC5;
unsigned long       EBI_SMC6;
unsigned long       EBI_SMC7;
unsigned long       EBI_SDRAM0;
unsigned long       EBI_SDRAM1;
unsigned long       EBI_SDRAM2;
unsigned long       EBI_SDRAM3;
unsigned long       EBI_SDRAM4;
unsigned long       EBI_SDRAM5;
unsigned long       EBI_SDRAM6;
unsigned long       EBI_SDRAM7;
unsigned long       EBI_SDRAM8;
unsigned long       EBI_SDRAM9;
unsigned long       EBI_SDRAM10;
unsigned long       EBI_ECC0;
unsigned long       EBI_ECC1;
unsigned long       EBI_ECC2;
unsigned long       EBI_ECC3;
unsigned long       EBI_ECC4;  
} SAM7X_EBI;

typedef struct stSAM7X_SMC
{
unsigned long       SMC_CSR0;
unsigned long       SMC_CSR1;
unsigned long       SMC_CSR2;
unsigned long       SMC_CSR3;
unsigned long       SMC_CSR4;
unsigned long       SMC_CSR5;
unsigned long       SMC_CSR6;
unsigned long       SMC_CSR7;
} SAM7X_SMC;

typedef struct stSAM7X_ECC
{
unsigned long       ECC_CR;
unsigned long       ECC_MR;
unsigned long       ECC_SR;
unsigned long       ECC_PR;
unsigned long       ECC_NPR;
} SAM7X_ECC;
#endif

typedef struct stSAM7X_AIC
{
unsigned long       AIC_SMR0;    
unsigned long       AIC_SMR1;    
unsigned long       AIC_SMR2;    
unsigned long       AIC_SMR3;    
unsigned long       AIC_SMR4;    
unsigned long       AIC_SMR5;    
unsigned long       AIC_SMR6;    
unsigned long       AIC_SMR7;    
unsigned long       AIC_SMR8;    
unsigned long       AIC_SMR9;    
unsigned long       AIC_SMR10;   
unsigned long       AIC_SMR11;   
unsigned long       AIC_SMR12;   
unsigned long       AIC_SMR13;   
unsigned long       AIC_SMR14;   
unsigned long       AIC_SMR15;   
unsigned long       AIC_SMR16;   
unsigned long       AIC_SMR17;   
unsigned long       AIC_SMR18;   
unsigned long       AIC_SMR19;   
unsigned long       AIC_SMR20;   
unsigned long       AIC_SMR21;   
unsigned long       AIC_SMR22;   
unsigned long       AIC_SMR23;   
unsigned long       AIC_SMR24;   
unsigned long       AIC_SMR25;   
unsigned long       AIC_SMR26;   
unsigned long       AIC_SMR27;   
unsigned long       AIC_SMR28;   
unsigned long       AIC_SMR29;   
unsigned long       AIC_SMR30;  
unsigned long       AIC_SMR31;   
unsigned long       AIC_SVR0;    
unsigned long       AIC_SVR1;    
unsigned long       AIC_SVR2;    
unsigned long       AIC_SVR3;    
unsigned long       AIC_SVR4;    
unsigned long       AIC_SVR5;    
unsigned long       AIC_SVR6;    
unsigned long       AIC_SVR7;    
unsigned long       AIC_SVR8;    
unsigned long       AIC_SVR9;    
unsigned long       AIC_SVR10;   
unsigned long       AIC_SVR11;   
unsigned long       AIC_SVR12;   
unsigned long       AIC_SVR13;   
unsigned long       AIC_SVR14;   
unsigned long       AIC_SVR15;   
unsigned long       AIC_SVR16;   
unsigned long       AIC_SVR17;   
unsigned long       AIC_SVR18;   
unsigned long       AIC_SVR19;   
unsigned long       AIC_SVR20;   
unsigned long       AIC_SVR21;   
unsigned long       AIC_SVR22;   
unsigned long       AIC_SVR23;   
unsigned long       AIC_SVR24;   
unsigned long       AIC_SVR25;   
unsigned long       AIC_SVR26;   
unsigned long       AIC_SVR27;   
unsigned long       AIC_SVR28; 
unsigned long       AIC_SVR29;   
unsigned long       AIC_SVR30;   
unsigned long       AIC_SVR31;   
unsigned long       AIC_IVR;     
unsigned long       AIC_FVR;     
unsigned long       AIC_ISR;     
unsigned long       AIC_IPR;     
unsigned long       AIC_IMR;     
unsigned long       AIC_CISR;    
unsigned long       ulRes1;    
unsigned long       ulRes2;    
unsigned long       AIC_IECR;    
unsigned long       AIC_IDCR;    
unsigned long       AIC_ICCR;    
unsigned long       AIC_ISCR;    
unsigned long       AIC_EOICR;
unsigned long       AIC_SPU;     
unsigned long       AIC_DCR;     
unsigned long       ulRes3;    
unsigned long       AIC_FFER;    
unsigned long       AIC_FFDR;    
unsigned long       AIC_FFSR;  
} SAM7X_AIC;



typedef struct stSAM7X_PIO
{
unsigned long       PIO_PER;
unsigned long       PIO_PDR;  
const unsigned long PIO_PSR;  
unsigned long       PIO_RES1;  
unsigned long       PIO_OER;   
unsigned long       PIO_ODR; 
const unsigned long PIO_OSR;   
unsigned long       PIO_RES2;  
unsigned long       PIO_IFER;  
unsigned long       PIO_IFDR;  
const unsigned long PIO_IFSR;  
unsigned long       PIO_RES3;  
unsigned long       PIO_SODR;  
unsigned long       PIO_CODR;  
const unsigned long PIO_ODSR;  
const unsigned long PIO_PDSR;  
unsigned long       PIO_IER;       
unsigned long       PIO_IDR;       
const unsigned long PIO_IMR;       
const unsigned long PIO_ISR;       
unsigned long       PIO_MDER;      
unsigned long       PIO_MDDR;      
const unsigned long PIO_MDSR;      
unsigned long       PIO_RES4;       
unsigned long       PIO_PUDR;       
unsigned long       PIO_PUER;       
const unsigned long PIO_PUSR;       
unsigned long       PIO_RES5;       
unsigned long       PIO_ASR;        
unsigned long       PIO_BSR;        
const unsigned long PIO_ABSR;       
unsigned long       PIO_RES6;       
unsigned long       PIO_RES7;       
unsigned long       PIO_RES8;       
unsigned long       PIO_RES9;       
unsigned long       PIO_RES10;      
unsigned long       PIO_RES11;      
unsigned long       PIO_RES12;      
unsigned long       PIO_RES13;      
unsigned long       PIO_RES14;      
unsigned long       PIO_OWER;       
unsigned long       PIO_OWDR;       
const unsigned long PIO_OWSR;       

} SAM7X_PIO;



// A copy of the SAM7X peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)

typedef struct stSAM7X_PERIPH
{
    SAM7X_TIMER      ucTimer0;
    SAM7X_TIMER      ucTimer1;
    SAM7X_TIMER      ucTimer2;
    SAM7X_TIMER_CTL  ucTimerCtl;
    SAM7X_USB        ucUSB;                                              // {3}
    SAM7X_TWI        ucSimTWI;
    SAM7X_UART       ucSimUart0;                                         // keep two UART blocks together
    SAM7X_UART       ucSimUart1;
    SAM7X_PWMC       ucPWMC;                                             // {8}
    SAM7X_CAN        ucSimCAN;
    SAM7X_SSC        ucSimSSC;                                           // {7}
    SAM7X_ADC        ucSimADC;
    SAM7X_MAC        ucSimMAC;
    SAM7X_SPI        ucSimSPI0;
#ifndef _HW_SAM7S
    SAM7X_SPI        ucSimSPI1;
#endif

    SAM7X_AIC        ucSimAIC;
    SAM7X_DBGU       ucSimDBGU;                                          // {4}
    SAM7X_PIT        ucSimPIT;
    SAM7X_PIO        ucSimPIOA;
    SAM7X_PIO        ucSimPIOB;
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {11}{13}
    SAM7X_PIO        ucSimPIOC;
#endif
    SAM7X_PMC        ucSimPMC;
    SAM7X_RST        ucSimRST;
    SAM7X_RTT        ucSimRTT;                                           // {12}
    SAM7X_WDT        ucSimWDT;
    SAM7X_MC         ucSimMC;
  #ifdef _HW_SAM7SE                                                      // {11}
    SAM7X_EBI        ucSimEBI;    
    SAM7X_SMC        ucSimSMC;
    SAM7X_ECC        ucSimECC;    
  #endif
} SAM7X_PERIPH;

extern SAM7X_PERIPH  ucSAM7x;

#if defined _HW_SAM7S                                                    // {9}
    #define _GPIO_A                     0                                // references for display in simulator
    #define _GPIO_ADC                   1                                // ADC port bits
#elif defined _HW_SAM7SE || defined _HW_SAM7L                            // {11}{13}
    #define _GPIO_A                     0                                // references for display in simulator
    #define _GPIO_B                     1
    #define _GPIO_C                     2
    #define _GPIO_ADC                   3
#else
    #define _GPIO_A                     0                                // references for display in simulator
    #define _GPIO_B                     1
    #define _GPIO_ADC                   2                                // {5} ADC port bits
#endif
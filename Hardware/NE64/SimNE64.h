/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:   	    SimNE64.h
   Project: 	Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   *********************************************************************/   
                           


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

typedef struct stNE64_CLK_MODE
{
unsigned char SYNR;
unsigned char REFDV;
unsigned char CTFLG;
unsigned char CRGFLG;
unsigned char CRGINT;
unsigned char CLKSEL;
unsigned char PLLCTL;
unsigned char RTICTL;
unsigned char COPCTL;
unsigned char FORBYP;
unsigned char CTCTL;
unsigned char ARMCOP;  
} NE64_CLK_MODE;


typedef struct stNE64_TIMERS
{
unsigned char TIOS;
unsigned char CFORC;
unsigned char OC7M;
unsigned char OC7D;
unsigned short TCNT_HI;
//unsigned char TCNT_LO;
unsigned char TSCR1;
unsigned char TTOV;
unsigned char TCTL1;
unsigned char RES_1;
unsigned char TCTL3;
unsigned char RES_2;  
unsigned char TIE;
unsigned char TSCR2;
unsigned char TFLG1;
unsigned char TFLG2;
unsigned char RES_3; 
unsigned char RES_4; 
unsigned char RES_5; 
unsigned char RES_6; 
unsigned char RES_7; 
unsigned char RES_8; 
unsigned char RES_9; 
unsigned char RES_10; 
unsigned short TC4_HI;
//unsigned char TC4_LO;
unsigned short TC5_HI;
//unsigned char TC5_LO;
unsigned short TC6_HI;
//unsigned char TC6_LO;
unsigned short TC7_HI;
//unsigned char TC7_LO;
unsigned char PACTL;
unsigned char PAFLG;
unsigned char PACNT_HI;
unsigned char PACNT_LO;
unsigned char RES_11;
unsigned char RES_12;
unsigned char RES_13;
unsigned char RES_14;
unsigned char RES_15;
unsigned char RES_16;
unsigned char RES_17;
unsigned char RES_18;
unsigned char RES_19;
unsigned char TIMTST;
unsigned char RES_20;
unsigned char RES_21;
} NE64_TIMERS;


typedef struct stNE64_ATD_BLOCK
{
unsigned char ATDCTL0;
unsigned char ATDCTL1;
unsigned char ATDCTL2;
unsigned char ATDCTL3;
unsigned char ATDCTL4;
unsigned char ATDCTL5;
unsigned char ATDSTAT0;
unsigned char ATD_UNUSED1;
unsigned char ATDTEST0;
unsigned char ATDTEST1;
unsigned char ATD_UNUSED_2;
unsigned char ATDSTAT1;  
unsigned char ATD_UNUSED_3;
unsigned char ATDDIEN;
unsigned char ATD_UNUSED_4;
unsigned char PORTAD;
unsigned char ATDDR0H;
unsigned char ATDDR0L;   
unsigned char ATDDR1H;
unsigned char ATDDR1L;
unsigned char ATDDR2H;
unsigned char ATDDR2L;
unsigned char ATDDR3H;
unsigned char ATDDR3L;
unsigned char ATDDR4H;
unsigned char ATDDR4L;   
unsigned char ATDDR5H;
unsigned char ATDDR5L;
unsigned char ATDDR6H;
unsigned char ATDDR6L;
unsigned char ATDDR7H;
unsigned char ATDDR7L;          
} NE64_ATD_BLOCK;



typedef struct stNE64_MEM
{
unsigned char  PORTA;
unsigned char  PORTB;
unsigned char  DDRA;
unsigned char  DDRB;
unsigned char  ucRes1[4];
unsigned char  PORTE;
unsigned char  DDRE;
unsigned char  REAR;
unsigned char  MODE;
unsigned char  PUCR;
unsigned char  RDRIV;
unsigned char  EBICTL;
unsigned char  ucRes2;
unsigned char  INITRM;
unsigned char  INITRG;
unsigned char  INITEE;
unsigned char  MISC;
unsigned char  MTST0;
unsigned char  ITCR;
unsigned char  ITEST;
unsigned char  MTST1;
unsigned char  ucRes3[2];
unsigned short PARTID;
unsigned char  MEMSIZ0;
unsigned char  MEMSIZ1;
unsigned char  IRQCR;
unsigned char  HPRIO;
unsigned char  DBGC1;
unsigned char  DBGSC;
unsigned short DBGTB;
unsigned char  DBGCNT;
unsigned char  DBGCCX;
unsigned short DBGCC;
unsigned char  DBGC2;
unsigned char  DBGC3;
unsigned char  DBGCAX;
unsigned short DBGCA;
unsigned char  DBGCBX;
unsigned short DBGCB;
unsigned char  PPAGE;
unsigned char  uRes4;
unsigned char  PORTK;
unsigned char  DDRK;
} NE64_MEM;


typedef struct stNE64_PIM
{
unsigned char PTT  ;
unsigned char PTIT ;
unsigned char DDRT ;
unsigned char RDRT ;
unsigned char PERT ;
unsigned char PPST ;
unsigned char RES1 ;
unsigned char RES2 ;
unsigned char PTS  ;
unsigned char PTIS ;
unsigned char DDRS ;
unsigned char RDRS ;
unsigned char PERS ;
unsigned char PPSS ;
unsigned char WOMS ;
unsigned char RES3 ;
unsigned char PTG  ;
unsigned char PTIG ;
unsigned char DDRG ;
unsigned char RDRG ;
unsigned char PERG ;
unsigned char PPSG ;
unsigned char PIEG ;
unsigned char PIFG ;
unsigned char PTH  ;
unsigned char PTIH ;
unsigned char DDRH ;
unsigned char RDRH ;
unsigned char PERH ;
unsigned char PPSH ;
unsigned char PIEH ;
unsigned char PIFH ;
unsigned char PTJ  ;
unsigned char PTIJ ;
unsigned char DDRJ ;
unsigned char RDRJ ;
unsigned char PERJ ;
unsigned char PPSJ ;
unsigned char PIEJ ;
unsigned char PIFJ ;
unsigned char PTL  ;
unsigned char PTIL  ;
unsigned char DDRL ;
unsigned char RDRL ;
unsigned char PERL ;
unsigned char PPSL ;
unsigned char WOML ;
} NE64_PIM; 


typedef struct stNE64_SCI
{
unsigned char SCIBDH_0;
unsigned char SCIBDL_0;
unsigned char SCICR1_0;
unsigned char SCICR2_0;
unsigned char SCISR1_0;
unsigned char SCISR2_0;
unsigned char SCIDRH_0;
unsigned char SCIDRL_0;
unsigned char SCIBDH_1;
unsigned char SCIBDL_1;
unsigned char SCICR1_1;
unsigned char SCICR2_1;
unsigned char SCISR1_1;
unsigned char SCISR2_1;
unsigned char SCIDRH_1;
unsigned char SCIDRL_1;
} NE64_SCI;


typedef struct stNE64_SPI
{
unsigned char SPICR1;
unsigned char SPICR2;
unsigned char SPIBR;
unsigned char SPISR;
unsigned char SPIres1;
unsigned char SPIDR;
unsigned char SPIres2;
unsigned char SPIres3;
} NE64_SPI;


typedef struct stNE64_IIC
{
unsigned char IBAD;
unsigned char IBFD;
unsigned char IBCR;
unsigned char IBSR;
unsigned char IBDR;
} NE64_IIC;


typedef struct stNE64_EPHY
{
unsigned char EPHYCTL0;
unsigned char EPHYCTL1;
unsigned char EPHYSR;
unsigned char EPHYTST;
} NE64_EPHY;



typedef struct stNE64_FLASH
{
unsigned char FCLKDIV;
unsigned char FSEC;
unsigned char F_RES1;
unsigned char FCNFG;
unsigned char FPROT;
unsigned char FSTAT;
unsigned char FCMD;
unsigned char F_RES2;
unsigned char FADDRHI;
unsigned char FADDRLO;
unsigned char FDATAHI;
unsigned char FDATALO;
unsigned char F_RES3;
unsigned char F_RES4;
unsigned char F_RES5;
unsigned char F_RES6;
} NE64_FLASH;


typedef struct stNE64_EMAC
{
unsigned char NETCTL;
unsigned char EMAC_RES_1;
unsigned char EMAC_RES_2;
unsigned char RXCTS;
unsigned char TXCTS;
unsigned char ETCTL;
unsigned short ETYLE;
unsigned short PTIME;
unsigned short IEVENT;
unsigned short IMASK;
unsigned char SWRST;
unsigned char EMAC_RES_3;
unsigned char MPADR;
unsigned char MRADR;
unsigned short MWDATA;
unsigned short MRDATA;
unsigned char MCMST;
unsigned char EMAC_RES_4;
unsigned short BUFCFG;
unsigned short RXAEFP;
unsigned short RXBEFP;
unsigned short TXEFP;
unsigned char MCHASH_7;
unsigned char MCHASH_6;
unsigned char MCHASH_5;
unsigned char MCHASH_4;
unsigned char MCHASH_3;
unsigned char MCHASH_2;
unsigned char MCHASH_1;
unsigned char MCHASH_0;
unsigned char MACAD_5;
unsigned char MACAD_4;
unsigned char MACAD_3;
unsigned char MACAD_2;
unsigned char MACAD_1;
unsigned char MACAD_0;
unsigned char EMISC_H;
unsigned char EMISC_L;
} NE64_EMAC;


extern unsigned char ucSimTxBuf [1536];

// A copy of the SAM7X peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)

typedef struct stNE64_PERIPH
{
    NE64_MEM           ucSimMebi;
    NE64_CLK_MODE      ucSimClockMod;
    NE64_TIMERS        ucSimTimers;
    NE64_ATD_BLOCK     ucSimATD;
    NE64_SCI           ucSimSCI;
    NE64_SPI           ucSimSPI;
    NE64_IIC           ucSimIIC;
    NE64_FLASH         ucSimFLASH;
    NE64_EPHY          ucSimEPHY;
    NE64_EMAC          ucSimEMAC;
    NE64_PIM           ucSimPIM;
} NE64_PERIPH;



extern NE64_PERIPH  ucNE64;

#define _PORT_A             0                                            // Port referencing in simulator
#define _PORT_B             1
#define _PORT_E             2
#define _PORT_G             3
#define _PORT_H             4
#define _PORT_J             5
#define _PORT_K             6
#define _PORT_L             7
#define _PORT_S             8
#define _PORT_T             9
#define _PORT_AD            10


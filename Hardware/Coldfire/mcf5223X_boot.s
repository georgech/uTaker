/*
 * File:	mcf5223X_boot.s
 *          12.08.2007 Added adjustment for boot loader with external SPI FLASH {1}
 *          21.01.2009 Added condition on KIRIN3                         // {2}
 */
 
#define _ASSEMBLER_CONFIG
#define _BOOT_LOADER_ASSEMBLER                                           // {1}
#define _M5223X
#include "config.h"

#define mcf52235_boot_init _mcf52235_boot_init
#define uTaskerBoot _uTaskerBoot
#define mcf5xxx_wr_vbr _mcf5xxx_wr_vbr
#define __IPSBAR        ___IPSBAR   
#define __SRAM          ___SRAM     
#define __FLASH         ___FLASH    
#define __SP_INIT       ___SP_INIT 

	.extern __IPSBAR
	.extern __SRAM
	.extern __FLASH
	.extern __SP_INIT 
	.extern mcf52235_boot_init	
	.extern uTaskerBoot	
	.extern _undef_int

	.global VECTOR_TABLE
	.global _VECTOR_TABLE
	.global start
	.global asm_startmeup
	.global _asm_startmeup
    .global asm_int_off
    .global _asm_int_off
    .global asm_int_on
    .global _asm_int_on 	
	.global mcf5xxx_wr_vbr	
	.text

/*
 * Exception Vector Table
 */
VECTOR_TABLE:
_VECTOR_TABLE:
INITSP:		.long	__SP_INIT				/* Initial SP			*/
INITPC:		.long	0x00000008				/* Initial PC			*/

asm_startmeup:                              /* needed to run from RAM (CW) */
_asm_startmeup:
start:
	move.w	#0x2700,sr
	
    /* Initialize RAMBAR1: locate SRAM and validate it */
	move.l	#__SRAM,d0
    add.l   #0x21,d0
    movec   d0,RAMBAR1
    
    /* Initialize IPSBAR */
	move.l	#__IPSBAR,d0
    add.l   #0x1,d0
	move.l	d0,0x40000000
	
    /* Initialize FLASHBAR */
    move.l  #__FLASH,d0
#if defined KIRIN3                                                       // {2} M5225X family have errata corrected
    add.l   #0x21,d0
#else
    add.l   #0x61,d0                                /* This is the workaround 1 due to the Internal FLASH Speculation error in the first devices */
#endif

    movec   d0,RAMBAR0

	/* Locate Stack Pointer */ 
	move.l	#__SP_INIT,sp
	
    /* Initialize the system */
	jsr		mcf52235_boot_init

	/* Jump to the main process */
	jsr		uTaskerBoot
	
    /* If the uTaskerBoot return (without interrupt) the normal code can run */
    /* we load its SP and the PC and let it continue... */
    #if defined SPI_SW_UPLOAD || defined KIRIN3
    move.l  #0x00001000,a0
    move.l  (a0),sp
    move.l  #0x00001004,a0
    move.l  (a0),a0               /* use for boot loader from external SPI FLASH */   
    #else
    move.l  #0x00000800,a0   
    move.l  (a0),sp
    move.l  #0x00000804,a0
    move.l  (a0),a0               /* use for standard boot loader from internal FLASH */
    #endif
    
    jmp (a0)                      /* jump to the main program */
    

asm_int_off:
_asm_int_off:
    link    A6,#-4
    movem.l d7,(SP)
    move.w  SR,D7      
    ori.l   #0x0700,d7  /* set interrupt mask  */ 
    move.w  D7,SR    
    movem.l (SP),D7
    lea     4(SP),SP        
    unlk    A6       
    rts
    
asm_int_on:
_asm_int_on:
    link    A6,#-4
    movem.l d7,(SP)
    move.w  SR,D7       
    andi.l   #0xf8ff,d7  /* enable interrupts  */ 
    move.w  D7,SR    
    movem.l (SP),D7
    lea     4(SP),SP        
    unlk    A6       
    rts  
    
    
	
mcf5xxx_wr_vbr:
    move.l  4(SP),D0
    .long   0x4e7b0801                 /* movec d0,VBR */
    nop
    rts	

	.end

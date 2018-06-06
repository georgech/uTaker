/*
 * File:	mcf5223X_GNU_boot.s
 * Purpose:	Start up MCF5223X and low level assember routines - for use by BM boot loader
 */

	.text

    .globl __interrupt_vector
	.globl mcf5xxx_wr_vbr
	.globl asm_int_off
	.globl asm_int_on
	.globl byte_rev
	.globl start_application
    .global _start
	
	.extern mcf52235_boot_init
	.extern uTaskerBoot	

__interrupt_vector:
	.long __stack 				/* Initial stack pointer */
	.long __GNUStart	        /* reset vector          */
	
_start: 
__GNUStart:	

	move.w	#0x2700,%sr

    /* Initialize RAMBAR1: locate SRAM and validate it */
	move.l	#__SRAM,%d0
    add.l   #0x21,%d0
/*  movec   %d0,RAMBAR1 */
    movec   %d0,#0x0c05

	/* Locate Stack Pointer */
	move.l	#__SP_INIT,%sp

    /* Initialize IPSBAR */
	move.l	#__IPSBAR,%d0
    add.l   #0x1,%d0
	move.l	%d0,0x40000000
	
    /* Initialize FLASHBAR */
    move.l  #__FLASH,%d0
/*  add.l   #0x21,d0   */                       /* No workaround 1 */
    add.l   #0x61,%d0                           /* This is the workaround 1 due to the Internal FLASH Speculation error in the first devices */
    /*movec   %d0,RAMBAR0*/
    movec   %d0,#0x0c04

	jsr		mcf52235_boot_init                  /* Initialize the system */

	jsr		uTaskerBoot	                        /* Jump to the main process */
	nop
	halt

mcf5xxx_wr_vbr:
    move.l  4(%SP),%D0
    .long   0x4e7b0801                          /* movec d0,VBR */
    nop
    rts	

asm_int_off:

    link    %A6,#-4
    movem.l %d7,(%SP)
    move.w  %SR,%D7      
    ori.l    #0x0700,%d7                        /* set interrupt mask  */ 
    move.w  %D7,%SR    
    movem.l (%SP),%D7 
    lea     4(%SP),%SP        
    unlk    %A6  
    rts
    
asm_int_on:
    link    %A6,#-4
    movem.l %d7,(%SP)
    move.w  %SR,%D7       
    andi.l   #0xf8ff,%D7                        /* enable interrupts  */ 
    move.w  %D7,%SR    
    movem.l (%SP),%D7
    lea     4(%SP),%SP        
    unlk    %A6   
    rts  

byte_rev:                                       /* reverse the bytes in a long word */
    move.l  4(%SP),%D0
    byterev.l %D0 
    rts	

start_application:	
    move.l  %D0,%A0                             /* load SP and the PC and let application continue... */
    move.l  (%A0),%sp
    addq.l   #4,%A0
    move.l  (%A0),%A0   
    jmp (%A0)                                   /* jump to the application */

/* 0x98..fill out with zeros - add general purpose assember in this space if required */
    .rept 256-45
    .long 0x00000000			/* fill unused area */
    .endr
/* 0x400..0x417 is used to configure FLASH controller on initialisation */

	.rept 265-257
	.long 0x00000000			/* flash initialisation range - must be zero */
	.endr

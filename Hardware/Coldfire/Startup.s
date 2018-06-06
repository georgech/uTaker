/*
 * File:	Startup.s
 * Purpose:	Start up MCF5223X and low level assember routines
 *          04.09.2007 Fix FLASHBAR value rather than taking from linker script, with errors {1}
 *          27.12.2007 Modify mcf5xxx_wr_vbr to support parameter passing in register        {2} 
 *          21.01.2009 Flash speculation error controlled conditionally                      {3}  
 *          21.03.2009 Add byte reverse routine                                              {4}
 *          01.05.2009 Add start_application                                                 {5}
 *          26.05.2010 Add M5208 configuration                                               {6}
 *          21.06.2011 Add M523x configuration                                               {7}
 *          16.02.2015 Initialise a5 with __SDA_BASE since this could be needed by some library functions (eg. log()) {8}
 */

    .global VECTOR_TABLE
    .global _VECTOR_TABLE
    .global start
    .global asm_int_off
    .global _asm_int_off
    .global asm_int_on
    .global _asm_int_on
	.global start_application
	.global _start_application    
    .global mcf5xxx_wr_vbr
    .global _mcf5xxx_wr_vbr    	
    .global byte_rev                                                     // {4}
    .global _byte_rev
	
#define _ASSEMBLER_CONFIG                                                // {3} allow main configuration control flash speculation setting
#define _M5223X
#include "config.h"
	
#define mcf52235_init   _mcf52235_init
#define main	    	_main
#define __IPSBAR        ___IPSBAR   
#define __SRAM          ___SRAM     
#define __FLASH         ___FLASH    
#define __SP_INIT       ___SP_INIT  
 	
    .extern ___PC_INIT
    .extern __IPSBAR
    .extern __SRAM
    .extern __FLASH
    .extern __SP_INIT
    .extern mcf52235_init
    .extern main		

    .text

/*
 * Exception Vector Table
 */
VECTOR_TABLE:
_VECTOR_TABLE:
INITSP: .long       ___SP_INIT                                           // initial SP
INITPC: .long       ___PC_INIT                                           // initial PC

start:
    move.w  #0x2700,sr                                                   // set initial status register content
#if !defined _M520X                                                      // {6}
    move.l  #__SRAM,d0                                                   // initialize RAMBAR1: locate SRAM and validate it
    add.l   #0x21,d0
    movec   d0,RAMBAR1
#endif
#if defined _EWL_FLOATING_POINT_LIBRARY
    lea __SDA_BASE,a5                                                    // {8}
#endif
    move.l  #__SP_INIT,sp 	                                             // locate Stack Pointer
#if !defined _M520X                                                      // {6}
    move.l  #__IPSBAR,d0                                                 // initialize IPSBAR
    add.l   #0x1,d0
    move.l  d0,0x40000000
	#if !defined _M523X                                                  // {7}
                                                                         // initialize FLASHBAR
//  move.l  #__FLASH,d0                                                     don't take from linker script because it is incorrect {1}
    #if defined FLASH_SPECULATION_ERROR_SOLVED                           // {3} M5225X family have errata corrected
    move.l  #0x121,d0                                                    // no workaround 1 {1} set value assuming FLASH is at low address
    #else
    move.l  #0x161,d0                                                    // this is the workaround 1 due to the Internal FLASH Speculation error in the first devices
    #endif
    movec   d0,RAMBAR0
	#endif
#endif
    jsr     mcf52235_init                                                // initialize the system
    jsr     main                                                         // jump to the main process
    	
mcf5xxx_wr_vbr:                                                          // sub-routine for access to VBR
_mcf5xxx_wr_vbr:
//  move.l  4(SP),D0                                                        {2} remove this when working with parameters passed in registers
   .long    0x4e7b0801                                                   // assembler code for movec d0,VBR
    nop
    rts	

asm_int_off:                                                             // sub routine for disabling interrupts
_asm_int_off:
    link    A6,#-4
    movem.l d7,(SP)
    move.w  SR,D7      
    ori.l   #0x0700,d7                                                   // set interrupt mask to disable all interrupts (apart from NMI)
    move.w  D7,SR    
    movem.l (SP),D7
    lea     4(SP),SP        
    unlk    A6       
    rts
    
asm_int_on:                                                              // sub routine for enabling interrupts
_asm_int_on:
    link    A6,#-4
    movem.l d7,(SP)
    move.w  SR,D7       
    andi.l   #0xf8ff,d7                                                  // enable all interrupts
    move.w  D7,SR    
    movem.l (SP),D7
    lea     4(SP),SP        
    unlk    A6       
    rts  

byte_rev:                                                                // {4} sub routine for reversing bytes in a long word
_byte_rev:
    byterev.l d0
    rts
    
start_application:
_start_application:	
    move.l  d0,a0                                                        // load SP and the PC and let application continue...
    move.l  (a0),sp
    addq.l   #4,a0
    move.l  (a0),a0   
    jmp (a0)                                                             // jump to the application
    

	.end



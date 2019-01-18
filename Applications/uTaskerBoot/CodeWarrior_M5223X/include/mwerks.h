/*
 * File:    mwerks.h
 * Purpose: Define constants used by CodeWarrior toolchain
 *
 * Notes:   
 *
 */

#ifndef _MWERKS_H_
#define _MWERKS_H_

/********************************************************************/

/* 
 * The source uses __interrupt__ to identify a function as
 * an interrupt or exception handler.  Codewarrior uses 
 * __declspec(interrupt), so we are appeasing it like this.
 */
#define __interrupt__   __declspec(interrupt)

/* 
 * Force functions to return values in D0 
 */
#pragma pointers_in_D0

/* 
 * Provide a few assembly instructions for C level routines
 */
//#define halt()      asm( halt)
//#define nop()       asm( nop)
#define tpf()       asm( tpf)
#define stop_2700() asm( stop #0x2700)
#define stop_2600() asm( stop #0x2600)
#define stop_2500() asm( stop #0x2500)
#define stop_2400() asm( stop #0x2400)
#define stop_2300() asm( stop #0x2300)
#define stop_2200() asm( stop #0x2200)
#define stop_2100() asm( stop #0x2100)
#define stop_2000() asm( stop #0x2000)

/********************************************************************/

#endif /* _MWERKS_H_ */

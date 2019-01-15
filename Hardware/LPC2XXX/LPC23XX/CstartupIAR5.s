;**********************************************************************
;  Mark Butcher    Bsc (Hons) MPhil MIET
;
;  M.J.Butcher Consulting
;  Birchstrasse 20f,    CH-5406, Rütihof
;  Switzerland
;
;  www.uTasker.com    Skype: M_J_Butcher
;
;  ---------------------------------------------------------------------
;  File:        CstartupIAR5.s
;  Project:     Single Chip Embedded Internet
;  ---------------------------------------------------------------------
;  Copyright (C) M.J.Butcher Consulting 2004..2013
;  *********************************************************************
;  Start up file for IAR5 on LPC23/4XX
;  15.01.2011 Added __vector to allow IAR5/6 to calculate the vector checksum
;


#define ARM_MODE_ABT     0x17
#define ARM_MODE_FIQ     0x11
#define ARM_MODE_IRQ     0x12
#define ARM_MODE_SVC     0x13
#define ARM_MODE_SYS     0x1F
#define ARM_MODE_UND     0x1B

#define I_BIT            0x80
#define F_BIT            0x40

#define VIC_INT_ENABLE  0xFFFFF014

        MODULE  ?cstartup

        SECTION FIQ_STACK:DATA:NOROOT(2)
        SECTION IRQ_STACK:DATA:NOROOT(2)
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetVector
        PUBLIC  __vector

        ARM
__vector:
resetVector:
        LDR     pc, =reset               ; Reset
        LDR     pc, Undefined_Addr       ; Undefined instructions
        LDR     pc, SWI_Addr             ; Software interrupt (SWI/SVC)
        LDR     pc, Prefetch_Addr        ; Prefetch abort
        LDR     pc, Abort_Addr           ; Data abort
        B       .                        ; RESERVED
        LDR     pc, [pc, #-0x120]        ; Vector from VicVectAddr
        LDR     pc, FIQ_Addr             ; FIQ




Undefined_Addr: DCD   Undefined_Handler
SWI_Addr:       DCD   SWI_Handler
Prefetch_Addr:  DCD   Prefetch_Handler
Abort_Addr:     DCD   Abort_Handler
FIQ_Addr:       DCD   fiqHandler

Undefined_Handler b Undefined_Handler
SWI_Handler       b SWI_Handler
Prefetch_Handler  b Prefetch_Handler
Abort_Handler     b Abort_Handler
fiqHandler        b fiqHandler
/*
   After a reset, execution starts here, the mode is ARM, supervisor
   with interrupts disabled.
   Initializes the chip and branches to the main() function.
 */
        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetHandler
        EXTERN  _LowLevelInit
        EXTERN  ?main
        REQUIRE resetVector
        ARM

resetHandler:
reset:
        ldr   r0,=VIC_INT_ENABLE                           ; disable all interrupts
        mov   r1,#0xFFFFFFFF
        str   r1,[r0]

        /* Set up the fast interrupt stack pointer. */
        MSR     cpsr_c, #ARM_MODE_FIQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(FIQ_STACK)

        /* Set up the interrupt stack pointer. */
        MSR     cpsr_c, #ARM_MODE_IRQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)

        /* Set up the abort stack pointer - share IRQ stack since this must never occur */
        MSR     cpsr_c, #ARM_MODE_ABT | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)

        /* Set up the undefined stack pointer - share IRQ stack since this must never occur */
        MSR     cpsr_c, #ARM_MODE_UND | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)

        /* Set up the SVC stack pointer. */
        MSR     cpsr_c, #ARM_MODE_SVC | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(CSTACK)

        LDR     r0, =_LowLevelInit
        MOV     lr, pc
        BX      r0

        /* Branch to main() and never return*/
        LDR     r0, =?main
        MOV     lr, pc
        BX      r0
        END


/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        CstartupIAR5_LPC21XX.s [NXP LPC21XX]
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2013
   *********************************************************************
   Start up file for IAR5 on LPC21XX
*/

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
        LDR     PC, [PC, #-0x0FF0]       ; Vector from VicVectAddr
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
        
        
        /*
MODE_BITS	DEFINE	0x1F		; Bit mask for mode bits in CPSR
USR_MODE	DEFINE	0x10		; User mode
FIQ_MODE	DEFINE	0x11		; Fast Interrupt Request mode
IRQ_MODE	DEFINE	0x12		; Interrupt Request mode
SVC_MODE	DEFINE	0x13		; Supervisor mode
ABT_MODE	DEFINE	0x17		; Abort mode
UND_MODE	DEFINE	0x1B		; Undefined Instruction mode
SYS_MODE	DEFINE	0x1F		; System mode

I_BIT           DEFINE  0x01
F_BIT           DEFINE  0x02


; Reset entry point
		MODULE	?RESET
		COMMON	INTVEC:CODE:NOROOT(2)
		PUBLIC  __program_start
		EXTERN	?cstartup
                CODE32	; Always ARM mode after reset
		ORG	0x00            ; reset starts at address 0
__program_start
		ldr	pc,[pc,#24]	; Branch to program start
		org	0x04
		ldr	pc,[pc,#24]	; Branch to undef_handler
		org	0x08
		ldr	pc,[pc,#24]	; Branch to swi_handler
		org	0x0c
		ldr	pc,[pc,#24]	; Branch to prefetch_handler
		org	0x10
		ldr	pc,[pc,#24]	; Branch to data_handler
        org	0x18
        LDR     PC, [PC, #-0x0FF0] ; Vector from VicVectAddr
		org	0x1c
		ldr	pc,[pc,#24]	; Branch to fiq_handler

		org	0x20
                dc32	?cstartup       ; reset entry point
		org	0x24
                dc32	undef_handler   ; undefined interrupt handler
		org	0x28
                dc32	swi_handler     ; software interrupt handler
		org	0x2c
                dc32	prefetch_handler; prefetch abort handler
		org	0x30
                dc32	data_handler    ; data abort handler
                org	0x3c
                dc32	fiq_handler     ; fast interrupt handler

undef_handler    b	undef_handler
swi_handler      b      swi_handler
prefetch_handler b      prefetch_handler
data_handler     b      data_handler
fiq_handler      b      fiq_handler

                LTORG
;		ENDMOD	__program_start
                ENDMOD


		MODULE	?CSTARTUP
		RSEG	IRQ_STACK:DATA(2)		
		RSEG	ABT_STACK:DATA:NOROOT(2)
		RSEG	UND_STACK:DATA:NOROOT(2)
		RSEG	FIQ_STACK:DATA:NOROOT(2)
		RSEG	SVC_STACK:DATA:NOROOT(2)
		RSEG	CSTACK:DATA(2)
		RSEG	ICODE:CODE:NOROOT(2)

		EXTERN	_LowLevelInit
		PUBLIC	?cstartup
		EXTERN	main
		CODE32                                      ; After a reset, the mode is ARM, Supervisor, interrupts disabled.
?cstartup                                                   ; start up code
                                                            ; Initialize the stack pointers.
                mrs     r0,cpsr                             ; Original PSR value
                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#IRQ_MODE                     ; Set IRQ mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(IRQ_STACK) & 0xFFFFFFF8     ; End of IRQ_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#ABT_MODE                     ; Set Abort mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(ABT_STACK) & 0xFFFFFFF8     ; End of ABT_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#SVC_MODE                     ; Set Supervisor mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(SVC_STACK) & 0xFFFFFFF8     ; End of SVC_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#UND_MODE                     ; Set Undefined mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(UND_STACK) & 0xFFFFFFF8     ; End of FIR_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#FIQ_MODE                     ; Set FIQ mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(FIQ_STACK) & 0xFFFFFFF8     ; End of FIQ_STACK

                bic     r0,r0,#MODE_BITS                    ; Clear the mode bits
                orr     r0,r0,#SYS_MODE                     ; Set System mode bits
                msr     cpsr_c,r0                           ; Change the mode
                ldr     sp,=SFE(CSTACK) & 0xFFFFFFF8        ; End of CSTACK

	        ldr	r0,=_LowLevelInit
                mov     lr, pc
	        bx	    r0                                      ; call initialisation routine

                ldr     r0,=main
                bx      r0                                  ; go to main program
                LTORG
                ENDMOD
                END

*/
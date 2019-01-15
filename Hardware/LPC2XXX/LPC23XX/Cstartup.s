/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        Cstartup.s [NXP LPC23XX]
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2013
   *********************************************************************

*/

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

VECTOR_START    DEFINE  0x0


; Reset entry point
		MODULE	?RESET
		COMMON	INTVEC:CODE:NOROOT(2)
		PUBLIC  __program_start
		EXTERN	?cstartup
                CODE32	; Always ARM mode after reset
		ORG	VECTOR_START    ; reset starts at this address
__program_start
		ldr	pc,[pc,#24]	; Branch to program start
		org	(VECTOR_START + 0x04)
		ldr	pc,[pc,#24]	; Branch to undef_handler
		org	(VECTOR_START + 0x08)
		ldr	pc,[pc,#24]	; Branch to swi_handler
		org	(VECTOR_START + 0x0c)
		ldr	pc,[pc,#24]	; Branch to prefetch_handler
		org	(VECTOR_START + 0x10)
		ldr	pc,[pc,#24]	; Branch to data_handler
		org	(VECTOR_START + 0x18)
        LDR     PC, [PC, #-0x0120]      ; Vector from VicVectAddr
		org	(VECTOR_START + 0x1c)
		ldr	pc,[pc,#24]	; Branch to fiq_handler

		org	(VECTOR_START + 0x20)
                dc32	?cstartup       ; reset entry point
		org	(VECTOR_START + 0x24)
                dc32	undef_handler   ; undefined interrupt handler
		org	(VECTOR_START + 0x28)
                dc32	swi_handler     ; software interrupt handler
		org	(VECTOR_START + 0x2c)
                dc32	prefetch_handler; prefetch abort handler
		org	(VECTOR_START + 0x30)
                dc32	data_handler    ; data abort handler
		org	(VECTOR_START + 0x3c)
                dc32	fiq_handler     ; fast interrupt handler

undef_handler    b	undef_handler
swi_handler      b      swi_handler
prefetch_handler b      prefetch_handler
data_handler     b      data_handler
fiq_handler      b      fiq_handler

                LTORG
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
	        bx	r0                                          ; call initialisation routine

                ldr     r0,=main
                bx      r0                                  ; go to main program
                LTORG
                ENDMOD
                END


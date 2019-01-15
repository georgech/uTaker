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
;  File:        cstartup_IAR5.s
;  Project:     Single Chip Embedded Internet
;  ---------------------------------------------------------------------
;  Copyright (C) M.J.Butcher Consulting 2004..2010
;  *********************************************************************
;  Start up file for IAR5 on SAM7X
;

#define ARM_MODE_ABT     0x17
#define ARM_MODE_FIQ     0x11
#define ARM_MODE_IRQ     0x12
#define ARM_MODE_SVC     0x13
#define ARM_MODE_SYS     0x1F

#define I_BIT            0x80
#define F_BIT            0x40

#define AT91C_BASE_AIC   0xFFFFF000      ; AIC Base Address

#define AIC_IVR          256
#define AIC_FVR          260
#define AIC_EOICR        304

        MODULE  ?cstartup

        SECTION FIQ_STACK:DATA:NOROOT(2)
        SECTION IRQ_STACK:DATA:NOROOT(2)
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetVector

        ARM
resetVector:
        LDR     pc, =reset               ; Reset
        LDR     pc, Undefined_Addr       ; Undefined instructions
        LDR     pc, SWI_Addr             ; Software interrupt (SWI/SVC)
        LDR     pc, Prefetch_Addr        ; Prefetch abort
        LDR     pc, Abort_Addr           ; Data abort
        B       .                        ; RESERVED
        LDR     pc, =irqHandler          ; IRQ
fiqHandler:
/* Switch in SVC/User Mode to allow User Stack access for C code
   because the FIQ is not yet acknowledged */
/* Save and r0 in FIQ_Register */
            mov         r9,r0
	        ldr         r0 , [r8, #AIC_FVR]
            msr         CPSR_c,#I_BIT | F_BIT | ARM_MODE_SVC

/* Save scratch/used registers and LR in User Stack */
            stmfd       sp!, { r1-r3, r12, lr}

/* Branch to the routine pointed by the AIC_FVR */
            mov         r14, pc
            bx          r0

/* Restore scratch/used registers and LR from User Stack */
            ldmia       sp!, { r1-r3, r12, lr}

/* Leave Interrupts disabled and switch back in FIQ mode */
            msr         CPSR_c, #I_BIT | F_BIT | ARM_MODE_FIQ

/* Restore the R0 ARM_MODE_SVC register */
            mov         r0,r9

/* Restore the Program Counter using the LR_fiq directly in the PC */
            subs        pc,lr,#4

Undefined_Addr: DCD   Undefined_Handler
SWI_Addr:       DCD   SWI_Handler
Prefetch_Addr:  DCD   Prefetch_Handler
Abort_Addr:     DCD   Abort_Handler

Undefined_Handler b Undefined_Handler
SWI_Handler       b SWI_Handler
Prefetch_Handler  b Prefetch_Handler
Abort_Handler     b Abort_Handler

/*
   After a reset, execution starts here, the mode is ARM, supervisor
   with interrupts disabled.
   Initializes the chip and branches to the main() function.
 */
        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetHandler
        EXTERN  AT91F_LowLevelInit
        EXTERN  ?main
        REQUIRE resetVector
        ARM

resetHandler:
reset:
        /* Set up the fast interrupt stack pointer. */
        MSR     cpsr_c, #ARM_MODE_FIQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(FIQ_STACK)

        /* Set up the interrupt stack pointer. */
        MSR     cpsr_c, #ARM_MODE_IRQ | I_BIT | F_BIT      ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)

        /* Set up the SVC stack pointer. */
        MSR     cpsr_c, #ARM_MODE_SVC                      ; Change the mode with enabled interrupts
        LDR     sp, =SFE(CSTACK)

	    LDR     r0, =AT91F_LowLevelInit
        MOV     lr, pc
        BX      r0

        /* Branch to main() and never return*/
        LDR     r0, =?main
        MOV     lr, pc
        BX      r0


/*
   Handles incoming interrupt requests by branching to the corresponding
   handler, as defined in the AIC. Supports interrupt nesting.
 */
irqHandler:
        /* Save interrupt context on the stack to allow nesting */
        SUB     lr, lr, #4
        STMFD   sp!, {lr}
        MRS     lr, SPSR
        STMFD   sp!, {r0, lr}

        /* Write in the IVR to support Protect Mode */
        LDR     lr, =AT91C_BASE_AIC
        LDR     r0, [r14, #AIC_IVR]
        STR     lr, [r14, #AIC_IVR]

        /* Branch to interrupt handler in Supervisor mode */
        MSR     CPSR_c, #ARM_MODE_SVC | I_BIT
        STMFD   sp!, {r1-r3, r12, lr}
        MOV     lr, pc
        BX      r0
        LDMIA   sp!, {r1-r3, r12, lr}
        MSR     CPSR_c, #ARM_MODE_IRQ | I_BIT

        /* Acknowledge interrupt */
        LDR     lr, =AT91C_BASE_AIC
        STR     lr, [r14, #AIC_EOICR]

        /* Restore interrupt context and branch back to calling code */
        LDMIA   sp!, {r0, lr}
        MSR     SPSR_cxsf, lr
        LDMIA   sp!, {pc}^
        END




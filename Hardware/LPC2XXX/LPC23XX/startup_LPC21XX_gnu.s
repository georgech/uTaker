/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        startup_LPC21XX_gnu.s [NXP LPC21XX]
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2013
   *********************************************************************

*/


 
/* The vector table is fixed in FLASH so that the processor can boot to the reset startup code */
    .section .vectors, "ax"
    .code 32
    .align 0
    .global _vectors
    .global reset_code
    .global end
    .extern _LowLevelInit
    .extern main 

/* reset vector followed by exception vectors */
_vectors:
    ldr pc, [pc, #_reset_handler - . - 8]                                /* reset vector             */
    ldr pc, [pc, #_undef_handler - . - 8]                                /* undefined instruction    */
    ldr pc, [pc, #_swi_handler - . - 8]                                  /* swi handler              */
    ldr pc, [pc, #_pabort_handler - . - 8]                               /* abort prefetch           */
    ldr pc, [pc, #_dabort_handler - . - 8]                               /* abort data               */
    .word 0xB8A06F88                                                     /* LPC boot loader checksum */
    ldr pc, [pc, #_irq_handler - . - 8]                                  /* irq                      */
    ldr pc, [pc, #_fiq_handler - . - 8]                                  /* fiq                      */


_reset_handler:
    .word reset_code
  
_irq_handler:
    .word irq_handler

_undef_handler:
_swi_handler:
_pabort_handler:
_dabort_handler:
_fiq_handler:
    .word default_code



/* defines
ARM_MODE_FIQ            EQU     0x11
ARM_MODE_IRQ            EQU     0x12
ARM_MODE_SVC            EQU     0x13

I_BIT                   EQU     0x80
F_BIT                   EQU     0x40                                     */



    .section .init, "ax"
    .code 32
    .align 0
/* Reset code - this is started as first code after a reset */
reset_code:
    mrs r0, cpsr                                                         /* Read the status register */
    bic r0, r0, #0x1f                                                    /* Mask out the mode bits */
    orr r1, r0, #0x1b                                                    /* prepare undefined mode */
    msr     cpsr_cxsf, r1                                                /* set mode */
    ldr     sp, = __stack_und_end__                                      /* set undefined stack pointer */
    orr r1, r0, #0x17                                                    /* prepare abort mode */
    msr cpsr_cxsf, r1                                                    /* set mode */
    ldr sp, =__stack_abt_end__                                           /* set abort stack pointer */
    orr r1, r0, #0x12                                                    /* prepare IRQ mode */
    msr cpsr_cxsf, r1                                                    /* set mode */
    ldr sp, =__stack_irq_end__                                           /* set IRQ stack pointer */
    orr r1, r0, #0x11                                                    /* prepare FIQ mode */
    msr cpsr_cxsf, r1                                                    /* set mode */
    ldr sp, =__stack_fiq_end__                                           /* set FIQ stack pointer */
    orr r1, r0, #0x13                                                    /* prepare Supervisor mode */
    msr cpsr_cxsf, r1                                                    /* set mode */
    ldr sp, =__stack_svc_end__                                           /* set Supervisor stack pointer */
    orr r1, r0, #0x1f                                                    /* prepare System mode */
    msr cpsr_cxsf, r1                                                    /* set mode */
    ldr sp, =__stack_end__                                               /* set System stack pointer */
    ldr	    r0,= _LowLevelInit                                           /* call C code to initialise variables */
    mov     lr, pc
    bx	    r0

    ldr	r0,=main                                                         /* jump to main */
    bx	r0

    .code 32
/* disable all interrupts (this assumes processor originally running in Thumb mode) */
/* switches to ARM mode and uses R12 (assumed to be free) */
    .global __irq_dis
__irq_dis:
    /*bx pc*/                                                            /* switch to ARM mode        */
    /*nop
    .code 32*/
_retry:
    mrs r12, CPSR                                                        /* get present SR value      */
    orr r12, r12, #0xc0                                                  /* set IRQ and FIQ mask bits */
    msr CPSR_c, r12                                                      /* set new value to SR       */
    mrs r12, CPSR                                                        /* get new SR value          */
    ands r12, r12, #0xc0                                                 /* mask the IRQ bits         */
    beq _retry                                                           /* loop if not set (can happen if there is an interrupt in last machine cycle) */
    bx lr                                                                /* return with interrupts disabled */


/* enable interrupts all - (this assumes processor originally running in Thumb mode) */
    .code 32
    .global __irq_en
__irq_en:
    /*bx pc*/                                                            /* switch to ARM mode        */
    /*nop
  .code 32*/
    mrs r12, CPSR                                                        /* get present SR value      */
    bic r12, r12, #0xC0                                                  /* remove IRQ and FIQ mask bits */
    msr CPSR_c, r12                                                      /* set new value to SR       */
    bx lr                                                                /* return with interrupts enabled */

  .code 32
  .global irq_handler

irq_handler:
  stmfd sp!, {r0}
  mrs r0, spsr
  tst r0, #0x80                                                          /* check if I bit is set in SPSR to detect spurious interrupts */
  ldmfd sp!, {r0}
  subnes pc, lr, #4
  stmfd sp!, {r0-r4, r12, lr}                                            /* save context registers */
  ldr r4, =0xFFFFF030                                                    /* VIC Address Block - LPC21XX */
  ldr r3, [r4]                                                           /* load the value from the interrupt vector register */
  mov lr, pc
  bx r3                                                                  /* jump to the interrupt handler */
  mov r3, #0
  str r3, [r4]                                                           /* acknowledge the interrupt */
  ldmfd sp!, {r0-r4, r12, lr}                                            /* return interrupted context registers */
  subs pc, lr, #4                                                        /* return */


default_code:                                                            /* default exception vectors */
end:
  b .                                                                    /* endless loop */


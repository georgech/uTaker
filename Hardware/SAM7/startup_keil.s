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
;  File:        startup_keil.s [sam7x]
;  Project:     Single Chip Embedded Internet
;  ---------------------------------------------------------------------
;  Copyright (C) M.J.Butcher Consulting 2004..2011
;  *********************************************************************
;  Start up file for sam7x with Keil
;  including IRQ and FIQ dispatchers
;  05.06.2011 added EXPORT  Vectors to allow control of location with uVision4
;  05.06.2011 reserve 4 bytes at top of RAM for random number
;

PROGRAM_RAM_START EQU   0x200000
PROGRAM_RAM_SIZE  EQU   (0x10000 - 4)                                    ; 64k program RAM available in SAM7X256

; Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17		  
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80            ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40            ; when F bit is set, FIQ is disabled

AT91C_BASE_AIC  EQU     0xFFFFF000      ; AIC Base Address
AIC_IVR         EQU     256             ; IRQ Vector Register
AIC_FVR         EQU     260             ; FIQ Vector Register
AIC_EOICR       EQU     304             ; end of Interrupt Command Register

UND_Stack_Size  EQU     0x00000080
ABT_Stack_Size  EQU     0x00000080
FIQ_Stack_Size  EQU     0x00000100
IRQ_Stack_Size  EQU     0x00000100
SVC_Stack_Size  EQU     0x00000100

Stack_Sizes     EQU     UND_Stack_Size + ABT_Stack_Size	+ FIQ_Stack_Size + IRQ_Stack_Size + SVC_Stack_Size

__stack_und_end__  EQU  PROGRAM_RAM_START + PROGRAM_RAM_SIZE

                PRESERVE8

; Area Definition and Entry Point
; Startup Code must be linked first at Address at which it expects to run.

                AREA    RESET, CODE, READONLY
                ARM
                EXPORT  Vectors
; Exception Vectors
; Mapped to Address 0.
; Absolute addressing mode must be used.
; Dummy Handlers are implemented as infinite loops which can be modified.
Vectors         LDR     PC, Reset_Addr         
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                                                      ; reserved Vector
                LDR     PC, IRQ_Addr									 ; jump to IRQ handler
FIQ_Handler_Entry                                                        ; FIQ hander
                mov     r9, r0                                           ; save r0 on FIQ stack
	            ldr     r0 , [r8, #AIC_FVR]                              ; get the FIQ interrupt 
                msr     CPSR_c,#I_Bit | F_Bit | Mode_SVC                 ; disable interrupts and switch to supervisor mode
                stmfd   sp!, { r1-r3, r12, lr}                           ; save user registers on user stack
                mov     r14, pc                                          ; branch to handling routine
                bx      r0

                ldmia   sp!, { r1-r3, r12, lr}                           ; restore registers from user stack
                msr     CPSR_c, #I_Bit | F_Bit | Mode_FIQ                ; switch back to FIQ mode
                mov     r0, r9                                           ; restore r0
                subs    pc, lr, #4                                       ; restore the PC and continue interrupted program operation

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handlerr
                DCD     0                      ; Reserved Address 
IRQ_Addr        DCD     IRQ_Handler


Undef_Handler   b       Undef_Handler
SWI_Handler     b       SWI_Handler
PAbt_Handler    b       PAbt_Handler
DAbt_Handlerr   b       DAbt_Handlerr
FIQ_Handler     b       FIQ_Handler

; Reset Handler
                EXPORT  Reset_Handler
Reset_Handler   

; Setup Stack for each mode
                LDR     R0, =__stack_und_end__

; Undefined Instruction Stack Pointer at top of program RAM
                MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

;  Set Abort Mode Stack Pointer
                MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

;  Set FIQ Mode  Stack Pointer
                MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

;  Set IRQ Mode Stack Pointer
                MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

;  Set Supervisor Mode Stack Pointer
                MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

;  Enter SYS Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
				
; Continue operation in SYS mode				                 
                IMPORT AT91F_LowLevelInit
                BL AT91F_LowLevelInit

; Enter the C code and never returns
                IMPORT  __main
                LDR     R0, =__main
                BX      R0

USR_Stack_Size  EQU     0x00000004

		        EXPORT  Stack_Mem
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   USR_Stack_Size

Heap_Size       EQU     0x00000004

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
Heap_Mem        SPACE   Heap_Size
; User Initial Stack & Heap
                AREA    |.text|, CODE, READONLY

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap			                                     ; this is called by the Keil initialisation routine

                LDR     R0, =  Heap_Mem	                                 ; inform where the heap memory is located
               ;LDR     R1, =(Stack_Mem + USR_Stack_Size)				 ; this is used by Keil to set the SP
			    LDR     R1, = __stack_und_end__							 ; we don't use a specified stack size but rather leave the SP at its original location
				SUB     R1, R1, #Stack_Sizes                             ; SP below the IRQ stacks
                LDR     R2, = (Heap_Mem + Heap_Size)					 ; we don't generally use library heap but the heap size can be increase if required
                LDR     R3, = Stack_Mem									 ; this is the location of the last system variable (initialised, followed by bss)
                BX      LR

IRQ_Handler
                sub     lr, lr, #4                                       ; adjust and save lr on IRQ stack
                stmfd   sp!, {lr}
                stmfd   sp!, {r0}                                        ; save r0 on IRQ stack

                ldr     r14, =AT91C_BASE_AIC
	            ldr     r0 , [r14, #AIC_IVR]                             ; get the interrupt vector
	            str     r14, [r14, #AIC_IVR]							 ; clear the interrupt source
                msr     CPSR_c, #I_Bit | Mode_SVC                        ; switch to supervisor mode
                stmfd   sp!, { r1-r3, r12, r14}                          ; save user registers on user stack
                mov     r14, pc                                          ; call IRQ handling routine
                bx      r0

                ldmia   sp!, { r1-r3, r12, r14}                          ; restore user registers
                msr     CPSR_c, #I_Bit | Mode_IRQ                        ; move back to IRQ mode
                ldr     r14, =AT91C_BASE_AIC                             ; mark the end of the interrupt
                str     r14, [r14, #AIC_EOICR]
                ldmia   sp!, {r0}                                        ; restore r0
                ldmia   sp!, {pc}^                                       ; restore adjusted lr and return for interrupted program code
                END




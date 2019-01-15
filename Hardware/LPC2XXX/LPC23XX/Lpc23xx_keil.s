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
;  File:        LPC23xx_keil.s [NXP]
;  Project:     Single Chip Embedded Internet
;  ---------------------------------------------------------------------
;  Copyright (C) M.J.Butcher Consulting 2004..2013
;  *********************************************************************
;  Start up file for Keil on LPC23xx
;  05.06.2011 added EXPORT  Vectors to allow control of location with uVision4
;

PROGRAM_RAM_START EQU   0x40000000
PROGRAM_RAM_SIZE  EQU   0x8000                                           ; 32k program RAM available in LPC2368

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



IAP_Stack_Size  EQU     0x00000024      ; reserved for IAP use plus 4 bytes for random number
UND_Stack_Size  EQU     0x00000080
ABT_Stack_Size  EQU     0x00000080
FIQ_Stack_Size  EQU     0x00000100
IRQ_Stack_Size  EQU     0x00000100
SVC_Stack_Size  EQU     0x00000100

Stack_Sizes     EQU     UND_Stack_Size + ABT_Stack_Size	+ FIQ_Stack_Size + IRQ_Stack_Size + SVC_Stack_Size

__stack_und_end__  EQU  PROGRAM_RAM_START + PROGRAM_RAM_SIZE - IAP_Stack_Size

                PRESERVE8

; Area Definition and Entry Point
;  Startup Code must be linked first at Address at which it expects to run.

                AREA    RESET, CODE, READONLY
                ARM
                EXPORT  Vectors

; Exception Vectors
;  Mapped to Address 0.
;  Absolute addressing mode must be used.
;  Dummy Handlers are implemented as infinite loops which can be modified.

Vectors         LDR     PC, Reset_Addr         
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                            ; reserved Vector (the loader will set a value to allow the processor to recognise valid code)
                LDR     PC, [PC, #-0x0120]     ; vector from VicVectAddr
                LDR     PC, FIQ_Addr

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handlerr
                DCD     0                      ; Reserved Address 
FIQ_Addr        DCD     FIQ_Handler


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
                IMPORT _LowLevelInit
                BL _LowLevelInit

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
__user_initial_stackheap                                                 ; this is called by the Keil initialisation routine

                LDR     R0, =  Heap_Mem                                  ; inform where the heap memory is located
               ;LDR     R1, =(Stack_Mem + USR_Stack_Size)                ; this is used by Keil to set the SP
                LDR     R1, = __stack_und_end__                          ; we don't use a specified stack size but rather leave the SP at its original location
                SUB     R1, R1, #Stack_Sizes                             ; SP below the IRQ stacks
                LDR     R2, = (Heap_Mem + Heap_Size)                     ; we don't generally use library heap but the heap size can be increase if required
                LDR     R3, = Stack_Mem                                  ; this is the location of the last system variable (initialised, followed by bss)
                BX      LR

                END

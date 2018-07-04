//**********************************************************************
// Mark Butcher    Bsc (Hons) MPhil MIET
//
// M.J.Butcher Consulting
// Birchstrasse 20f,    CH-5406, Rütihof
// Switzerland
//
// www.uTasker.com    Skype: M_J_Butcher
//
// ---------------------------------------------------------------------
// File:        LM3S6XXX_asm.s [Luminary]
// Project:     Single Chip Embedded Internet
// ---------------------------------------------------------------------
// Copyright (C) M.J.Butcher Consulting 2004..2018
// *********************************************************************
//
// This file contains assembler code needed to access some of the processor's registers (IAR)
//
// 25.7.2009 Added start_application()


    rseg CODE:CODE(2)                                                    // put contents to code section

    export __enable_interrupt                                            // global routine to enable processor interrupts
__enable_interrupt:
    cpsie   i
    bx      lr


    export __disable_interrupt                                           // global routine to disable processor interrupts
__disable_interrupt:
    cpsid   i
    bx      lr


    export __sleep_mode                                                  // global routine to set low power state and wait for events to wake
__sleep_mode:
    wfi
    bx      lr

    export start_application                                             // global routine to jump to a new application
start_application:
    ldr sp, [r0,#0]
    ldr pc, [r0,#4]

    end                                                                  // end of file

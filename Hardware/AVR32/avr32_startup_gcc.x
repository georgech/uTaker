
  .section  .init, "ax", @progbits


  .global _start
  .type _start, @function
_start:
  lda.w   sp, __SRAM_segment_end__                                       // set initial stack pointer
  lda.w   pc, _stext

  .section  .text._stext, "ax", @progbits

  .global _stext
  .type _stext, @function
_stext:

  csrf    21                                                             // enable the exception processing
  lda.w pc, AVR32_LowLevelInit


        .global  __set_evba
        .global  __set_count
        .global  __set_compare
        .global  start_application

__set_evba:                                                              // write the passed value to EVBA register
        mtsr    4,r12
        ret 0

__set_count:                                                             // write the passed value to COUNT register
        mtsr    264,r12
        ret 0

__set_compare:                                                           // write the passed value to COMPARE register
        mtsr    268,r12
        ret 0

start_application:
        mov pc,r12                                                       // jump to the defined address (where new application is located)




/*
        lddpc   r0,AVR32_LowLevelInit
        icall   r0

  // Start the show.
  lda.w   pc, main
  // Load initialized data having a global lifetime from the data LMA.
  lda.w   r0, _data
  lda.w   r1, _edata
  cp      r0, r1
  brhs    idata_load_loop_end
  lda.w   r2, _data_lma
idata_load_loop:
  ld.d    r4, r2++
  st.d    r0++, r4
  cp      r0, r1
  brlo    idata_load_loop
idata_load_loop_end:

  // Clear uninitialized data having a global lifetime in the blank static storage section.
  lda.w   r0, __bss_start
  lda.w   r1, _end
  cp      r0, r1
  brhs    udata_clear_loop_end
  mov     r2, 0
  mov     r3, 0
udata_clear_loop:
  st.d    r0++, r2
  cp      r0, r1
  brlo    udata_clear_loop
udata_clear_loop_end:
#ifdef CONFIG_FRAME_POINTER
  // Safety: Set the default "return" @ to the exit routine address.
  lda.w   lr, exit
#endif
*/
OUTPUT_ARCH(m68k)

MEMORY
{
  rom (rx) : ORIGIN = 0, LENGTH = 256K
  vectorram(RWX) : ORIGIN = 512M, LENGTH = 0x00000400
  usbram (rwx) : ORIGIN = 512M + 0x400, LENGTH = 0x80
  ram (rwx) : ORIGIN = 512M + 0x480, LENGTH = 32K - 0x400 - 0x80
  ipsbar  (RWX)  : ORIGIN = 0x40000000, LENGTH = 0x0
}

/* Place the stack at the end of memory, unless specified otherwise. */
PROVIDE (__stack = ORIGIN (ram) + LENGTH (ram));

SECTIONS
{
  .rom	         : {} > rom
  .vectorram	 : {} > vectorram
  .usbram	     : {} > usbram
  .ipsbar	     : {} > ipsbar

  .text :
  {
    *(.interrupt_vector)  
    Build/Startup_gnu.o(.text) 
  } >rom

  .text :
  {
    CREATE_OBJECT_SYMBOLS
    *(.text .text.*)
    *(.gnu.linkonce.t.*)

    . = ALIGN(0x4);
    /* These are for running static constructors and destructors under ELF.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))

    . = ALIGN(0x4);
    KEEP (*crtbegin.o(.jcr))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .jcr))
    KEEP (*crtend.o(.jcr))

    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)

    . = ALIGN(0x4);
    *(.gcc_except_table) 

    . = ALIGN(0x4);
    *(.eh_frame)


    . = ALIGN(0x4);
    __INIT_SECTION__ = . ;
   _init = . ;
    LONG (0x4e560000)   /* linkw %fp,#0 */
    *(.init)
    SHORT (0x4e5e)   /* unlk %fp */
    SHORT (0x4e75)   /* rts */

    . = ALIGN(0x4);
    __FINI_SECTION__ = . ;
    _fini = . ;
    LONG (0x4e560000)   /* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4e5e)   /* unlk %fp */
    SHORT (0x4e75)   /* rts */

    *(.lit)

    . = ALIGN(4);
    __region_array = .;    
    _etext = .;
  } >rom




  .data :
  {
    __data_load = LOADADDR (.data);
    __data_start = .;
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN (4);
    _edata = .;
  } >ram AT>rom

  .bss :
  {
    __bss_start = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _end = .;
    __end = _end;
  } >ram AT>rom

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

    __IPSBAR     = ADDR(.ipsbar);
    __SRAM 	     = ADDR(.vectorram);
    __FLASH      = ADDR(.rom);
    __SP_INIT    = __SRAM + 32K - 4;
    __VECTOR_RAM = __SRAM;
    __USB_BDT_RAM = __SRAM + 0x400;

}


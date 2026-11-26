/* The loader addresses the header and calls its callbacks. It does not
   jump into the header. dc_overlay supplies this image's P0 or P1 base. */
PHDRS
{
  header PT_LOAD FLAGS(6);
  code PT_LOAD FLAGS(5);
  data PT_LOAD FLAGS(6);
}
SECTIONS
{
  .head overlay_base : { KEEP(*(.head)) } :header
  . = ALIGN(0x1000);
  .text : { *(.text .text.* .gnu.linkonce.t.* P PPOSE PSG) } :code
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) } :code
  . = ALIGN(0x1000);
  .data : { *(.data .data.* .sdata .sdata.* .gnu.linkonce.d.*) } :data
  .bss (NOLOAD) : { *(.bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* COMMON) } :data
  .unexpected : { INPUT_SECTION_FLAGS(SHF_ALLOC) *(*) } :data
}
ASSERT(SIZEOF(.unexpected) == 0, "unplaced overlay storage")

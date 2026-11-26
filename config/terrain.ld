/* The overlay loader supplies the base. All addresses in this relocatable
   layout, including the start of free RAM, move with the image. */
SECTIONS
{
  .head : { KEEP(*(.head)) }
  .text : { *(.text .text.* .gnu.linkonce.t.* P) }
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  .data : { *(.data .data.* .sdata .sdata.* .gnu.linkonce.d.*) }
  .bss (NOLOAD) : { *(.bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* COMMON) }
  /* Two Chao gardens have no BSS. Keep their cursor relative to the last
     initialized data so a later link can place the stage at another address. */
  _end = SIZEOF(.bss) ? . : (ADDR(.data) + SIZEOF(.data));
  .unexpected : { INPUT_SECTION_FLAGS(SHF_ALLOC) *(*) }
}
ASSERT(SIZEOF(.head) > 0, "missing stage header")
ASSERT(SIZEOF(.unexpected) == 0, "unplaced stage storage")

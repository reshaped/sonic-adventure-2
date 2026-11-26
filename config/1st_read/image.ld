/* The hardware loads the resident at the start of its library header.
   The first record starts execution and the following handle is writable.
   The loader's decompression workspace begins at 0x8c300000. */
ENTRY(_START_SYS)
PHDRS
{
  header PT_LOAD FLAGS(7);
  code PT_LOAD FLAGS(5);
  data PT_LOAD FLAGS(6);
}
SECTIONS
{
  . = 0x8c010000;
  .resident_header : { KEEP(*(.text.resident_entry)) } :header
  . = ALIGN(4096);
  .text : { *(.text .text.* P PSG) } :code
  .rodata : { *(.rodata .rodata.* C CSG) } :code
  . = ALIGN(4096);
  .data : { *(.data.resident .data.resident_sdk) *(.data .data.*) } :data
  .bss (NOLOAD) : { *(.bss.resident) *(.bss .bss.*) } :data
  .unexpected : { INPUT_SECTION_FLAGS(SHF_ALLOC) *(*) } :data
}
ASSERT(_START_SYS == ADDR(.resident_header) && SIZEOF(.resident_header) == 128,
       "invalid resident startup header")
ASSERT(SIZEOF(.unexpected) == 0, "unplaced resident storage")
ASSERT(ADDR(.bss) + SIZEOF(.bss) <= 0x8c300000,
       "resident overlaps the loader workspace")

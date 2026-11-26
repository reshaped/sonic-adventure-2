/* The stage loader places CHAO at 0x8c500000. CHAO loads its model
   archive at 0x8c698700, motions at 0x8c8ba800 and areas at 0x8c9c0000. */
SECTIONS
{
  /* Consume the external objects before the resident's .bss.* wildcard. */
  INCLUDE build/dc/census/chao-bank-objects.ld
  .head 0x8c500000 : { KEEP(*(.head)) }
  .text : { *(.text .text.* .gnu.linkonce.t.* P PPOSE) }
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  .data : { *(.data .data.* .sdata .sdata.* .gnu.linkonce.d.*) }
  .bss (NOLOAD) : { *(.bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* COMMON) }
  .unexpected : { INPUT_SECTION_FLAGS(SHF_ALLOC) *(*) }
}
ASSERT(SIZEOF(.head) == 24, "missing CHAO stage header")
ASSERT(SIZEOF(.unexpected) == 0, "unplaced CHAO storage")
ASSERT(ADDR(.bss) + SIZEOF(.bss) <= 0x8c698700, "CHAO overlaps its model archive")
ASSERT(ADDR(.chao_models) + SIZEOF(.chao_models) <= ADDR(.chao_motions), "model archive overlaps motions")
ASSERT(ADDR(.chao_motions) + SIZEOF(.chao_motions) <= 0x0c9c0000, "motion archive overlaps the child")

/* chao-placed.o has already laid out the archive members and resident code
   used by chao_area.py. Keep those VMAs while applying final relocations. */
PHDRS
{
  models PT_LOAD FLAGS(6);
  motions PT_LOAD FLAGS(6);
  /* The mutable garden header and first instructions share a page. The
     children already hold these code and storage addresses. */
  resident PT_LOAD FLAGS(7);
}
SECTIONS
{
  .chao_models chao_model_base (NOLOAD) : { *(.chao_models) } :models
  .chao_motions chao_motion_base (NOLOAD) : { *(.chao_motions) } :motions
  .head overlay_base : { KEEP(*(.head)) } :resident
  .text : { *(.text) } :resident
  .rodata : { *(.rodata) } :resident
  .data : { *(.data) } :resident
  .bss (NOLOAD) : { *(.bss) } :resident
  .unexpected : { INPUT_SECTION_FLAGS(SHF_ALLOC) *(*) } :resident
}
ASSERT(SIZEOF(.unexpected) == 0, "unplaced Chao storage")

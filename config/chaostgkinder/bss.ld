/* The overlay clears the game's BSS and the SDK's BSG before it runs the
   network initializers. Keep both ranges relocatable with their storage. */
SECTIONS
{
  .bss.kinder (NOLOAD) :
  {
    _kinder_bss_area = .;
    EXCLUDE_FILE (*chaostgkinder-sdk.o) *(.bss .bss.* .gnu.linkonce.b.* .sbss .sbss.* .gnu.linkonce.sb.* COMMON)
    . = ALIGN(4);
    _kinder_bss_end = .;
    _kinder_sg_bss_area = .;
    *chaostgkinder-sdk.o(.bss .bss.* .gnu.linkonce.b.* .sbss .sbss.* .gnu.linkonce.sb.* COMMON)
    . = ALIGN(4);
    _kinder_sg_bss_end = .;
  }
}
INSERT BEFORE .sbss;

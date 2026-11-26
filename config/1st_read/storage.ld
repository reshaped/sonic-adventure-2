/* Retain the input families before the resident relocatable link combines
   them. The SDK cache objects have no ordinary mutable storage. */
SECTIONS
{
    /* The first instructions and the mutable handle share one loader
       record. strt1's ELF text input supplies its execute permission. */
    .resident.head :
    {
        *strt1.o(.text)
        *(DSGLH)
        *(.data.ovg_libhdl)
    }
    .resident.matrix (NOLOAD) : { *(.bss.matrix_stack) }
    .resident.sdk_data32 :
    {
        *1st_read-cache.o(.data .data.* .sdata .sdata.* .gnu.linkonce.d.* .gnu.linkonce.s.*)
    }
    .resident.sdk_bss32 (NOLOAD) :
    {
        *1st_read-cache.o(.bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* .gnu.linkonce.sb.* COMMON)
    }
    .resident.sdk_data :
    {
        *1st_read-sdk.o(.data .data.* .sdata .sdata.* .gnu.linkonce.d.* .gnu.linkonce.s.*)
    }
    .resident.sdk_bss (NOLOAD) :
    {
        *1st_read-sdk.o(.bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* .gnu.linkonce.sb.* COMMON)
    }
}

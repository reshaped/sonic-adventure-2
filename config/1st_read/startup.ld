/* The image runs from RAM. Its D and DSG data arrive in place and the
   genuine R/R32 and RSG/RSG32 destination pairs have no payload. The
   startup clears each ordinary BSS through the end of its cache BSS. */
SECTIONS
{
    .text.resident_entry : ALIGN(4)
    {
        __startof_DSGLH = .;
        KEEP(*(.resident.head))
        __endof_DSGLH = .;
        __startof_DSGLE = .;
        KEEP(*(DSGLE))
        __endof_DSGLE = .;
    }
    .data.resident : ALIGN(32)
    {
        __startof_D = .;
        *(D .data .data.* .sdata .sdata.* .gnu.linkonce.d.* .gnu.linkonce.s.*)
        . = ALIGN(32);
        __startof_D32 = .;
        *(D32)
        . = ALIGN(4);
        __startof_DSG_INIT_ = .;
        KEEP(*(DSG_INIT_))
        __endof_DSG_INIT_ = .;
        __startof_DSG_END_ = .;
        KEEP(*(DSG_END_))
        __endof_DSG_END_ = .;
        __startof_D_INIT_ = .;
        KEEP(*(D_INIT_))
        __endof_D_INIT_ = .;
        __startof_D_END_ = .;
        KEEP(*(D_END_))
        __endof_D_END_ = .;
        . = ALIGN(32);
        __startof_R = .;
        *(R)
        __endof_R = .;
        . = ALIGN(32);
        __startof_R32 = .;
        *(R32)
        __endof_R32 = .;
    }
    .data.resident_sdk : ALIGN(32)
    {
        __startof_DSG = .;
        *(DSG .resident.sdk_data)
        . = ALIGN(32);
        __startof_DSG32 = .;
        *(DSG32 .resident.sdk_data32)
        . = ALIGN(32);
        __startof_RSG = .;
        *(RSG)
        __endof_RSG = .;
        . = ALIGN(32);
        __startof_RSG32 = .;
        *(RSG32)
        __endof_RSG32 = .;
    }
    .bss.resident (NOLOAD) : ALIGN(32)
    {
        __startof_$G0 = .;
        *($G0)
        . = ALIGN(32);
        *($G1)
        . = ALIGN(4);
        __startof_B = .;
        *(B .bss .bss.* .sbss .sbss.* .gnu.linkonce.b.* .gnu.linkonce.sb.* COMMON)
        . = ALIGN(4);
        __endof_B = .;
        . = ALIGN(32);
        __startof_B32 = .;
        *(B32 .resident.matrix)
        . = ALIGN(4);
        __endof_B32 = .;
        . = ALIGN(4);
        __startof_BSG = .;
        *(BSG .resident.sdk_bss)
        . = ALIGN(4);
        __endof_BSG = .;
        . = ALIGN(32);
        __startof_BSG32 = .;
        *(BSG32 .resident.sdk_bss32)
        __endof_BSG32 = .;
    }
}
ASSERT(_START_SYS == __startof_DSGLH && _ovg_libhdl == _START_SYS + 64 &&
       __endof_DSGLH == _ovg_libhdl + 32 && __endof_DSGLE == __startof_DSGLE + 32,
       "Resident library handle must lie between its startup records")
ASSERT(__startof_D < __startof_D32 && __startof_DSG < __startof_DSG32 &&
       __startof_DSG32 < __startof_RSG, "Resident initialized data missing")
ASSERT(__startof_B < __endof_B && __startof_B32 < __endof_B32 &&
       __startof_BSG < __endof_BSG && __startof_BSG32 < __endof_BSG32,
       "Resident program or SDK BSS missing")
ASSERT(__endof_R32 == __startof_R && __endof_RSG32 == __startof_RSG,
       "Resident initialized data must run in place")

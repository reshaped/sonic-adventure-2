/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* @asm open: SHC's dead RTS follows a tail jump, not the body's own return */
#pragma inline_asm(njRoundUp)
Float njRoundUp(Float n)
{
        .IMPORT     _ceilf
        MOV.L       #_ceilf,R0
        FLDI0       FR0
        FCMP/GT     FR4,FR0
        BF/S        ?positive
        FABS        FR4
        STS.L       PR,@-R15
        JSR         @R0
        NOP
        LDS.L       @R15+,PR
        RTS
        FNEG        FR0
?positive:
        JMP         @R0
        NOP
}
#endif
/* clang-format on */

#include <sg_xpt.h>

/* the member exports this datum, though no header of the SDK declares it.
 * njFraction leaves its argument's integral part here */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Float FracInt;
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* @asm open: SHC's dead RTS follows a tail jump, not the body's own return */
#pragma inline_asm(njFraction)
Float njFraction(Float n)
{
        .IMPORT     _modff
        MOV.L       #_modff,R0
        MOV.L       #_FracInt,R4
        JMP         @R0
        NOP
}
#endif
/* clang-format on */

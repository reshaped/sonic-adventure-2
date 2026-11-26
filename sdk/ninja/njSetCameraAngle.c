/* clang-format off */
#ifdef DREAMCAST
#include <ninja.h>

/* the body stores ang in c's ang, at byte offset 28 */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c125a70 follows the body's own
 * return at 0x8c125a6c */
#pragma inline_asm(njSetCameraAngle)
void njSetCameraAngle(NJS_CAMERA *c, Angle ang)
{
        RTS
        MOV.L       R5,@(28,R4)
}
#endif
/* clang-format on */

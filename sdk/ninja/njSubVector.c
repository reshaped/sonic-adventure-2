#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. shz_vec3_sub subtracts each of vs's
 * components from vd's, as the body's three FSUBs do */
void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    zam_store_vec3(vd, shz_vec3_sub(zam_load_vec3(vd), zam_load_vec3(vs)));
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a160 follows the body's own
 * return at 0x8c11a15a */
#pragma inline_asm(njSubVector)
void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
        FMOV.S      @R5+,FR1
        FMOV.S      @R4+,FR4
        FMOV.S      @R5+,FR2
        FSUB        FR1,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R5,FR3
        FSUB        FR2,FR5
        FMOV.S      @R4,FR6
        FSUB        FR3,FR6
        FMOV.S      FR6,@R4
        FMOV.S      FR5,@-R4
        RTS
        FMOV.S      FR4,@-R4
}
/* clang-format on */
#endif
#endif

#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. shz_vec3_add adds each of vs's components
 * to vd's, as the body's three FADDs do */
void njAddVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
    zam_store_vec3(vd, shz_vec3_add(zam_load_vec3(vs), zam_load_vec3(vd)));
}
#else
/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c118ac0 follows the body's own
 * return at 0x8c118aba */
#pragma inline_asm(njAddVector)
void njAddVector(NJS_VECTOR *vd, NJS_VECTOR *vs)
{
        FMOV.S      @R4+,FR4
        FMOV.S      @R5+,FR1
        FMOV.S      @R4+,FR5
        FADD        FR4,FR1
        FMOV.S      @R5+,FR2
        FMOV.S      @R4,FR6
        FADD        FR5,FR2
        FMOV.S      @R5,FR3
        FADD        FR6,FR3
        FMOV.S      FR3,@R4
        FMOV.S      FR2,@-R4
        RTS
        FMOV.S      FR1,@-R4
}
/* clang-format on */
#endif
#endif

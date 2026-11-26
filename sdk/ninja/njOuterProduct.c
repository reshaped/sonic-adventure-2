#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. each component takes the body's FMUL
 * of one product. the body's FMAC then adds the other product, with one
 * factor negated. the length takes the body's FIPR and FSQRT */
Float njOuterProduct(NJS_VECTOR *v1, NJS_VECTOR *v2, NJS_VECTOR *ov)
{
    shz_vec3_t a = zam_load_vec3(v1);
    shz_vec3_t b = zam_load_vec3(v2);
    shz_vec3_t out = shz_vec3_init(__builtin_fmaf(-a.z, b.y, a.y * b.z),
                                   __builtin_fmaf(a.x, -b.z, a.z * b.x),
                                   __builtin_fmaf(a.y, -b.x, a.x * b.y));

    zam_store_vec3(ov, out);
    return __builtin_sqrtf(zam_squared_length(out));
}
#else
/* clang-format off */
/* the body ends with ov's length, the result, in FR0. it has no return of
 * its own: it runs into the RTS that SHC writes after it */
/* @asm operand: fipr-registers, FIPR FV0,FV8 at 0x8c1196c2 takes ov's length
 * squared */
#pragma inline_asm(njOuterProduct)
Float njOuterProduct(NJS_VECTOR *v1, NJS_VECTOR *v2, NJS_VECTOR *ov)
{
        MOV         #4,R0
        FMOV.S      @R4,FR10
        FMOV.S      @R5,FR6
        FMOV.S      @(R0,R4),FR4
        FMOV.S      @(R0,R5),FR5
        MOV         #8,R0
        FMOV.S      FR4,FR8
        FMOV.S      @(R0,R4),FR9
        FMOV.S      @(R0,R5),FR11
        FMUL        FR11,FR8
        FMOV.S      FR9,FR0
        FNEG        FR0
        FMAC        FR0,FR5,FR8
        MOV         #4,R0
        FMOV.S      FR8,@R6
        FMUL        FR6,FR9
        FMOV.S      FR10,FR0
        FNEG        FR11
        FMAC        FR0,FR11,FR9
        FMOV.S      FR9,@(R0,R6)
        FMUL        FR5,FR10
        FNEG        FR6
        FMOV.S      FR4,FR0
        FMAC        FR0,FR6,FR10
        MOV         #8,R0
        FMOV.S      FR10,@(R0,R6)
        FMOV.S      FR8,FR0
        FMOV.S      FR9,FR1
        FMOV.S      FR10,FR2
        FLDI0       FR3
        FLDI0       FR11
        FIPR        FV0,FV8
        FMOV.S      FR11,FR0
        FSQRT       FR0
}
/* clang-format on */
#endif
#endif

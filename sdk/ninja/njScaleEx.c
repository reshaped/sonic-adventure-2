#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. SH4ZAM's scale, through ninja_zam.h,
 * runs the same twelve FMULs */
#undef njScaleEx
void njScaleEx(NJS_VECTOR *v)
{
    zam_njScaleEx(v);
}
#else
/* clang-format off */
/* the scale multiplies rows 0, 1 and 2 by v's x, y and z. row 3 stays as it
 * is. the current matrix's rows pass through FR0 to FR3 and FR8 to FR11
 * between FSCHGs */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c118a88 follows the body's own
 * return at 0x8c118a84 */
#pragma inline_asm(njScaleEx)
void njScaleEx(NJS_VECTOR *v)
{
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        FMOV.S      @R4+,FR6
        FSCHG
        FMOV        XD0,DR0
        FMUL        FR4,FR0
        FMOV        XD2,DR2
        FMUL        FR4,FR1
        FMOV        XD4,DR8
        FMUL        FR4,FR2
        FMOV        XD6,DR10
        FMUL        FR4,FR3
        FMOV        DR0,XD0
        FMUL        FR5,FR8
        FMOV        DR2,XD2
        FMUL        FR5,FR9
        FMOV        XD8,DR0
        FMUL        FR5,FR10
        FMOV        XD10,DR2
        FMUL        FR5,FR11
        FMOV        DR8,XD4
        FMUL        FR6,FR0
        FMOV        DR10,XD6
        FMUL        FR6,FR1
        FMUL        FR6,FR2
        FMOV        DR0,XD8
        FMUL        FR6,FR3
        FMOV        DR2,XD10
        RTS
        FSCHG
}
/* clang-format on */
#endif
#endif

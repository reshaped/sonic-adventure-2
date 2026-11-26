/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ and sends a vertex as njCnkEasyMultiPolygon's does. SZ is
 * clear around its call of njCnkEasyMultiPolygonNearClip, as SHC's calling
 * interface needs */
/* @bug uninitialized: hands the near clip a triangle whose greatest z is
 * exactly 1 (FCMP/GT FR0,FR11 at 0x8c143fba). the near clip sends four
 * vertices with unset positions for it (from 0x8c15db40) */
/* @asm operand: fschg-pairs, FMOV.S @R11,FR12 at 0x8c143f18 loads v2's z
 * and x as a pair */
#pragma inline_asm(njCnkEasyMultiPolygonClip)
Uint32 *njCnkEasyMultiPolygonClip(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                                  Sint32 nbstrip, Uint32 *sq, Float a, Float r,
                                  Float g, Float b, Float ar, Float ag,
                                  Float ab)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_draw_polygon_
        .IMPORT     _njCnkEasyMultiPolygonNearClip
        MOV.L       R8,@-R15
        MOV.L       R9,@-R15
        MOV.L       R10,@-R15
        MOV.L       R11,@-R15
        MOV.L       R12,@-R15
        MOV.L       R13,@-R15
        MOV.L       R14,@-R15
        FMOV.S      FR12,@-R15
        FMOV.S      FR13,@-R15
        FMOV.S      FR14,@-R15
        FMOV.S      FR15,@-R15
        STS.L       PR,@-R15
        MOV.L       ?mask,R0
        MOV         R4,R2
        ADD         #6,R2
        PREF        @R2
        MOV         R6,R1
        AND         R0,R1
        MOV         #-14,R2
        SHLD        R2,R6
        MOV         #3,R3
        MOV         #0,R2
        AND         R3,R6
        FLDI1       FR11
        MOV         #0,R3
        SHLL        R6
        MOV.L       ?vertex_control,R13
        MOV.L       ?end_control,R14
        FSCHG
?strip:
        MOV         #5,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
?forward:
        ADD         R8,R3
        ADD         #-2,R8
        MOV         #0,R12
        MOV         #5,R0
        MOV.W       @R4+,R10
        SHLD        R0,R10
        ADD         R5,R10
        PREF        @R10
?triangle:
        MOV         #5,R0
        MOV.W       @R4+,R11
        SHLD        R0,R11
        FMOV.S      @R9,FR0
        ADD         R5,R11
        PREF        @R11
        FCMP/GT     FR0,FR11
        FMOV.S      @R10,FR2
        BT          ?near
        FCMP/GT     FR2,FR11
        FMOV.S      @R11,FR12
        BT          ?near
        FCMP/GT     FR12,FR11
        BT          ?near
        TST         R12,R12
        BF          ?continue_run
        ADD         #16,R9
        FMOV.S      @R9+,FR12
        FMUL        FR4,FR12
        FMOV.S      @R9+,FR14
        FMUL        FR5,FR13
        ADD         #-32,R9
        FMUL        FR6,FR14
        FMOV.S      @R9+,FR0
        FMUL        FR7,FR15
        FMOV.S      @R9+,FR2
        FADD        FR8,FR13
        ADD         #-16,R9
        FADD        FR9,FR14
        FADD        FR10,FR15
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        FMOV.S      FR12,@-R7
        ADD         #1,R12
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        ADD         #32,R7
?continue_run:
        ADD         #16,R10
        FMOV.S      @R10+,FR12
        FMUL        FR4,FR12
        FMOV.S      @R10+,FR14
        FMUL        FR5,FR13
        ADD         #-32,R10
        FMUL        FR6,FR14
        FMOV.S      @R10+,FR0
        FMUL        FR7,FR15
        FMOV.S      @R10+,FR2
        FADD        FR8,FR13
        ADD         #-16,R10
        FADD        FR9,FR14
        FADD        FR10,FR15
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        FMOV.S      FR12,@-R7
        ADD         #1,R12
        FMOV.S      FR2,@-R7
        FMOV.S      FR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        BRA         ?next_triangle
        ADD         #32,R7
?near:
        TST         R12,R12
        BT          ?check_near
        ADD         #16,R10
        FMOV.S      @R10+,FR12
        FMUL        FR4,FR12
        FMOV.S      @R10+,FR14
        FMUL        FR5,FR13
        ADD         #-32,R10
        FMUL        FR6,FR14
        FMOV.S      @R10+,FR0
        FMUL        FR7,FR15
        FMOV.S      @R10+,FR2
        FADD        FR8,FR13
        ADD         #-16,R10
        FADD        FR9,FR14
        FADD        FR10,FR15
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        ADD         #1,R12
        FMOV.S      FR12,@-R7
        ADD         R12,R2
        FMOV.S      FR2,@-R7
        MOV         #0,R12
        FMOV.S      FR0,@-R7
        MOV.L       R14,@R7
        PREF        @R7
        ADD         #32,R7
?check_near:
        FMOV.S      @R9,FR0
        FCMP/GT     FR0,FR11
        FMOV.S      @R10,FR2
        BF          ?clip
        FCMP/GT     FR2,FR11
        FMOV.S      @R11,FR0
        BF          ?clip
        FCMP/GT     FR0,FR11
        BF          ?clip
?next_triangle:
        DT          R8
        ADD         R6,R4
        MOV         R10,R9
        BF/S        ?triangle
        MOV         R11,R10
        TST         R12,R12
        BT          ?strip_done
        ADD         #16,R11
        FMOV.S      @R11+,FR12
        FMUL        FR4,FR12
        FMOV.S      @R11+,FR14
        FMUL        FR5,FR13
        ADD         #-32,R11
        FMUL        FR6,FR14
        FMOV.S      @R11+,FR0
        FMUL        FR7,FR15
        FMOV.S      @R11+,FR2
        FADD        FR8,FR13
        ADD         #-16,R11
        FADD        FR9,FR14
        FADD        FR10,FR15
        ADD         #32,R7
        FMOV.S      FR14,@-R7
        ADD         R12,R2
        FMOV.S      FR12,@-R7
        FMOV.S      FR2,@-R7
        ADD         #1,R2
        FMOV.S      FR0,@-R7
        MOV.L       R14,@R7
        PREF        @R7
        ADD         #32,R7
?strip_done:
        DT          R1
        BT          ?done
        BRA         ?strip
        NOP
?done:
        FSCHG
        MOV.L       ?draw,R4
        MOV         R7,R0
        MOV.L       ?calc,R5
        MOV.L       @R4,R6
        MOV.L       @R5,R7
        ADD         R2,R6
        ADD         R3,R7
        MOV.L       R6,@R4
        MOV.L       R7,@R5
        LDS.L       @R15+,PR
        FMOV.S      @R15+,FR15
        FMOV.S      @R15+,FR14
        FMOV.S      @R15+,FR13
        FMOV.S      @R15+,FR12
        MOV.L       @R15+,R14
        MOV.L       @R15+,R13
        MOV.L       @R15+,R12
        MOV.L       @R15+,R11
        MOV.L       @R15+,R10
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R8
?clip:
        FSCHG
        MOV.L       R1,@-R15
        MOV.L       R2,@-R15
        MOV.L       R3,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        FMOV.S      FR4,@-R15
        FMOV.S      FR5,@-R15
        FMOV.S      FR6,@-R15
        FMOV.S      FR7,@-R15
        FMOV.S      FR8,@-R15
        FMOV.S      FR9,@-R15
        FMOV.S      FR10,@-R15
        MOV.L       ?near_clip,R1
        MOV         R9,R4
        MOV         R10,R5
        JSR         @R1
        MOV         R11,R6
        FMOV.S      @R15+,FR10
        FMOV.S      @R15+,FR9
        FMOV.S      @R15+,FR8
        FMOV.S      @R15+,FR7
        FMOV.S      @R15+,FR6
        FMOV.S      @R15+,FR5
        FMOV.S      @R15+,FR4
        MOV         R0,R7
        MOV.L       @R15+,R6
        MOV.L       @R15+,R5
        MOV.L       @R15+,R4
        MOV.L       @R15+,R3
        MOV.L       @R15+,R2
        MOV.L       @R15+,R1
        FLDI1       FR11
        BRA         ?next_triangle
        FSCHG
        .ALIGN      4
?mask:
        .DATA.L     H'00003FFF
?vertex_control:
        .DATA.L     H'E0000000
?end_control:
        .DATA.L     H'F0000000
?draw:
        .DATA.L     __nj_draw_polygon_
?calc:
        .DATA.L     __nj_calc_polygon_
?near_clip:
        .DATA.L     _njCnkEasyMultiPolygonNearClip
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
#include <nj_cnk.h>

/* the body sets SZ by FSCHG, so each FMOV moves a pair of floats. between its
 * first FSCHG and its last, SZ is clear only for each triangle's facing test.
 * a vertex is its entry's first 32 bytes with the color lit, moved as four
 * pairs. its control word then replaces the entry's z in word 0 */
/* @asm operand: fschg-pairs, FMOV @R9+,DR12 at 0x8c1466bc loads an entry's a
 * and r as a pair */
#pragma inline_asm(njCnkSimpleMultiPolygon)
Uint32 *njCnkSimpleMultiPolygon(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float a, Float r,
                                Float g, Float b, Float ar, Float ag, Float ab)
{
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_control_3d_flag_
        .IMPORT     __nj_draw_polygon_
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
        ADD         #-4,R15
        MOV.L       ?flag,R0
        MOV.L       @R0,R0
        MOV.L       R0,@R15
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
        MOV         #0,R3
        SHLL        R6
        MOV.L       ?vertex_control,R13
        MOV.L       ?end_control,R14
        FSCHG
?strip:
        MOV         #6,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        MOV.L       @R15,R0
        SHLL        R0
        FLDI1       FR11
        BF          ?unmirrored
        FNEG        FR11
?unmirrored:
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
        FNEG        FR11
?forward:
        ADD         R8,R3
        ADD         #-2,R8
        MOV         #0,R12
        MOV         #6,R0
        MOV.W       @R4+,R10
        SHLD        R0,R10
        ADD         R5,R10
        PREF        @R10
?triangle:
        MOV.W       @R4+,R11
        FSCHG
        ADD         #4,R9
        MOV         #6,R0
        FMOV.S      @R9+,FR0
        ADD         #4,R10
        FMOV.S      @R9+,FR2
        SHLD        R0,R11
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR3
        PREF        @R11
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR3,FR2
        ADD         #4,R11
        FMOV.S      @R11+,FR12
        ADD         #20,R4
        FMOV.S      @R11+,FR13
        ADD         #-12,R11
        FSUB        FR1,FR12
        FSUB        FR3,FR13
        PREF        @R4
        FMUL        FR0,FR13
        FMUL        FR2,FR12
        FLDI0       FR0
        FSUB        FR12,FR13
        ADD         #-20,R4
        FMUL        FR11,FR13
        FSCHG
        FCMP/GT     FR0,FR13
        FNEG        FR11
        BF/S        ?back
        TST         R12,R12
        BF          ?open_v1
        ADD         #16,R9
        FMOV        @R9+,DR12
        ADD         #1,R12
        FMOV        @R9+,DR14
        FMUL        FR7,FR15
        ADD         #-32,R9
        FMUL        FR6,FR14
        FMOV        @R9+,DR0
        FMUL        FR5,FR13
        FMOV        @R9+,DR2
        FMUL        FR4,FR12
        ADD         #32,R7
        FADD        FR10,FR15
        ADD         #-16,R9
        FADD        FR9,FR14
        FMOV        DR14,@-R7
        FADD        FR8,FR13
        FMOV        DR12,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        ADD         #32,R7
?open_v1:
        ADD         #16,R10
        FMOV        @R10+,DR12
        ADD         #1,R12
        FMOV        @R10+,DR14
        FMUL        FR7,FR15
        ADD         #-32,R10
        FMUL        FR6,FR14
        FMOV        @R10+,DR0
        FMUL        FR5,FR13
        FMOV        @R10+,DR2
        FMUL        FR4,FR12
        ADD         #32,R7
        FADD        FR10,FR15
        ADD         #-16,R10
        FADD        FR9,FR14
        FMOV        DR14,@-R7
        FADD        FR8,FR13
        FMOV        DR12,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        BRA         ?next
        ADD         #32,R7
?back:
        BT          ?next
        ADD         #16,R10
        FMOV        @R10+,DR12
        ADD         #1,R12
        FMOV        @R10+,DR14
        FMUL        FR7,FR15
        ADD         #-32,R10
        FMUL        FR6,FR14
        FMOV        @R10+,DR0
        FMUL        FR5,FR13
        FMOV        @R10+,DR2
        FMUL        FR4,FR12
        ADD         #32,R7
        FADD        FR10,FR15
        ADD         #-16,R10
        FADD        FR9,FR14
        FMOV        DR14,@-R7
        FADD        FR8,FR13
        FMOV        DR12,@-R7
        ADD         R12,R2
        FMOV        DR2,@-R7
        MOV         #0,R12
        FMOV        DR0,@-R7
        MOV.L       R14,@R7
        PREF        @R7
        ADD         #32,R7
?next:
        MOV         R10,R9
        DT          R8
        MOV         R11,R10
        BF/S        ?triangle
        ADD         R6,R4
        TST         R12,R12
        BT          ?strip_done
        ADD         #16,R11
        FMOV        @R11+,DR12
        ADD         #1,R12
        FMOV        @R11+,DR14
        FMUL        FR7,FR15
        ADD         #-32,R11
        FMUL        FR6,FR14
        FMOV        @R11+,DR0
        FMUL        FR5,FR13
        FMOV        @R11+,DR2
        FMUL        FR4,FR12
        ADD         #32,R7
        FADD        FR10,FR15
        ADD         #-16,R11
        FADD        FR9,FR14
        FMOV        DR14,@-R7
        FADD        FR8,FR13
        FMOV        DR12,@-R7
        ADD         R12,R2
        FMOV        DR2,@-R7
        MOV         #0,R12
        FMOV        DR0,@-R7
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
        ADD         #4,R15
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
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#endif
/* clang-format on */

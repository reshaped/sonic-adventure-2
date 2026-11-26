#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimplePolygon serves vertices lit by an intensity, in NJD_CS strips
 * without NJD_FST_DB or NJD_FST_ENV. a sent vertex's words 1 to 3 are its
 * entry's x, y and ooz. word 6 is its lit inten, and word 7 its entry's spec */
Uint32 *njCnkSimplePolygon(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                           Uint32 nbstrip, Uint32 *sq, Float ambient);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes NJS_CNK_SIMPLE_BUF's size, 32 bytes, and the offsets of
 * its fields from z to spec. it sets FPSCR.SZ by FSCHG, so an FMOV moves a pair
 * of floats. it clears SZ only for the move that keeps a lit inten from falling
 * below ambient. a vertex's words 0 to 3 are its entry's z, x, y and ooz, moved
 * as two pairs. its control word then replaces z */
/* @asm operand: integer-constant, MOV.L ?mask,R0 at 0x8c16129c loads H'3FFF,
 * the mask of nbstrip's strip count, from the pool */
#pragma inline_asm(njCnkSimplePolygon)
Uint32 *njCnkSimplePolygon(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                           Uint32 nbstrip, Uint32 *sq, Float ambient)
{
        .IMPORT     __nj_control_3d_flag_
        .IMPORT     __nj_calc_polygon_
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
        MOV         #5,R0
        MOV.W       @R4+,R8
        MOV.W       @R4+,R9
        SHLD        R0,R9
        MOV.L       @R15,R0
        SHLL        R0
        FLDI1       FR5
        BF          ?unmirrored
        FNEG        FR5
?unmirrored:
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
        FNEG        FR5
?forward:
        MOV         #5,R0
        MOV.W       @R4+,R10
        SHLD        R0,R10
        ADD         R8,R3
        ADD         R5,R10
        ADD         #-2,R8
        PREF        @R10
        MOV         #0,R12
?triangle:
        FMOV        @R9+,DR0
        MOV         #5,R0
        FMOV        @R9+,DR2
        ADD         #-16,R9
        MOV.W       @R4+,R11
        SHLD        R0,R11
        FMOV        @R10+,DR6
        ADD         R5,R11
        FMOV        @R10+,DR8
        FMOV        @R11+,DR10
        FSUB        FR7,FR11
        FMOV        @R11+,DR12
        FSUB        FR1,FR7
        ADD         #-16,R11
        FSUB        FR8,FR12
        FSUB        FR2,FR8
        ADD         #-16,R10
        FMUL        FR12,FR7
        FMUL        FR11,FR8
        FLDI0       FR6
        FSUB        FR8,FR7
        FMUL        FR5,FR7
        FCMP/GT     FR7,FR6
        FNEG        FR5
        BF/S        ?hidden
        TST         R12,R12
        BF          ?front_v1
        FMOV        @R9+,DR0
        ADD         #1,R12
        FMOV        @R9+,DR2
        ADD         #24,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
        ADD         #-8,R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp1
        FMOV.S      FR4,FR6
?clamp1:
        FSCHG
        MOV.L       R13,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?front_v1:
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        ADD         #-8,R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp2
        FMOV.S      FR4,FR6
?clamp2:
        FSCHG
        MOV.L       R13,@R7
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        BRA         ?next
        ADD         #8,R7
?hidden:
        BT          ?next
        FMOV        @R10+,DR0
        ADD         #1,R12
        FMOV        @R10+,DR2
        ADD         #24,R7
        FMOV        @R10+,DR6
        ADD         #-24,R10
        ADD         #-8,R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp3
        FMOV.S      FR4,FR6
?clamp3:
        ADD         R12,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        MOV         #0,R12
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?next:
        DT          R8
        MOV         R10,R9
        ADD         R6,R4
        BF/S        ?triangle
        MOV         R11,R10
        TST         R12,R12
        BT          ?strip_next
        FMOV        @R11+,DR0
        ADD         R12,R2
        FMOV        @R11+,DR2
        ADD         #24,R7
        FMOV        @R11+,DR6
        ADD         #-24,R11
        ADD         #-8,R7
        FADD        FR4,FR6
        FMOV        DR2,@-R7
        FCMP/GT     FR6,FR4
        FMOV        DR0,@-R7
        FSCHG
        BF          ?clamp4
        FMOV.S      FR4,FR6
?clamp4:
        ADD         #1,R2
        MOV.L       R14,@R7
        FSCHG
        ADD         #24,R7
        FMOV        DR6,@R7
        PREF        @R7
        ADD         #8,R7
?strip_next:
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

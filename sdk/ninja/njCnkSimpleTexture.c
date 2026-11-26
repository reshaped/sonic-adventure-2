#include <nj_cnk.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* njCnkSimpleTexture is njCnkSimplePolygon for NJD_CS_UVN and NJD_CS_UVH
 * strips */
Uint32 *njCnkSimpleTexture(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                           Uint32 nbstrip, Uint32 *sq, Float ambient,
                           Float uvscale);
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* the body hard-codes NJS_CNK_SIMPLE_BUF's size, 32 bytes, and the offsets of
 * its fields from z to spec. it sets FPSCR.SZ by FSCHG, so an FMOV moves a pair
 * of floats. it clears SZ for the single moves that load a facing test's x and
 * y. it clears SZ too for the move that keeps a lit inten from falling below
 * ambient. a vertex's words 0 to 3 are its entry's z, x, y and ooz, moved as
 * two pairs. the control word then overwrites word 0, z */
/* @asm operand: integer-constant, MOV.L ?mask,R0 at 0x8c165790 loads H'3FFF,
 * the mask of nbstrip's strip count, from the pool */
#pragma inline_asm(njCnkSimpleTexture)
Uint32 *njCnkSimpleTexture(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                           Uint32 nbstrip, Uint32 *sq, Float ambient,
                           Float uvscale)
{
        .IMPORT     __nj_draw_polygon_
        .IMPORT     __nj_calc_polygon_
        .IMPORT     __nj_control_3d_flag_
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
        FLDI1       FR9
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
        FLDI1       FR9
        BF          ?unmirrored
        FNEG        FR9
?unmirrored:
        ADD         R5,R9
        CMP/PZ      R8
        PREF        @R9
        BT          ?forward
        NEG         R8,R8
        FNEG        FR9
?forward:
        ADD         R8,R3
        ADD         #-2,R8
        MOV         #0,R12
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR10
        LDS         R0,FPUL
        MOV         #5,R0
        FLOAT       FPUL,FR11
        MOV.W       @R4+,R10
        SHLD        R0,R10
        FMUL        FR5,FR10
        ADD         R5,R10
        FMUL        FR5,FR11
        PREF        @R10
        MOV.W       @R4+,R0
        LDS         R0,FPUL
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR12
        LDS         R0,FPUL
        FLOAT       FPUL,FR13
        FMUL        FR5,FR12
        FMUL        FR5,FR13
?triangle:
        MOV.W       @R4+,R11
        FSCHG
        MOV.W       @R4+,R0
        ADD         #4,R9
        LDS         R0,FPUL
        ADD         #4,R10
        MOV.W       @R4+,R0
        FLOAT       FPUL,FR14
        LDS         R0,FPUL
        MOV         #5,R0
        FMOV.S      @R9+,FR0
        SHLD        R0,R11
        FMOV.S      @R9+,FR6
        FLOAT       FPUL,FR15
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR7
        FMUL        FR5,FR14
        PREF        @R11
        FMUL        FR5,FR15
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR7,FR6
        ADD         #4,R11
        FMOV.S      @R11+,FR2
        ADD         #20,R4
        FMOV.S      @R11+,FR8
        ADD         #-12,R11
        FSUB        FR1,FR2
        FSUB        FR7,FR8
        PREF        @R4
        FMUL        FR0,FR8
        FMUL        FR2,FR6
        FLDI0       FR7
        FSUB        FR8,FR6
        ADD         #-20,R4
        FMUL        FR9,FR6
        FSCHG
        FCMP/GT     FR6,FR7
        FNEG        FR9
        BF/S        ?hidden
        TST         R12,R12
        BF          ?front_v1
        FMOV        @R9+,DR0
        ADD         #1,R12
        FMOV        @R9+,DR2
        ADD         #24,R7
        FMOV        @R9+,DR6
        ADD         #-24,R9
        FMOV        DR10,@-R7
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
        FMOV        DR12,@-R7
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
        FMOV        DR12,@-R7
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
        MOV         R10,R9
        ADD         R6,R4
        MOV         R11,R10
        DT          R8
        FMOV        DR12,DR10
        BF/S        ?triangle
        FMOV        DR14,DR12
        TST         R12,R12
        BT          ?strip_next
        FMOV        @R11+,DR0
        ADD         R12,R2
        FMOV        @R11+,DR2
        ADD         #24,R7
        FMOV        @R11+,DR6
        ADD         #-24,R11
        FMOV        DR14,@-R7
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

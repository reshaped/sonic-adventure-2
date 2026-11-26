#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* send the strips as njCnkSimpleMultiPolygonEnv does while
 * NJD_CONTROL_3D_ENV_UV_SCROLL is clear. it never offsets u and v, whatever the
 * flag holds */
Uint32 *_njCnkSimpleMultiPolygonEnv(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float a,
                                    Float ar, Float ag, Float ab);
/* NOLINTEND(misc-use-internal-linkage) */

/* send entry as a vertex at sq, with param as its control word. sq is left past
 * the vertex */
#define SEND_VERTEX(entry, param)                                              \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float sx;                                                     \
        register Float sy;                                                     \
        register Float ooz;                                                    \
        register Float u;                                                      \
        register Float v;                                                      \
        register Float ca;                                                     \
        register Float cr;                                                     \
        register Float cg;                                                     \
        register Float cb;                                                     \
        register Float sr;                                                     \
        register Float sg;                                                     \
        register Float sb;                                                     \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->z;                                                       \
        /* @hack spelling: the entry's second block prefetched a block past    \
         * its z, where &(entry)->x would do */                                \
        prefetch(p + CNK_BLOCK_WORDS);                                         \
        p++;                                                                   \
        sx = *p++;                                                             \
        sy = *p++;                                                             \
        ooz = *p++;                                                            \
        p += CNK_SIMPLEMULTI_NX - CNK_SIMPLEMULTI_A;                           \
        u = uoff + (*p++ * CNK_ENV_SCALE);                                     \
        v = voff + (*p * CNK_ENV_SCALE);                                       \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = sy;                                                   \
        *(Float *)--sq = sx;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        p -= CNK_SIMPLEMULTI_NY - CNK_SIMPLEMULTI_A;                           \
        ca = a * *p++;                                                         \
        cr = ar + *p++;                                                        \
        cg = ag + *p++;                                                        \
        cb = ab + *p++;                                                        \
        p += CNK_SIMPLEMULTI_SR - CNK_SIMPLEMULTI_X;                           \
        sr = *p++;                                                             \
        sg = *p++;                                                             \
        sb = *p;                                                               \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = sb;                                                   \
        *(Float *)--sq = sg;                                                   \
        *(Float *)--sq = sr;                                                   \
        sq--;                                                                  \
        *(Float *)--sq = cb;                                                   \
        *(Float *)--sq = cg;                                                   \
        *(Float *)--sq = cr;                                                   \
        *(Float *)--sq = ca;                                                   \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleMultiPolygonEnv(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq, Float a,
                                   Float ar, Float ag, Float ab)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLEMULTI_BUF *v0;
    register NJS_CNK_SIMPLEMULTI_BUF *v1;
    register NJS_CNK_SIMPLEMULTI_BUF *v2;
    register Sint32 len;
    register Sint32 calculated;
    register Sint32 drawn;
    register Sint32 sent;
    register Float side;
    register Float uoff;
    register Float voff;
    register Float model_side;
    register Float facing;
    register Sint32 user_halfwords;

    /* @hack spelling: the call of _njCnkSimpleMultiPolygonEnv in the test's
     * else, where a return under the negated test would do */
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + CNK_ENV_CENTER;
        voff = _nj_uv_scroll_v_ + CNK_ENV_CENTER;
    } else {
        return _njCnkSimpleMultiPolygonEnv(strips, buf, nbstrip, sq, a, ar, ag,
                                           ab);
    }
    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    /* @hack spelling: nbstrip = nbstrip & STRIP_COUNT, where
     * nbstrip &= STRIP_COUNT would do */
    nbstrip = nbstrip & STRIP_COUNT;
    /* @hack spelling: calculated and drawn zeroed in one chained assignment,
     * where a statement for each would do */
    calculated = drawn = 0;
    model_side = 1.0F;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_MIRROR_MODEL) {
        model_side = -1.0F;
    }
    do {
        len = *strips++;
        v0 = &buf[*strips++];
        prefetch(v0);
        side = model_side;
        if (len < 0) {
            len = -len;
            side = -side;
        }
        calculated += len;
        len -= CNK_STRIP_LEAD;
        sent = 0;
        v1 = &buf[*strips++];
        prefetch(v1);
        do {
            v2 = &buf[*strips++];
            /* @bug logic: prefetches v1 again, where v2, the entry just
             * read, is meant (PREF @R13 at 0x8c146166) */
            prefetch(v1);
            facing = (((v1->sx - v0->sx) * (v2->sy - v1->sy)) -
                      ((v1->sy - v0->sy) * (v2->sx - v1->sx))) *
                     side;
            if (facing < 0.0F) {
                if (sent == 0) {
                    SEND_VERTEX(v0, KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                SEND_VERTEX(v1, KM_VERTEXPARAM_NORMAL)
                sent++;
            } else if (sent != 0) {
                SEND_VERTEX(v1, KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            side = -side;
            v0 = v1;
            v1 = v2;
        } while (--len);
        if (sent != 0) {
            SEND_VERTEX(v2, KM_VERTEXPARAM_ENDOFSTRIP)
            sent++;
            drawn += sent;
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}

/* clang-format off */
#ifdef DREAMCAST
/* the body sends the strips as njCnkSimpleMultiPolygonEnv does with uoff and
 * voff each CNK_ENV_CENTER. it also sends each entry's nz as word 12. its
 * facing is (v0 - v1) x (v2 - v1) times side, the negation of that function's.
 * a triangle faces the viewer where the body's facing is above 0 */
/* @bug logic: doesn't skip the user halfwords after a strip's last triangle.
 * where the chunk has them, the next strip is read from them. BT ?strip_end
 * at 0x8c14655c branches past ADD R6,R4 at 0x8c146560, which skips them after
 * each other triangle in BRA's delay slot */
/* @asm operand: fschg-pairs, FMOV @R9,DR10 at 0x8c14648a loads an entry's nx
 * and ny as a pair */
#pragma inline_asm(send_unscrolled)
static Uint32 *send_unscrolled(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float a, Float ar,
                               Float ag, Float ab)
{
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
        MOVA        ?half,R0
        FMOV.S      @R0,FR8
        FSCHG
?strip:
        MOV         #6,R0
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
        MOV         #6,R0
        MOV.W       @R4+,R10
        SHLD        R0,R10
        ADD         R5,R10
        PREF        @R10
?triangle:
        MOV.W       @R4+,R11
        FSCHG
        ADD         #4,R9
        ADD         #4,R10
        MOV         #6,R0
        FMOV.S      @R9+,FR0
        SHLD        R0,R11
        FMOV.S      @R9+,FR2
        FMOV.S      @R10+,FR1
        ADD         R5,R11
        FMOV.S      @R10+,FR3
        PREF        @R11
        ADD         #-12,R9
        FSUB        FR1,FR0
        ADD         #-12,R10
        FSUB        FR3,FR2
        ADD         #4,R11
        FMOV.S      @R11+,FR10
        ADD         #32,R10
        FMOV.S      @R11+,FR11
        ADD         #-12,R11
        FSUB        FR1,FR10
        FSUB        FR3,FR11
        PREF        @R10
        FMUL        FR0,FR11
        FMUL        FR2,FR10
        FLDI0       FR0
        FSUB        FR10,FR11
        ADD         #-32,R10
        FMUL        FR9,FR11
        FSCHG
        FCMP/GT     FR0,FR11
        FNEG        FR9
        BF/S        ?back
        TST         R12,R12
        BF          ?open_v1
        ADD         #40,R9
        FMOV        @R9,DR10
        ADD         #-40,R9
        FMUL        FR8,FR10
        FMOV        @R9+,DR0
        FMUL        FR8,FR11
        FMOV        @R9+,DR2
        FADD        FR8,FR10
        ADD         #24,R7
        FADD        FR8,FR11
        FMOV        DR10,@-R7
        FMOV        DR2,@-R7
        ADD         #1,R12
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        FMOV        @R9+,DR0
        ADD         #64,R7
        FMOV        @R9+,DR2
        ADD         #16,R9
        FMUL        FR4,FR0
        FMOV        @R9+,DR12
        FADD        FR5,FR1
        FMOV        @R9+,DR14
        FADD        FR6,FR2
        FMOV        DR14,@-R7
        FADD        FR7,FR3
        FMOV        DR12,@-R7
        ADD         #-64,R9
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        ADD         #32,R7
?open_v1:
        ADD         #40,R10
        FMOV        @R10,DR10
        ADD         #-40,R10
        FMUL        FR8,FR10
        FMOV        @R10+,DR0
        FMUL        FR8,FR11
        FMOV        @R10+,DR2
        FADD        FR8,FR10
        ADD         #24,R7
        FADD        FR8,FR11
        FMOV        DR10,@-R7
        FMOV        DR2,@-R7
        ADD         #1,R12
        FMOV        DR0,@-R7
        MOV.L       R13,@R7
        PREF        @R7
        FMOV        @R10+,DR0
        ADD         #64,R7
        FMOV        @R10+,DR2
        ADD         #16,R10
        FMUL        FR4,FR0
        FMOV        @R10+,DR12
        FADD        FR5,FR1
        FMOV        @R10+,DR14
        FADD        FR6,FR2
        FMOV        DR14,@-R7
        FADD        FR7,FR3
        FMOV        DR12,@-R7
        ADD         #-64,R10
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        BRA         ?next
        ADD         #32,R7
?back:
        BT          ?next
        ADD         #40,R10
        FMOV        @R10,DR10
        ADD         #-40,R10
        FMUL        FR8,FR10
        FMOV        @R10+,DR0
        FMUL        FR8,FR11
        ADD         #1,R12
        FADD        FR8,FR10
        FMOV        @R10+,DR2
        FADD        FR8,FR11
        ADD         #24,R7
        FMOV        DR10,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        ADD         R12,R2
        MOV.L       R14,@R7
        MOV         #0,R12
        PREF        @R7
        FMOV        @R10+,DR0
        ADD         #64,R7
        FMOV        @R10+,DR2
        ADD         #16,R10
        FMUL        FR4,FR0
        FMOV        @R10+,DR12
        FADD        FR5,FR1
        FMOV        @R10+,DR14
        FADD        FR6,FR2
        FMOV        DR14,@-R7
        FADD        FR7,FR3
        FMOV        DR12,@-R7
        ADD         #-64,R10
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        PREF        @R7
        ADD         #32,R7
?next:
        MOV         R10,R9
        DT          R8
        MOV         R11,R10
        BT          ?strip_end
        BRA         ?triangle
        ADD         R6,R4
?strip_end:
        TST         R12,R12
        BT          ?strip_done
        ADD         #40,R11
        FMOV        @R11,DR10
        ADD         #-40,R11
        FMUL        FR8,FR10
        FMOV        @R11+,DR0
        FMUL        FR8,FR11
        ADD         R12,R2
        FADD        FR8,FR10
        FMOV        @R11+,DR2
        FADD        FR8,FR11
        ADD         #24,R7
        FMOV        DR10,@-R7
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
        ADD         #1,R2
        MOV.L       R14,@R7
        PREF        @R7
        FMOV        @R11+,DR0
        ADD         #64,R7
        FMOV        @R11+,DR2
        ADD         #16,R11
        FMUL        FR4,FR0
        FMOV        @R11+,DR12
        FADD        FR5,FR1
        FMOV        @R11+,DR14
        FADD        FR6,FR2
        FMOV        DR14,@-R7
        FADD        FR7,FR3
        FMOV        DR12,@-R7
        ADD         #-64,R11
        FMOV        DR2,@-R7
        FMOV        DR0,@-R7
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
?half:
        .FDATA.S    F'0.5
?flag:
        .DATA.L     __nj_control_3d_flag_
}
#else
/* NOLINTBEGIN(clang-diagnostic-undefined-internal) */
static Uint32 *send_unscrolled(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                               Uint32 nbstrip, Uint32 *sq, Float a, Float ar,
                               Float ag, Float ab);
/* NOLINTEND(clang-diagnostic-undefined-internal) */
#endif
/* clang-format on */

/* @hack spelling: a call of a static inline body, where an inline body named
 * _njCnkSimpleMultiPolygonEnv would do */
Uint32 *_njCnkSimpleMultiPolygonEnv(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float a,
                                    Float ar, Float ag, Float ab)
{
    return send_unscrolled(strips, buf, nbstrip, sq, a, ar, ag, ab);
}

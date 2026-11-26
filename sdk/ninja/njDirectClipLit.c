/* the near clips the clip drawers call for a strip that crosses the near
 * plane. the lit near clippers cut the strip's triangles, and the bodies here
 * light and project their corners */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_direct.h>
#include <nj_direct_draw.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a store queue holds SQ_WORDS words, and one vertex. a polygon's parameter
 * opens with its PARAMETER_WORDS words: the global, ISP, TSP and texture
 * words */
#define SQ_WORDS 8
#define PARAMETER_WORDS 4
/* a struct direct_lit_screen_vertex's floats */
#define LIT_VERTEX_FLOATS 6
/* a vertex record's 32 bytes, as a shift. a near clip reads a window of
 * records, from CLIP_MARGIN records before the first record that changes side
 * to CLIP_MARGIN records after the last */
#define RECORD_SHIFT 5
#define CLIP_MARGIN 2
/* njDirectClip's result for a model that needs no near clip. such a model lies
 * wholly past the near plane, away from the camera */
#define NO_NEAR_CLIP 1
/* the environment map's u and v are the normal's times ENVIRONMENT_HALF, plus
 * ENVIRONMENT_HALF */
#define ENVIRONMENT_HALF 0.5F

#ifdef DREAMCAST

/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c1272d8 follows the body's own
 * at 0x8c1272d2 */
#pragma inline_asm(clip_edge)
static void clip_edge(const struct direct_lit_corner *first,
                        const struct direct_lit_corner *second,
                        struct direct_lit_screen_vertex *out, const Float *screen)
{
        FMOV.S      @R4+,FR2
        FMOV.S      @R5+,FR3
        FLDI1       FR0
        FSUB        FR2,FR3
        FMOV.S      @R4+,FR1
        FSUB        FR2,FR0
        FMOV.S      @R5+,FR2
        FDIV        FR3,FR0
        FMOV.S      @R4+,FR3
        FMOV.S      @R5+,FR4
        FMOV.S      @R7+,FR5
        FMOV.S      @R7+,FR6
        FMOV.S      @R7+,FR7
        FMOV.S      @R7+,FR8
        FSUB        FR1,FR2
        FMOV.S      @R4+,FR9
        FMAC        FR0,FR2,FR1
        FMOV.S      @R5+,FR10
        FSUB        FR3,FR4
        FMAC        FR0,FR4,FR3
        FMUL        FR5,FR1
        FADD        FR7,FR1
        FMOV.S      @R4+,FR5
        FMUL        FR6,FR3
        FMOV.S      @R5+,FR6
        FADD        FR8,FR3
        FMOV.S      @R4+,FR7
        FSUB        FR9,FR10
        FMOV.S      @R5+,FR8
        FMAC        FR0,FR10,FR9
        FLDI1       FR2
        FMOV.S      FR1,@R6
        ADD         #4,R6
        FSUB        FR5,FR6
        FMOV.S      FR3,@R6
        ADD         #4,R6
        FMAC        FR0,FR6,FR5
        FMOV.S      FR2,@R6
        ADD         #4,R6
        FSUB        FR7,FR8
        FMOV.S      FR9,@R6
        ADD         #4,R6
        FMAC        FR0,FR8,FR7
        FMOV.S      FR5,@R6
        ADD         #4,R6
        RTS
        FMOV.S      FR7,@R6
}
/* clang-format on */
void njDirectClipAdjust(const struct direct_lit_corner *first,
                        const struct direct_lit_corner *second,
                        struct direct_lit_screen_vertex *out,
                        const Float *screen)
{
    clip_edge(first, second, out, screen);
}

/* clang-format off */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c12730c follows the body's own
 * at 0x8c127308 */
#pragma inline_asm(project_corner)
static void project_corner(const struct direct_lit_corner *corner,
                         struct direct_lit_screen_vertex *out, const Float *screen)
{
        FMOV.S      @R4+,FR2
        FLDI1       FR0
        FDIV        FR2,FR0
        FMOV.S      @R4+,FR1
        FMOV.S      @R4+,FR2
        FMOV.S      @R6+,FR6
        FMOV.S      @R6+,FR7
        FMOV.S      @R6+,FR8
        FMOV.S      @R6+,FR9
        FMOV.S      @R4+,FR3
        FMOV.S      @R4+,FR4
        FMOV.S      @R4+,FR5
        ADD         #24,R5
        FMOV.S      FR5,@-R5
        FMUL        FR6,FR1
        FMOV.S      FR4,@-R5
        FMUL        FR7,FR2
        FMOV.S      FR3,@-R5
        FMAC        FR0,FR2,FR9
        FMOV.S      FR0,@-R5
        FMAC        FR0,FR1,FR8
        FMOV.S      FR9,@-R5
        RTS
        FMOV.S      FR8,@-R5
}
/* clang-format on */
void njDirectClipAdjust2(const struct direct_lit_corner *corner,
                         struct direct_lit_screen_vertex *out,
                         const Float *screen)
{
    project_corner(corner, out, screen);
}

/* clang-format off */
/* @asm operand: fipr-registers, FIPR FV0,FV4 at 0x8c127326 takes the
 * normal's inner product with the current light */
#pragma inline_asm(light_corners)
static void light_corners(const Float *light, const NJS_DIRECT_VERTEX *source,
                       struct direct_lit_corner *corners)
{
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        MOV         #3,R0
        FMOV.S      @R4+,FR2
        FMOV.S      @R4+,FR3
light_corner:
        ADD         #12,R5
        FLDI1       FR7
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR4
        ADD         #24,R6
        FMOV.S      @R5+,FR5
        FIPR        FV0,FV4
        MOV.L       @R5+,R1
        FCMP/GT     FR3,FR7
        MOV         R1,R2
        BT          lit
        FMOV.S      FR3,FR7
lit:
        SHLL16      R1
        SHLR16      R2
        MOV.L       R1,@-R6
        SHLL16      R2
        ADD         #4,R5
        MOV.L       R2,@-R6
        DT          R0
        FMOV.S      FR7,@-R6
        BF/S        light_corner
        ADD         #20,R6
}
/* clang-format on */
void njDirectCalcLight(const Float *light, const NJS_DIRECT_VERTEX *source,
                       struct direct_lit_corner *corners)
{
    light_corners(light, source, corners);
}
#endif

#define CLIP_TRIANGLE njDirectNearClipSub
#define FILL_CORNERS()                                                         \
    {                                                                          \
        prefetch(source);                                                      \
        njDirectCalcLight(direct_clip_light, source, corners);                 \
    }
#include <nj_direct_clip_triangle.h>
#undef FILL_CORNERS
#undef CLIP_TRIANGLE

#define READ_ATTRIBUTES()                                                      \
    {                                                                          \
        y = *vertices++;                                                       \
        inverse_z = *vertices++;                                               \
        intensity = *vertices++;                                               \
        u = *vertices++;                                                       \
    }
/* write a lit vertex below queue as the tile accelerator takes it. it holds
 * its command, x, y, 1/z, u and v, then its base and offset intensities. the
 * offset is the light's intensity, and the base 1 more */
#define WRITE_LIT_VERTEX(command)                                              \
    {                                                                          \
        *(Float *)--queue = intensity;                                         \
        intensity += 1.0F;                                                     \
        *(Float *)--queue = intensity;                                         \
        *(Float *)--queue = v;                                                 \
        *(Float *)--queue = u;                                                 \
        *(Float *)--queue = inverse_z;                                         \
        *(Float *)--queue = y;                                                 \
        *(Float *)--queue = x;                                                 \
        *--queue = (command);                                                  \
        prefetch(queue);                                                       \
    }

Uint32 *njDirectClipStore(const Float *vertices, Uint32 *queue, Int count)
{
    /* @hack redundant: register on the six locals */
    register Float x;
    register Float y;
    register Float inverse_z;
    register Float intensity;
    register Float u;
    register Float v;

    queue += SQ_WORDS;
    x = *vertices++;
    READ_ATTRIBUTES();
    v = *vertices++;
    WRITE_LIT_VERTEX(KM_VERTEXPARAM_NORMAL);

    x = *vertices++;
    queue += 2 * SQ_WORDS;
    READ_ATTRIBUTES();
    v = *vertices++;
    WRITE_LIT_VERTEX(KM_VERTEXPARAM_NORMAL);
    queue += SQ_WORDS;

    if (count != DIRECT_TRIANGLE_CORNERS) {
        vertices += LIT_VERTEX_FLOATS;
        x = *vertices++;
        queue += SQ_WORDS;
        READ_ATTRIBUTES();
        v = *vertices++;
        WRITE_LIT_VERTEX(KM_VERTEXPARAM_NORMAL);
        queue += SQ_WORDS;
        vertices -= 2 * LIT_VERTEX_FLOATS;
    }
    x = *vertices++;
    queue += SQ_WORDS;
    READ_ATTRIBUTES();
    v = *vertices;
    WRITE_LIT_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP);
    queue += SQ_WORDS;
    return queue;
}

#define CLIP_TRIANGLE njDirectEnvNearClipSub
#define FILL_CORNERS()                                                         \
    njDirectCalcEnvLight(direct_clip_light, source, corners, ENVIRONMENT_HALF)
#include <nj_direct_clip_triangle.h>
#undef FILL_CORNERS
#undef CLIP_TRIANGLE

#ifdef DREAMCAST
/* clang-format off */
/* @asm operand: fipr-registers, FIPR FV0,FV4 at 0x8c1275f0 takes the
 * normal's inner product with the light */
#pragma inline_asm(light_environment)
static void
#ifdef NONMATCHING
light_environment(const Float *light, const NJS_DIRECT_VERTEX *source,
                  struct direct_lit_corner *corners, Float half)
#else
light_environment(void)
#endif
{
        FMOV.S      FR4,FR9
        FMOV.S      @R4+,FR0
        FMOV.S      @R4+,FR1
        MOV         #3,R0
        FMOV.S      @R4+,FR2
        ADD         #12,R5
        FMOV.S      @R4+,FR3
env_corner:
        FLDI1       FR7
        FMOV.S      @R5+,FR6
        FMOV.S      @R5+,FR4
        ADD         #12,R6
        FMOV.S      @R5+,FR5
        FIPR        FV0,FV4
        FCMP/GT     FR3,FR7
        BT          env_lit
        FMOV.S      FR3,FR7
env_lit:
        FMOV.S      FR7,@R6
        FLDI0       FR7
        FTRV        XMTRX,FV4
        ADD         #20,R5
        ADD         #4,R6
        FMUL        FR9,FR4
        FADD        FR9,FR4
        FMOV.S      FR4,@R6
        FMUL        FR9,FR5
        ADD         #4,R6
        FADD        FR9,FR5
        DT          R0
        FMOV.S      FR5,@R6
        BF/S        env_corner
        ADD         #12,R6
}
/* clang-format on */

void njDirectCalcEnvLight(const Float *light, const NJS_DIRECT_VERTEX *source,
                          struct direct_lit_corner *corners, Float half)
{
#ifdef NONMATCHING
    light_environment(light, source, corners, half);
#else
    /* @hack implicit: the call without its four arguments, which the body
     * reads from njDirectCalcEnvLight's own registers, where passing them
     * would do */
    light_environment();
#endif
}
#endif

#define SET_CLIP_LIGHT()                                                       \
    {                                                                          \
        direct_clip_light[0] = light_x;                                        \
        direct_clip_light[1] = light_y;                                        \
        direct_clip_light[2] = light_z;                                        \
        direct_clip_light[3] = ambient;                                        \
    }

Uint32 *njDirectEnvNearClip(NJS_DIRECT_GLOBAL *strip, Sint32 count,
                            Uint32 *queue, Float light_x, Float light_y,
                            Float light_z, Float ambient)
{
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return queue;
    }
    ++strip;
    prefetch(strip);
    SET_CLIP_LIGHT();
    direct_queue = queue;
    njDirectCalcPointLoop((NJS_DIRECT_VERTEX *)strip, count, &direct_point,
                          TRUE);
    return direct_queue;
}

#define WRITE_COLOR()                                                          \
    {                                                                          \
        *(Float *)--queue = strip->db * direct_light.blue;                     \
        *(Float *)--queue = strip->dg * direct_light.green;                    \
        *(Float *)--queue = strip->dr * direct_light.red;                      \
        *(Float *)--queue = strip->da;                                         \
    }

/* the strip's parameter with GLOBAL_UV16 clear. the clipped vertices take u and
 * v as two floats, whatever the strip's take */
#define WRITE_PARAMETER()                                                      \
    {                                                                          \
        *--queue = strip->TEXTUREADDR;                                         \
        *--queue = strip->TSPPARAM;                                            \
        *--queue = strip->ISPPARAM;                                            \
        *--queue = strip->GLOBALPARAM & ~GLOBAL_UV16;                          \
    }

/* the window of records the near clip reads: span records from clip_start.
 * start is the first record that changes side. span comes in as the bytes from
 * start to the record after the last that changes side. it leaves as the
 * window's records. the window stays within strip's own records */
#define FIND_CLIP_WINDOW(header_size)                                          \
    {                                                                          \
        vertex_address = (Uint32)start;                                        \
        first_vertex = (Uint32)strip;                                          \
        vertex_address -= CLIP_MARGIN * sizeof(*start);                        \
        first_vertex += (header_size);                                         \
        if (vertex_address < first_vertex) {                                   \
            vertex_address = first_vertex;                                     \
        }                                                                      \
        clip_start = vertex_address;                                           \
        span = (span >> RECORD_SHIFT) + (2 * CLIP_MARGIN);                     \
        available =                                                            \
            strip->num -                                                       \
            ((clip_start - (Uint32)strip - (header_size)) >> RECORD_SHIFT);    \
        if ((Uint32)span > available) {                                        \
            span = available;                                                  \
        }                                                                      \
    }

Uint32 *njDirectNearClip(NJS_DIRECT_GLOBAL *strip, Sint32 span, Uint32 *output,
                         NJS_DIRECT_VERTEX *start, Float light_x, Float light_y,
                         Float light_z, Float ambient)
{
    Uint32 *queue;
    Uint32 vertex_address;
    Uint32 first_vertex;
    Uint32 clip_start;
    Uint32 available;

    queue = output;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return queue;
    }
    if (strip->GP & GLOBAL_TEXTURE) {
        if (strip->GP & GLOBAL_OFFSET) {
            queue += PARAMETER_WORDS;
            WRITE_PARAMETER();
            prefetch(queue);
            queue += 2 * SQ_WORDS;
            *(Float *)--queue = strip->sb;
            *(Float *)--queue = strip->sg;
            *(Float *)--queue = strip->sr;
            *(Float *)--queue = strip->sa;
            WRITE_COLOR();
        } else {
            queue += SQ_WORDS;
            WRITE_COLOR();
            WRITE_PARAMETER();
        }
        prefetch(queue);
        queue += SQ_WORDS;
    }
    prefetch(start);
    SET_CLIP_LIGHT();
    direct_queue = queue;
    FIND_CLIP_WINDOW(sizeof(*strip));
    njDirectCalcPointLoop((NJS_DIRECT_VERTEX *)clip_start, span, &direct_point,
                          FALSE);
    return direct_queue;
}

Uint32 *njDirectD8NearClip(NJS_DIRECT_GLOBAL_D8 *strip, Sint32 span,
                           Uint32 *output, NJS_DIRECT_VERTEX_D8 *start)
{
    Uint32 *queue;
    Uint32 vertex_address;
    Uint32 first_vertex;
    Uint32 clip_start;
    Uint32 available;

    queue = output;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_DISABLE_NEAR_CLIP) {
        return queue;
    }
    if (strip->GP & GLOBAL_TEXTURE) {
        queue += PARAMETER_WORDS;
        WRITE_PARAMETER();
        prefetch(queue);
        queue += SQ_WORDS;
    }
    prefetch(start);
    direct_queue = queue;
    FIND_CLIP_WINDOW(sizeof(*strip));
    njDirectCalcPointLoopD8((NJS_DIRECT_VERTEX_D8 *)clip_start, span,
                            &direct_point);
    return direct_queue;
}

/* draw count strips from cursor, which each drawer moves past the strips it
 * draws. a model that crosses the near plane takes the drawers that clip */
#define DRAW_LIST(light_record, queue, count)                                  \
    {                                                                          \
        if (visibility == NO_NEAR_CLIP) {                                      \
            if (!head->mode) {                                                 \
                (queue) =                                                      \
                    njDirectDraw((light_record), &cursor, (queue), (count));   \
            } else {                                                           \
                (queue) =                                                      \
                    njDirectDrawD8((light_record), &cursor, (queue), (count)); \
            }                                                                  \
        } else {                                                               \
            if (!head->mode) {                                                 \
                (queue) = njDirectDrawClip((light_record), &cursor, (queue),   \
                                           (count));                           \
            } else {                                                           \
                (queue) = njDirectDrawD8Clip((light_record), &cursor, (queue), \
                                             (count));                         \
            }                                                                  \
        }                                                                      \
    }
void njDirectDrawObject(NJS_DIRECT_HEAD *model)
{
    void *cursor;
    Uint32 trans_area;
    Uint32 opaque_area;
    Sint32 visibility;
    Uint32 trans_base;
    Uint32 opaque_base;
    Uint32 *opaque_queue;
    Uint32 *trans_queue;
    Uint32 address;
    struct direct_lighting *light;
    Uint32 *qacr0;
    NJS_DIRECT_HEAD *head;

    head = model;
    njGetkmPointer();
    address = (Uint32)_nj_parameterkm_.opaque;
    opaque_base = address & ~SQ_OFFSET;
    opaque_area = (address >> QACR_AREA_SHIFT) & QACR_AREA;
    opaque_queue = SQ_ADDRESS(address);
    address = (Uint32)_nj_parameterkm_.trans;
    trans_base = address & ~SQ_OFFSET;
    trans_area = (address >> QACR_AREA_SHIFT) & QACR_AREA;
    /* @hack spelling: trans_queue's store queue address in two steps, where
     * SQ_ADDRESS(address) would do */
    trans_queue = (Uint32 *)(address & SQ_OFFSET);
    trans_queue = (Uint32 *)((Uint32)trans_queue | SQ_AREA);
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of the drawers' addresses in a block that
     * never runs */
    if (0) {
        /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
        if (njDirectDraw && njDirectDrawD8 && njDirectDrawClip) {
        }
    }
#endif
    light = &direct_light;
    /* @hack spelling: qacr0, QACR0's address as one below QACR1's, where
     * &QACR0 would do */
    qacr0 = &QACR1;
    --qacr0;
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of qacr0 in a block that never runs */
    if (0) {
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        if (qacr0 && qacr0 && qacr0) {
        }
    }
    /* @hack redundant: a test of address's two fields in a block that never
     * runs */
    if (0) {
        if ((address & ~SQ_OFFSET) && (address & QACR_AREA)) {
        }
    }
#endif
    do {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((visibility = njDirectClip((Float *)head, (Float *)&direct_clip)) !=
            0) {
            /* @hack spelling: first, a copy of head + 1, where cursor = head
             * + 1 would do */
            NJS_DIRECT_GLOBAL *first = (NJS_DIRECT_GLOBAL *)(head + 1);

            cursor = first;
            if (head->nOP) {
                *qacr0 = opaque_area;
                QACR1 = opaque_area;
                DRAW_LIST(light, opaque_queue, head->nOP);
            }
            if (head->nTR) {
                struct direct_lighting *trans_light;

                *qacr0 = trans_area;
                QACR1 = trans_area;
                /* @hack type: trans_light, light cast from its first
                 * field's address, where light itself would do */
                trans_light = (struct direct_lighting *)&light->direction;
                DRAW_LIST(trans_light, trans_queue, head->nTR);
            }
        }
        head = head->next;
    } while (head);
    _nj_parameterkm_.opaque =
        (Uint32 *)(((Uint32)opaque_queue & SQ_OFFSET) | opaque_base);
    _nj_parameterkm_.trans =
        (Uint32 *)(((Uint32)trans_queue & SQ_OFFSET) | trans_base);
    njSetkmPointer();
}

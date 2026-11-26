/* Dry Lagoon's water surfaces, each showing the step's frame of
 * lagoon_water_texlist */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "stg18/lagoon_util.h"
#include "stg18/water.h"
#include "stg18/water/text.inc"
#include "system/angle.h"
#include "system/game_state.h"

/* lagoon_water_draw_under's surfaces show kd_dl_mizu2_001 to 015, the
 * DRAW_UNDER_FRAMES textures of lagoon_water_texlist from 0 */
#define DRAW_UNDER_FRAMES 15
/* lagoon_water_object_0's polygon list opens with its NJD_CT_TID chunk.
 * the halfword at RING_TID_AT holds its texture's index */
#define RING_TID_AT 1

/* lagoon_water_object_0 has RING_VERTICES vertices. CORNERS corners come
 * first, then a ring of RING_POINTS about the point. the center comes last,
 * at the point */
#define CORNERS 4
#define RING_POINTS 6
#define RING_VERTICES (CORNERS + RING_POINTS + 1)
/* the center's first word, counted from the last corner's */
#define CENTER_WORD ((RING_POINTS + 1) * CHUNK_D8_WORDS)
/* the words of a struct chunk_vertex_d8 that hold x, z and the color */
#define X_WORD 0
#define Z_WORD 2
#define COLOR_WORD 3
/* the halfwords of lagoon_water_object_0's polygon list. its NJD_CT_TID
 * chunk comes first, then two NJD_CS_UVN chunks of two strips each. a strip
 * takes three halfwords for each vertex: its index, u and v */
#define TID_CHUNK 2 /* the NJD_CT_TID chunk */
#define STRIP_CHUNK_HEAD 3 /* a strip chunk's header, size and strip count */
#define CHUNK_PAD 1 /* the padding that ends the first strip chunk */
#define STRIP_HEAD 1 /* a strip's length */
#define FIRST_STRIP_VERTICES 9 /* the first strip's vertices */
#define STRIP_VERTICES 5 /* each other strip's vertices */
/* a texture spans TEXTURE_SPAN units of x and of z, and UV_PER_TEXTURE of u
 * and of v. u is 0 where x is -UV_OFFSET, and v is UV_TOP where z is
 * -UV_OFFSET */
#define TEXTURE_SPAN 54.0F
#define UV_PER_TEXTURE 256.0F
#define UV_OFFSET 270.0F
#define UV_TOP 255.0F
/* lagoon_water_object_0's r is its half-diagonal plus RADIUS_MARGIN */
#define RADIUS_MARGIN 50.0F

/* The animated lagoon surfaces and the deformable wake. */

static NJS_TEXNAME lagoon_water_texlist_names[30] = {
#include "stg18/water/lagoon_water_texlist_names.inc"
};

NJS_TEXLIST lagoon_water_texlist = {
#include "stg18/water/lagoon_water_texlist.inc"
};

static NJS_TEXNAME water_texture_set_2_names[2] = {
#include "stg18/water/water_texture_set_2_names.inc"
};

static NJS_TEXLIST water_texture_set_2 = {
#include "stg18/water/water_texture_set_2.inc"
};

static Sint16 lagoon_water_object_0_polygons[87] = {
#include "stg18/water/lagoon_water_object_0_polygons.inc"
};

static Sint32 lagoon_water_object_0_vertices[47] = {
#include "stg18/water/lagoon_water_object_0_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_0_model = {
#include "stg18/water/lagoon_water_object_0_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_0[1] = {
#include "stg18/water/lagoon_water_object_0.inc"
};

static NJS_TEXNAME water_texture_set_3_names[1] = {
#include "stg18/water/water_texture_set_3_names.inc"
};

static NJS_TEXLIST water_texture_set_3 = {
#include "stg18/water/water_texture_set_3.inc"
};

static Sint16 lagoon_water_object_1_polygons[45] = {
#include "stg18/water/lagoon_water_object_1_polygons.inc"
};

static Sint32 lagoon_water_object_1_vertices[45] = {
#include "stg18/water/lagoon_water_object_1_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_1_model = {
#include "stg18/water/lagoon_water_object_1_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_1[1] = {
#include "stg18/water/lagoon_water_object_1.inc"
};

static NJS_TEXNAME water_texture_set_4_names[1] = {
#include "stg18/water/water_texture_set_4_names.inc"
};

static NJS_TEXLIST water_texture_set_4 = {
#include "stg18/water/water_texture_set_4.inc"
};

static Sint16 lagoon_water_object_2_polygons[25] = {
#include "stg18/water/lagoon_water_object_2_polygons.inc"
};

static Sint32 lagoon_water_object_2_vertices[27] = {
#include "stg18/water/lagoon_water_object_2_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_2_model = {
#include "stg18/water/lagoon_water_object_2_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_2[1] = {
#include "stg18/water/lagoon_water_object_2.inc"
};

static NJS_TEXNAME water_texture_set_5_names[1] = {
#include "stg18/water/water_texture_set_5_names.inc"
};

static NJS_TEXLIST water_texture_set_5 = {
#include "stg18/water/water_texture_set_5.inc"
};

static Sint16 lagoon_water_object_3_polygons[45] = {
#include "stg18/water/lagoon_water_object_3_polygons.inc"
};

static Sint32 lagoon_water_object_3_vertices[45] = {
#include "stg18/water/lagoon_water_object_3_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_3_model = {
#include "stg18/water/lagoon_water_object_3_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_3[1] = {
#include "stg18/water/lagoon_water_object_3.inc"
};

static NJS_TEXNAME water_texture_set_6_names[1] = {
#include "stg18/water/water_texture_set_6_names.inc"
};

static NJS_TEXLIST water_texture_set_6 = {
#include "stg18/water/water_texture_set_6.inc"
};

static Sint16 lagoon_water_object_4_polygons[95] = {
#include "stg18/water/lagoon_water_object_4_polygons.inc"
};

static Sint32 lagoon_water_object_4_vertices[81] = {
#include "stg18/water/lagoon_water_object_4_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_4_model = {
#include "stg18/water/lagoon_water_object_4_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_4[1] = {
#include "stg18/water/lagoon_water_object_4.inc"
};

static NJS_TEXNAME water_texture_set_7_names[1] = {
#include "stg18/water/water_texture_set_7_names.inc"
};

static NJS_TEXLIST water_texture_set_7 = {
#include "stg18/water/water_texture_set_7.inc"
};

static Sint16 lagoon_water_object_5_polygons[63] = {
#include "stg18/water/lagoon_water_object_5_polygons.inc"
};

static Sint32 lagoon_water_object_5_vertices[75] = {
#include "stg18/water/lagoon_water_object_5_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_5_model = {
#include "stg18/water/lagoon_water_object_5_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_5[1] = {
#include "stg18/water/lagoon_water_object_5.inc"
};

static NJS_TEXNAME water_texture_set_8_names[1] = {
#include "stg18/water/water_texture_set_8_names.inc"
};

static NJS_TEXLIST water_texture_set_8 = {
#include "stg18/water/water_texture_set_8.inc"
};

static Sint16 lagoon_water_object_6_polygons[55] = {
#include "stg18/water/lagoon_water_object_6_polygons.inc"
};

static Sint32 lagoon_water_object_6_vertices[51] = {
#include "stg18/water/lagoon_water_object_6_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_6_model = {
#include "stg18/water/lagoon_water_object_6_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_6[1] = {
#include "stg18/water/lagoon_water_object_6.inc"
};

static NJS_TEXNAME water_texture_set_9_names[1] = {
#include "stg18/water/water_texture_set_9_names.inc"
};

static NJS_TEXLIST water_texture_set_9 = {
#include "stg18/water/water_texture_set_9.inc"
};

static Sint16 lagoon_water_object_7_polygons[89] = {
#include "stg18/water/lagoon_water_object_7_polygons.inc"
};

static Sint32 lagoon_water_object_7_vertices[67] = {
#include "stg18/water/lagoon_water_object_7_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_7_model = {
#include "stg18/water/lagoon_water_object_7_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_7[1] = {
#include "stg18/water/lagoon_water_object_7.inc"
};

static NJS_TEXNAME water_texture_set_10_names[1] = {
#include "stg18/water/water_texture_set_10_names.inc"
};

static NJS_TEXLIST water_texture_set_10 = {
#include "stg18/water/water_texture_set_10.inc"
};

static Sint16 lagoon_water_object_8_polygons[69] = {
#include "stg18/water/lagoon_water_object_8_polygons.inc"
};

static Sint32 lagoon_water_object_8_vertices[67] = {
#include "stg18/water/lagoon_water_object_8_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_8_model = {
#include "stg18/water/lagoon_water_object_8_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_8[1] = {
#include "stg18/water/lagoon_water_object_8.inc"
};

static NJS_TEXNAME water_texture_set_11_names[1] = {
#include "stg18/water/water_texture_set_11_names.inc"
};

static NJS_TEXLIST water_texture_set_11 = {
#include "stg18/water/water_texture_set_11.inc"
};

static Sint16 lagoon_water_object_9_polygons[63] = {
#include "stg18/water/lagoon_water_object_9_polygons.inc"
};

static Sint32 lagoon_water_object_9_vertices[51] = {
#include "stg18/water/lagoon_water_object_9_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_9_model = {
#include "stg18/water/lagoon_water_object_9_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_9[1] = {
#include "stg18/water/lagoon_water_object_9.inc"
};

static NJS_TEXNAME water_texture_set_12_names[1] = {
#include "stg18/water/water_texture_set_12_names.inc"
};

static NJS_TEXLIST water_texture_set_12 = {
#include "stg18/water/water_texture_set_12.inc"
};

static Sint16 lagoon_water_object_10_polygons[43] = {
#include "stg18/water/lagoon_water_object_10_polygons.inc"
};

static Sint32 lagoon_water_object_10_vertices[35] = {
#include "stg18/water/lagoon_water_object_10_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_10_model = {
#include "stg18/water/lagoon_water_object_10_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_10[1] = {
#include "stg18/water/lagoon_water_object_10.inc"
};

static NJS_TEXNAME water_texture_set_13_names[1] = {
#include "stg18/water/water_texture_set_13_names.inc"
};

static NJS_TEXLIST water_texture_set_13 = {
#include "stg18/water/water_texture_set_13.inc"
};

static Sint16 lagoon_water_object_11_polygons[115] = {
#include "stg18/water/lagoon_water_object_11_polygons.inc"
};

static Sint32 lagoon_water_object_11_vertices[107] = {
#include "stg18/water/lagoon_water_object_11_vertices.inc"
};

static NJS_CNK_MODEL lagoon_water_object_11_model = {
#include "stg18/water/lagoon_water_object_11_model.inc"
};

static NJS_CNK_OBJECT lagoon_water_object_11[1] = {
#include "stg18/water/lagoon_water_object_11.inc"
};

void lagoon_water_draw(void)
{
    /* @hack spelling: tid, a local for each halfword's new value, built in
     * two statements, where one assignment to the halfword would do */
    Sint16 tid;

    if (game_state.land_block_mask[0] & SOUTH_WEST_BLOCKS) {
        tid = lagoon_water_object_1->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | ((game_state.steps % LAGOON_WATER_FRAMES) +
                                      LAGOON_WATER_FIRST_FRAME);
        lagoon_water_object_1->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 0.0F, -70.0F);
        njCnkEasyDrawModel(lagoon_water_object_1->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & HALL_BLOCKS) {
        tid = lagoon_water_object_2->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | ((game_state.steps % LAGOON_WATER_FRAMES) +
                                      LAGOON_WATER_FIRST_FRAME);
        lagoon_water_object_2->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 180.0F, 1180.0F);
        njCnkEasyDrawModel(lagoon_water_object_2->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & (LOW_BLOCKS | HALL_BLOCKS)) {
        tid = lagoon_water_object_3->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | ((game_state.steps % LAGOON_WATER_FRAMES) +
                                      LAGOON_WATER_FIRST_FRAME);
        lagoon_water_object_3->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 10.0F, 180.0F, 970.0F);
        njCnkEasyDrawModel(lagoon_water_object_3->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] &
        (MIDDLE_BLOCKS | WEST_BLOCKS | EAST_BLOCKS)) {
        tid = lagoon_water_object_5->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | ((game_state.steps % LAGOON_WATER_FRAMES) +
                                      LAGOON_WATER_FIRST_FRAME);
        lagoon_water_object_5->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 180.0F, 1500.0F);
        njCnkEasyDrawModel(lagoon_water_object_5->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & (MIDDLE_BLOCKS | WEST_BLOCKS)) {
        tid = lagoon_water_object_4->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | ((game_state.steps % LAGOON_WATER_FRAMES) +
                                      LAGOON_WATER_FIRST_FRAME);
        lagoon_water_object_4->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, -460.0F, 280.0F, 1450.0F);
        njCnkEasyDrawModel(lagoon_water_object_4->model);
        njPopMatrixEx();
    }
}

void lagoon_water_draw_under(void)
{
    /* @hack spelling: tid, a local for each halfword's new value, built in
     * two statements, where one assignment to the halfword would do */
    Sint16 tid;

    if (game_state.land_block_mask[0] & SOUTH_WEST_BLOCKS) {
        tid = lagoon_water_object_6->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_6->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 0.0F, -70.0F);
        njCnkEasyDrawModel(lagoon_water_object_6->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & HALL_BLOCKS) {
        tid = lagoon_water_object_10->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_10->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 125.0F, 1220.0F);
        njCnkEasyDrawModel(lagoon_water_object_10->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & (MIDDLE_BLOCKS | LOW_BLOCKS)) {
        tid = lagoon_water_object_7->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_7->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 150.0F, 1250.0F);
        njCnkEasyDrawModel(lagoon_water_object_7->model);
        njPopMatrixEx();
        tid = lagoon_water_object_8->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_8->model->plist[LAGOON_WATER_TID_AT] = tid;
        njPushMatrixEx();
        njTranslate(NULL, 200.0F, 130.0F, 1450.0F);
        njCnkEasyDrawModel(lagoon_water_object_8->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & LOW_BLOCKS) {
        tid = lagoon_water_object_9->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_9->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 13.0F, 0.0F, 970.0F);
        njCnkEasyDrawModel(lagoon_water_object_9->model);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & EAST_BLOCKS) {
        tid = lagoon_water_object_11->model->plist[LAGOON_WATER_TID_AT];
        tid = (tid & NJD_FFM_MASK) | (game_state.steps % DRAW_UNDER_FRAMES);
        lagoon_water_object_11->model->plist[LAGOON_WATER_TID_AT] = tid;
        njSetTexture(&lagoon_water_texlist);
        njPushMatrixEx();
        njCnkEasyDrawModel(lagoon_water_object_11->model);
        njPopMatrixEx();
    }
}

static Float u_scale = UV_PER_TEXTURE / TEXTURE_SPAN;
static Float v_scale = UV_PER_TEXTURE / TEXTURE_SPAN;
/* the center's color. the strips inside the ring are translucent, so its
 * alpha of 0x60 thins the water toward the point */
static Uint32 ring_center_color = 0x60b2b2b2;
static Sint32 dat_8c5c70b0 = 0x2aab;
/* the ring's radius is ring_radius, less ring_shrink for each unit the point
 * lies above or below the surface. it is never below 0 */
static Float ring_radius = 25.0F;
static Float ring_shrink = 0.5F;
/* the ring's vertices in the order of their angles. each is an index from
 * the last corner, vertex CORNERS - 1. 1 to RING_POINTS are the ring's */
static Sint32 ring_order[RING_POINTS] = {4, 6, 1, 5, 3, 2};

void lagoon_water_draw_around(Float x, Float y, Float z, Float half_x,
                              Float half_z, NJS_POINT3 *point)
{
    NJS_CNK_OBJECT *object;
    struct chunk_vertex_d8 *vertex;
    Sint16 *strip;
    Float min_x;
    Float max_x;
    Float min_z;
    Float max_z;
    Float radius;
    Float px;
    Float pz;
    Angle ang;
    int i;
    int index;
    Sint16 tid;

    if (point == NULL) {
        return;
    }
    /* @hack redundant: a block no path reaches, which squares half_z */
    if (0) {
        half_z = half_z * half_z;
    }
    object = lagoon_water_object_0;
    /* @hack spelling: tid, a local for the halfword's new value, built in
     * three statements, where one assignment to the halfword would do */
    tid = object->model->plist[RING_TID_AT];
    tid &= NJD_FFM_MASK;
    tid |= (game_state.steps % LAGOON_WATER_FRAMES) + LAGOON_WATER_FIRST_FRAME;
    object->model->plist[RING_TID_AT] = tid;
    vertex =
        (struct chunk_vertex_d8 *)(object->model->vlist + CHUNK_HEAD_WORDS);
    /* @hack spelling: the bounds set max_x, min_z, max_z, then min_x, not
     * min_x first */
    max_x = half_x;
    min_z = -half_z;
    max_z = half_z;
    min_x = -half_x;
    /* @hack spelling: a block of its own for offset, and another for the
     * arrays, where all declared with the other locals would do */
    {
        NJS_POINT3 offset;

        offset = *point;
        offset.x -= x;
        offset.y -= y;
        offset.z -= z;
        radius = ring_radius - (ring_shrink * njAbs(offset.y));
        if (radius <= 0.0F) {
            radius = 0.0F;
        }
        /* @hack spelling: the corners stored from half_x, half_z and their
         * negations as well as from the bounds, where the bounds alone would
         * do */
        vertex->point.x = -half_x;
        vertex->point.z = half_z;
        vertex++;
        vertex->point.x = min_x;
        vertex->point.z = -half_z;
        vertex++;
        vertex->point.x = half_x;
        vertex->point.z = half_z;
        vertex++;
        vertex->point.x = half_x;
        vertex->point.z = min_z;
        ang = QUARTER_TURN;
        for (i = 0; i < RING_POINTS; i++) {
            px = (radius * njSin(ang)) + offset.x;
            pz = (radius * njCos(ang)) + offset.z;
            if (px > max_x) {
                px = max_x;
            }
            if (min_x > px) {
                px = min_x;
            }
            if (pz > max_z) {
                pz = max_z;
            }
            if (min_z > pz) {
                pz = min_z;
            }
            index = ring_order[i];
            vertex[index].point.x = px;
            vertex[index].point.z = pz;
            ang -= FULL_TURN / RING_POINTS;
        }
        /* @hack spelling: the point's z taken before its x, not in the
         * members' order */
        pz = offset.z;
        px = offset.x;
        if (px > max_x) {
            px = max_x;
        }
        if (min_x > px) {
            px = min_x;
        }
        if (pz > max_z) {
            pz = max_z;
        }
        if (min_z > pz) {
            pz = min_z;
        }
        /* @hack type: the center stored through Float and Uint32 views of
         * vertex, which points at the last corner, where
         * vertex[RING_POINTS + 1] would do */
        ((Float *)vertex)[CENTER_WORD + X_WORD] = px;
        ((Float *)vertex)[CENTER_WORD + Z_WORD] = pz;
        ((Uint32 *)vertex)[CENTER_WORD + COLOR_WORD] = ring_center_color;
    }
    {
        struct chunk_vertex_d8 *cursor;
        NJS_POINT3 points[RING_VERTICES];
        NJS_TEX uv[RING_VERTICES];
        NJS_TEX *tex;
        NJS_POINT3 *pt;

        /* @hack spelling: object set again inside the expression, as the
         * address of the array's first element, where object read as it is
         * would do */
        vertex = (struct chunk_vertex_d8 *)((object = &lagoon_water_object_0[0])
                                                ->model->vlist +
                                            CHUNK_HEAD_WORDS);
        strip = object->model->plist;
        /* @hack redundant: a block no path reaches, which sets and steps
         * vertex */
        if (0) {
            vertex = cursor;
            vertex = (void *)strip;
            vertex++;
            vertex++;
            vertex++;
            vertex++;
            vertex++;
            vertex++;
        }
        cursor = vertex;
        for (i = 0; i < RING_VERTICES; i++, cursor++) {
            points[i].x = cursor->point.x;
            points[i].z = cursor->point.z;
        }
        /* @hack spelling: pt, a pointer that walks points, where points[i]
         * would do */
        pt = points;
        /* @hack redundant: a block no path reaches, which steps pt */
        if (0) {
            pt++;
            pt++;
            pt++;
        }
        for (i = 0; i < RING_VERTICES; i++) {
            /* @hack spelling: tex, a local for &uv[i], where uv[i] would do */
            tex = &uv[i];
            tex->u = (pt->x + UV_OFFSET) * u_scale;
            tex->v = UV_TOP - ((pt->z + UV_OFFSET) * v_scale);
            pt++;
        }
        /* @hack redundant: a block no path reaches, which steps strip */
        if (0) {
            strip++;
            strip++;
            strip++;
            strip++;
            strip++;
            strip++;
            strip++;
        }
        strip += TID_CHUNK + STRIP_CHUNK_HEAD + STRIP_HEAD;
        for (i = 0; i < FIRST_STRIP_VERTICES; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
        strip += STRIP_HEAD;
        for (i = 0; i < STRIP_VERTICES; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
        strip += CHUNK_PAD + STRIP_CHUNK_HEAD + STRIP_HEAD;
        for (i = 0; i < STRIP_VERTICES; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
        strip += STRIP_HEAD;
        for (i = 0; i < STRIP_VERTICES; i++) {
            index = *strip++;
            *strip++ = uv[index].u;
            *strip++ = uv[index].v;
        }
    }
    /* @hack spelling: the radius and the center set through
     * lagoon_water_object_0, where object would do */
    lagoon_water_object_0->model->r =
        njSqrt((half_x * half_x) + (half_z * half_z)) + RADIUS_MARGIN;
    lagoon_water_object_0->model->center.x = 0.0F;
    lagoon_water_object_0->model->center.y = 0.0F;
    lagoon_water_object_0->model->center.z = 0.0F;
    njPushMatrixEx();
    njTranslate(NULL, x, y, z);
    njSetTexture(&lagoon_water_texlist);
    njCnkEasyDrawModel(lagoon_water_object_0->model);
    njPopMatrixEx();
}

/* helpers the stages share: chunk model draws, turns about y, dust, a camera
 * shake and a pad's vibration */
#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/stage_util.h"
#include "shared/vertex_colors.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the ranges of dust_behind's random turns about y and about x, in degrees */
#define TURN_Y_MIN 120.0F
#define TURN_Y_SPREAD 120.0F
#define TILT_MAX 60.0F

/* the size, in words, of the stack buffer that model_draw_tinted_stack copies
 * a chunk into */
#define STACK_COPY_WORDS 0x300
/* the words vlist_copy_tinted wants in dst beyond a chunk's size. the copy
 * fills only 2 of them. one is the chunk's first word, which the size leaves
 * out, and one the word after the chunk */
#define COPY_SPARE_WORDS 4

/* the camera camera_shake_at shakes, and the player it measures from */
#define SHAKE_CAMERA 0
#define SHAKE_PLAYER 0

/* a VibSlightExec task keeps vib_slight_create's parameters in its work, and
 * in STEPS the steps it has run */
#define PLAYER(work) ((work)->tick)
#define POWER(work) ((work)->timer)
#define FREQ(work) ((work)->ang.x)
#define INTERVAL(work) ((work)->ang.y)
#define PULSES(work) ((work)->ang.z)
#define STEPS(task) ((task)->unk_3c.l)
/* task_create's level for a VibSlightExec task */
#define VIB_SLIGHT_LEVEL 2

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* no header declares these, and no image that links this source calls them.
 * they stay external: the image holds their code, and SHC leaves out a static
 * function nothing calls */
/* draw model as model_draw_scaled does, but with njCnkSimpleDrawModel */
void model_draw_simple_scaled(NJS_CNK_MODEL *model, Float scale);
/* draw model as model_draw_scaled does, but with model_draw_tinted */
void model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color, Float scale);
/* draw model with njCnkEasyDrawModel, from a tinted copy of its first vertex
 * chunk on the stack. vlist_copy_tinted makes the copy. the chunk must be
 * NJD_CV_D8, unchecked */
void model_draw_tinted_stack(NJS_CNK_MODEL *model, Uint32 color);
/* tint the vertex colors of model's first chunk in place, as
 * vlist_copy_tinted tints its copy. the chunk must be NJD_CV_D8, unchecked.
 * where the word after the chunk isn't NJD_CE, the call raises a trap with
 * trapa(0) */
void model_tint(NJS_CNK_MODEL *model, Uint32 color);
/* shake camera 0 with camera_shake for steps. the shake's size is size
 * divided by the distance from pos to player 0's pos. a distance of 1 or less
 * leaves size whole. the call does nothing where camera 0's shake_steps is
 * above 0 and its shake_size is above that size. player_work[0] must not be
 * null, unchecked */
void camera_shake_at(NJS_POINT3 *pos, int steps, Float size);
/* draw model with njCnkSimpleDrawModel. each material's diffuse color is
 * taken from the constant material (alpha, red, green, blue). the call saves
 * and puts back _nj_control_3d_flag_ and njSetConstantAttr's masks as
 * model_draw_material_additive does. it leaves the constant material at 0 */
void model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                         Float green, Float blue);
/* NOLINTEND(misc-use-internal-linkage) */

void polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = njSin(ang) * radius;
    *z = njCos(ang) * radius;
}

void dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius, Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    struct angles ang;

    offset.x = 0.0F;
    offset.y = 0.0F;
    offset.z = radius;
    njPushMatrixEx();
    njRotateY(NULL, (Angle)((((Float)rand() * (TURN_Y_SPREAD * RAND_SCALE)) +
                             TURN_Y_MIN) *
                            ANGLE_PER_DEGREE));
    njRotateX(NULL, (Angle)((Float)rand() *
                            (TILT_MAX * ANGLE_PER_DEGREE * RAND_SCALE)));
    current_matrix_angles_xyz(&ang.x, &ang.y, &ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateXYZ(NULL, ang.x, ang.y, ang.z);
    njCalcPoint(NULL, &offset, &point);
    dust_create(&point, spd, scale);
    njPopMatrixEx();
}

void model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    njCnkEasyDrawModel(model);
    *radius = saved;
}

void model_draw_simple_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(model);
    *radius = saved;
}

void model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                NJS_VECTOR *scale)
{
    Float largest;
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius;
    Float saved;

    largest = scale->x > scale->y ? scale->x : scale->y;
    largest = largest > scale->z ? largest : scale->z;
    radius = &model->r;
    saved = *radius;
    *radius *= largest;
    njScaleEx(scale);
    model_draw_tinted(model, color);
    *radius = saved;
}

void point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    Float sine;
    Float cosine;

    sine = njSin(ang);
    cosine = njCos(ang);
    point->x = (x * cosine) + (z * sine);
    point->z = (z * cosine) - (x * sine);
}

/* copy src's first chunk, an NJD_CV_D8 chunk, to dst, each vertex's color
 * multiplied by color with color_multiply. then copy the word after the chunk.
 * dst has room for limit words. the call never returns where the chunk's size
 * passes limit - COPY_SPARE_WORDS. nor does it where the word after the chunk
 * isn't NJD_CE */
static void vlist_copy_tinted(Sint32 *dst, Sint32 *src, Uint32 color, int limit)
{
    Sint32 word;
    int size;
    int count;

    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    size = (Uint16)(word >> 16);
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    word = *dst++ = *src++;
    count = (Uint16)(word >> 16);
    /* NOLINTNEXTLINE(bugprone-infinite-loop) */
    while (limit < size + COPY_SPARE_WORDS) {
    }
    /* @hack type: dst and src stepped word by word, where
     * struct chunk_vertex_d8 pointers stepped from vertex to vertex would do */
    while (count--) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = color_multiply(*src++, color);
    }
    word = *src;
    *dst = word;
    /* NOLINTNEXTLINE(bugprone-infinite-loop) */
    while (word != NJD_CE) {
    }
}

void model_draw_tinted_stack(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 copy[STACK_COPY_WORDS];
    Sint32 *vlist;

    vlist = model->vlist;
    vlist_copy_tinted(copy, vlist, color, STACK_COPY_WORDS);
    model->vlist = copy;
    njCnkEasyDrawModel(model);
    model->vlist = vlist;
}

void model_tint(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 *word;
    /* @hack spelling: value, a local for the chunk's second word and for the
     * word after the chunk, where each word read in place would do */
    Sint32 value;
    int count;

    word = model->vlist + 1;
    value = *word++;
    count = (Uint16)(value >> 16);
    /* @hack type: word stepped to each vertex's color, where a
     * struct chunk_vertex_d8 pointer stepped from vertex to vertex would do */
    while (count--) {
        word += CHUNK_V_WORDS;
        *word = color_multiply(*word, color);
        word++;
    }
    value = *word;
    if (value != NJD_CE) {
        trapa(0);
    }
}

void camera_shake_at(NJS_POINT3 *pos, int steps, Float size)
{
    Float distance;

    distance = njDistanceP2P(pos, &player_work[SHAKE_PLAYER]->pos);
    if (distance > 1.0F) {
        size /= distance;
    }
    /* @hack spelling: the shake in each branch, where one test with || would
     * do */
    if (camera_stacks[SHAKE_CAMERA].shake_steps > 0) {
        if (size >= camera_stacks[SHAKE_CAMERA].shake_size) {
            camera_shake(SHAKE_CAMERA, steps, size);
        }
    } else {
        camera_shake(SHAKE_CAMERA, steps, size);
    }
}

void model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                         Float green, Float blue)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    constant_material_set(alpha, red, green, blue);
    njCnkSimpleDrawModel(model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

void model_draw_material_additive(NJS_CNK_MODEL *model, Float alpha, Float red,
                                  Float green, Float blue)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
    constant_material_set(alpha, red, green, blue);
    njCnkSimpleDrawModel(model);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

void model_draw_tinted_additive(NJS_CNK_MODEL *model, Uint32 color)
{
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
    model_draw_tinted(model, color);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

void shape_motion_draw_tinted(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              NJS_MOTION *shape, Float frame, Uint32 color)
{
    Sint32 *vlist = object->model->vlist;
    /* @hack spelling: copy, a local for WORK_AREA, where WORK_AREA passed and
     * stored in place would do */
    Sint32 *copy;

    if (color != COLOR_NONE && (Uint16)*vlist == NJD_CV_D8) {
        copy = (Sint32 *)WORK_AREA;
        vlist_copy_shaded(copy, vlist, color, VLIST_UNREAD_MASK);
        object->model->vlist = copy;
        njCnkEasyDrawShapeMotion(object, motion, shape, frame);
        object->model->vlist = vlist;
    } else {
        njCnkEasyDrawShapeMotion(object, motion, shape, frame);
    }
}

Sint32 world_size_class_flags(Float size)
{
    /* @hack spelling: the tests chained with else, where each return ending
     * its branch would do */
    if (size > WORLD_SIZE_CLASS_1_MAX) {
        return WORLD_SIZE_CLASS(0);
    } else if (size > WORLD_SIZE_CLASS_2_MAX) {
        return WORLD_SIZE_CLASS(1);
    } else if (size > WORLD_SIZE_CLASS_3_MAX) {
        return WORLD_SIZE_CLASS(2);
    } else {
        return WORLD_SIZE_CLASS(3);
    }
}

void vlist_set_colors(Sint32 *vlist, int count, Uint32 color)
{
    /* @hack type: vlist stepped from color to color, where a
     * struct chunk_vertex_d8 pointer stepped from vertex to vertex would do */
    vlist += CHUNK_HEAD_WORDS + CHUNK_V_WORDS;
    while (count--) {
        *vlist = color;
        vlist += CHUNK_D8_WORDS;
    }
}

void modifier_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkModDrawModel(model);
    *radius = saved;
}

static void vib_slight_step(struct task *task)
{
    if (STEPS(task) % INTERVAL(task->work) == 0) {
        vibration_start(PLAYER(task->work), POWER(task->work), FREQ(task->work),
                        VIBRATION_UNTIMED);
    }
    /* @hack spelling: the increment inside the test, where a statement of its
     * own would do */
    if (++STEPS(task) / INTERVAL(task->work) == PULSES(task->work)) {
        task_kill(task);
    }
}

void vib_slight_create(int player, int power, int freq, int interval, int count)
{
    struct task *task;

    task = task_create(TASK_WORK, VIB_SLIGHT_LEVEL, vib_slight_step,
                       "VibSlightExec");
    if (task != NULL) {
        PLAYER(task->work) = player;
        POWER(task->work) = power;
        FREQ(task->work) = freq;
        PULSES(task->work) = count;
        /* @hack redundant: the steps set to 0, which task_create gave them
         * already */
        STEPS(task) = 0;
        INTERVAL(task->work) = interval;
        /* @hack spelling: the work's interval read back for the test, where
         * interval tested would do */
        if (INTERVAL(task->work) == 0) {
            INTERVAL(task->work) = 1;
        }
    }
}

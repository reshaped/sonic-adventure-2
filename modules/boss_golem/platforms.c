/* platforms.h's platforms A, B and C, with the modes and the draw that
 * head_parts.c's platforms share. golem_create creates them, and no object list
 * names them */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_golem/attachment_art.h"
#include "boss_golem/golem.h"
#include "boss_golem/platform_collision.h"
#include "boss_golem/platforms.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/golem.h"
#include "shared/golem_camera.h"
#include "shared/golem_util.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

/* matrix_set reads a matrix in pairs of floats, which needs the matrix on an
 * eight-byte boundary */
#ifdef NONMATCHING
/* on the Dreamcast, this side's NJS_MATRIX type gives the boundary, as
 * nj_render_types.h says */
#else
#pragma aligndata8(golem_platform_bases)
#endif
struct task *golem_platform_tasks[GOLEM_PLATFORMS];
static NJS_MATRIX golem_platform_bases[GOLEM_PLATFORMS];
static NJS_MATRIX golem_platform_matrices[GOLEM_PLATFORMS];

#define PLATFORM_WORLD_FLAGS (WORLD_TURNED | WORLD_MOVING | WORLD_SOLID)
#define SLIDE_SOUND 7
#define SLIDE_SOUND_STEPS 15

/* a platform's glow rises from 0 to 1 over the first GLOW_RISE_STEPS of each
 * GLOW_STEPS of game_state.steps. it falls back to 0 over the rest */
#define GLOW_STEPS 60
#define GLOW_RISE_STEPS 5
#define GLOW_LAST (GLOW_STEPS - 1)

/* the glow draws the model's child in a constant material. it adds the
 * child's colors, times the glow, to what lies behind it */
#define GLOW_CONTROL (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)

static void platform_run(struct task *task);
static void platform_draw_model(struct task *task, NJS_CNK_OBJECT *object,
                                Float x, Float z, Float glow);
static void platform_return_start(struct task *task);
static void platform_return_step(struct task *task);
static void platform_wait_start(struct task *task);
static void platform_wait_step(struct task *task);
static void platform_retract_start(struct task *task);
static void platform_retract_step(struct task *task);
static void platform_in_start(struct task *task);
static void platform_in_step(struct task *task);

/* clang-format off */
/* the same shape as head_parts.c's target_shape */
static const struct collision_shape unused_target_shape = {
    0, SHAPE_SPHERE, 0, SHAPE_HURTS_PLAYERS, SHAPE_UNK_2000 | SHAPE_HURTABLE,
    {0.0F, 0.0F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

static void (*starts_by_mode[])(struct task *task) = {
    platform_return_start, platform_wait_start, platform_retract_start,
    platform_in_start};
static void (*steps_by_mode[])(struct task *task) = {
    platform_return_step, platform_wait_step, platform_retract_step,
    platform_in_step};
static NJS_CNK_OBJECT *platform_models[GOLEM_PLATFORMS] = {
#include "boss_golem/platforms/platform_models.inc"
};
static NJS_OBJECT *collision_models[GOLEM_PLATFORMS] = {
#include "boss_golem/platforms/collision_models.inc"
};
/* each platform's range, which picks its object's size class and is the range
 * golem_platforms_place gives golem_sensor_place */
static Float ranges[GOLEM_PLATFORMS] = {190.0F, 158.8005F, 149.9043F};
/* golem_platform_target_pos's offset from the child, up and along z */
static Float target_y = 0.0F;
static Float target_z = -5.0F;
/* each platform's home. A slides in to in_z_a, and B and C to 0 */
static Float home_z_a = -40.0F;
static Float home_z_b = -70.0F;
static Float home_z_c = -100.0F;
static Float in_z_a = -10.0F;
static Float return_rate = 0.005F;
static Float retract_rate = -0.0005F;
static Float retract_base = 0.004F;
static Sint32 slide_volume = 15;
static Sint32 in_steps = 180;

#ifdef NONMATCHING
/* this side returns whether it made every platform, so that golem_create can
 * destroy an unfinished golem */
Bool golem_platforms_create(struct task *golem_task)
#else
void golem_platforms_create(struct task *golem_task)
#endif
{
    struct task *platforms[GOLEM_PLATFORMS];
    /* @hack redundant: twelve bytes of the frame that no code touches */
    NJS_POINT3 unused;
    NJS_OBJECT *object;
    int i;
    Sint32 flags;

    for (i = 0; i < GOLEM_PLATFORMS; i++) {
        object = world_pool_alloc();
        /* @bug memory: writes through a null object where world_pool_alloc has
         * none left to lend. golem_platform_destroy tests the object at
         * 0x8c50346e, but the copy of the model writes through it at
         * 0x8c5032c0 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns FALSE where the
         * pool has no object */
        if (object == NULL) {
            return FALSE;
        }
#endif
        platforms[i] = task_create_child(TASK_WORK | TASK_FORCE,
                                         golem_platform_step, golem_task);
#ifdef NONMATCHING
        /* this side gives the object back and returns FALSE where
         * task_create_child fails. the matching side writes through the null */
        if (platforms[i] == NULL) {
            world_pool_free(object);
            return FALSE;
        }
#endif
        platforms[i]->destructor = golem_platform_destroy;
        platforms[i]->draw = golem_platform_draw;
        platforms[i]->extra.p = syMalloc(sizeof(struct golem_platform));
#ifdef NONMATCHING
        /* this side gives the object back and returns FALSE where the heap has
         * no platform record. the matching side writes through the null */
        if (platforms[i]->extra.p == NULL) {
            world_pool_free(object);
            return FALSE;
        }
#endif
        GOLEM_PLATFORM(platforms[i])->index = i;
        GOLEM_PLATFORM(platforms[i])->sensor = GOLEM_SENSOR_LOW;
        GOLEM_PLATFORM(platforms[i])->retract_rate = retract_rate;
        GOLEM_PLATFORM(platforms[i])->retract_base = retract_base;
        GOLEM_PLATFORM(platforms[i])->return_rate = return_rate;
        GOLEM_PLATFORM(platforms[i])->glows = FALSE;
        GOLEM_PLATFORM(platforms[i])->target_down_steps = 0;
        GOLEM_PLATFORM(platforms[i])->target_down = FALSE;
        golem_platform_set_mode(platforms[i], GOLEM_PLATFORM_MODE_WAIT);
        golem_platform_tasks[i] = platforms[i];
        /* @hack spelling: the task read again from golem_platform_tasks,
         * where platforms[i] would do */
        golem_platform_tasks[i]->motion.p = object;
        *object = *collision_models[i];
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
        object->ang[0] = object->ang[1] = object->ang[2] = 0;
        object->pos[0] = object->pos[1] = object->pos[2] = 0.0F;
        flags = PLATFORM_WORLD_FLAGS;
        flags |= golem_size_class_flags(ranges[i]);
        world_add(flags, platforms[i], object);
    }
    platforms[0]->work->id = GOLEM_PLATFORM(platforms[0])->id =
        WORK_ID_GOLEM_PLATFORM_A;
    platforms[1]->work->id = GOLEM_PLATFORM(platforms[1])->id =
        WORK_ID_GOLEM_PLATFORM_B;
    platforms[2]->work->id = GOLEM_PLATFORM(platforms[2])->id =
        WORK_ID_GOLEM_PLATFORM_C;
    GOLEM_PLATFORM(platforms[0])->home_z = GOLEM_PLATFORM(platforms[0])->z =
        home_z_a;
    GOLEM_PLATFORM(platforms[1])->home_z = GOLEM_PLATFORM(platforms[1])->z =
        home_z_b;
    GOLEM_PLATFORM(platforms[2])->home_z = GOLEM_PLATFORM(platforms[2])->z =
        home_z_c;
    GOLEM_PLATFORM(platforms[0])->home_x = GOLEM_PLATFORM(platforms[0])->x =
        GOLEM_PLATFORM(platforms[1])->home_x = GOLEM_PLATFORM(platforms[1])->x =
            GOLEM_PLATFORM(platforms[2])->home_x =
                GOLEM_PLATFORM(platforms[2])->x = 0.0F;
    GOLEM_PLATFORM(platforms[0])->in_z = in_z_a;
    GOLEM_PLATFORM(platforms[1])->in_z = GOLEM_PLATFORM(platforms[2])->in_z =
        0.0F;
    GOLEM_PLATFORM(platforms[0])->object = &golem_platform_a_object;
    GOLEM_PLATFORM(platforms[0])->texlist = &golem_platform_a_texlist;
    GOLEM_PLATFORM(platforms[1])->object = &golem_platform_b_object;
    GOLEM_PLATFORM(platforms[1])->texlist = &golem_platform_b_texlist;
    GOLEM_PLATFORM(platforms[2])->object = &golem_platform_c_object;
    GOLEM_PLATFORM(platforms[2])->texlist = &golem_platform_c_texlist;
#ifdef NONMATCHING
    /* this side returns that it made every platform */
    return TRUE;
#endif
}

void golem_platform_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

void golem_platform_draw(struct task *task)
{
    int phase = game_state.steps % GLOW_STEPS;
    Float glow;

    if (phase < GLOW_RISE_STEPS) {
        glow = phase * (1.0F / (GLOW_RISE_STEPS - 1));
    } else {
        glow = (GLOW_LAST - phase) * (1.0F / (GLOW_LAST - GLOW_RISE_STEPS));
    }
    njPushMatrixEx();
    matrix_multiply_before(
        &golem_sensor_matrices[GOLEM_PLATFORM(task)->sensor]);
    njSetTexture(GOLEM_PLATFORM(task)->texlist);
    platform_draw_model(task, GOLEM_PLATFORM(task)->object,
                        GOLEM_PLATFORM(task)->x, GOLEM_PLATFORM(task)->z, glow);
    njPopMatrixEx();
}

void golem_platform_step(struct task *task)
{
    platform_run(task);
}

void golem_platform_set_mode(struct task *task, int mode)
{
    GOLEM_PLATFORM(task)->mode = mode;
    GOLEM_PLATFORM(task)->next_mode = GOLEM_PLATFORM_NO_MODE;
    GOLEM_PLATFORM(task)->mode_steps = 0;
    starts_by_mode[GOLEM_PLATFORM(task)->mode](task);
}

static void platform_run(struct task *task)
{
    if (GOLEM_PLATFORM(task)->next_mode != GOLEM_PLATFORM_NO_MODE) {
        GOLEM_PLATFORM(task)->mode = GOLEM_PLATFORM(task)->next_mode;
        GOLEM_PLATFORM(task)->next_mode = GOLEM_PLATFORM_NO_MODE;
        GOLEM_PLATFORM(task)->mode_steps = 0;
        starts_by_mode[GOLEM_PLATFORM(task)->mode](task);
    }
    steps_by_mode[GOLEM_PLATFORM(task)->mode](task);
    GOLEM_PLATFORM(task)->mode_steps++;
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* request mode for task's next step. only this source calls it, and SHC
 * expands each call. it is external so that SHC still writes its body, which
 * the image holds */
void golem_platform_request_mode(struct task *task, int mode)
{
    GOLEM_PLATFORM(task)->next_mode = mode;
}
/* NOLINTEND(misc-use-internal-linkage) */

static void platform_return_start(struct task *task)
{
}

static void platform_return_step(struct task *task)
{
    if (golem->power > 0.0F) {
        if (golem->camera_mode != GOLEM_CAMERA_FIXED) {
            GOLEM_PLATFORM(task)->z += GOLEM_PLATFORM(task)->return_rate *
                                       GOLEM_PLATFORM(task)->home_z;
            GOLEM_PLATFORM(task)->x += GOLEM_PLATFORM(task)->return_rate *
                                       GOLEM_PLATFORM(task)->home_x;
        }
        if (GOLEM_PLATFORM(task)->z <= GOLEM_PLATFORM(task)->home_z) {
            GOLEM_PLATFORM(task)->z = GOLEM_PLATFORM(task)->home_z;
            GOLEM_PLATFORM(task)->x = GOLEM_PLATFORM(task)->home_x;
            golem_platform_request_mode(task, GOLEM_PLATFORM_MODE_WAIT);
        }
    }
}

static void platform_wait_start(struct task *task)
{
}

static void platform_wait_step(struct task *task)
{
    if (golem_player_near(task)) {
        if (player_work[0] && (player_work[0]->flags & PLAYER_WORK_ON_GROUND) &&
            player_records[0] && player_records[0]->ground_task &&
            player_records[0]->ground_task->work &&
            player_records[0]->ground_task->work->id ==
                GOLEM_PLATFORM(task)->id) {
            golem_platform_request_mode(task, GOLEM_PLATFORM_MODE_RETRACT);
        }
    }
}

static void platform_retract_start(struct task *task)
{
}

static void platform_retract_step(struct task *task)
{
    Float rate;

    sound_play_task_for(SLIDE_SOUND, task, 0, slide_volume, SLIDE_SOUND_STEPS);
    if (golem->camera_mode != GOLEM_CAMERA_FIXED) {
        rate = (golem->power * GOLEM_PLATFORM(task)->retract_rate) +
               GOLEM_PLATFORM(task)->retract_base;
        /* @hack spelling: the products negated and added, where -= would
         * do */
        GOLEM_PLATFORM(task)->z += -(rate * GOLEM_PLATFORM(task)->home_z);
        GOLEM_PLATFORM(task)->x += -(rate * GOLEM_PLATFORM(task)->home_x);
    }
    if (GOLEM_PLATFORM(task)->z >= GOLEM_PLATFORM(task)->in_z) {
        GOLEM_PLATFORM(task)->z = GOLEM_PLATFORM(task)->in_z;
        GOLEM_PLATFORM(task)->x = GOLEM_PLATFORM(task)->in_z /
                                  GOLEM_PLATFORM(task)->home_z *
                                  GOLEM_PLATFORM(task)->home_x;
        golem_platform_request_mode(task, GOLEM_PLATFORM_MODE_IN);
    }
}

static void platform_in_start(struct task *task)
{
}

static void platform_in_step(struct task *task)
{
    if (!golem_player_near(task)) {
        if (GOLEM_PLATFORM(task)->mode_steps > in_steps) {
            golem_platform_request_mode(task, GOLEM_PLATFORM_MODE_RETURN);
        }
    }
}

static void platform_draw_model(struct task *task, NJS_CNK_OBJECT *object,
                                Float x, Float z, Float glow)
{
    NJS_CNK_OBJECT *child;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, x, 0.0F, z);
    njCnkEasyDrawModel(object->model);
    if (GOLEM_PLATFORM(task)->glows && !GOLEM_PLATFORM(task)->target_down &&
        golem->power > 0.0F) {
        child = object->child;
        njPushMatrixEx();
        njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
        njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(GLOW_CONTROL);
        njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
        njSetConstantAttr(0xff, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
        constant_material_set(glow, glow, glow, glow);
        njCnkSimpleDrawModel(child->model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
        njPopMatrixEx();
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

void golem_platform_target_pos(NJS_CNK_OBJECT *object, NJS_POINT3 *pos)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    object = object->child;
    njTranslate(NULL, object->pos[0], object->pos[1], object->pos[2]);
    njTranslate(NULL, 0.0F, target_y, target_z);
    njGetTranslation(NULL, pos);
    njPopMatrixEx();
}

void golem_platforms_place(void)
{
    Uint32 i;

    for (i = 0; i < GOLEM_PLATFORMS; i++) {
        njPushMatrixEx();
        njTranslate(NULL, GOLEM_PLATFORM(golem_platform_tasks[i])->x, 0.0F,
                    GOLEM_PLATFORM(golem_platform_tasks[i])->z);
        golem_sensor_place(
            golem_platform_tasks[i], golem_platform_tasks[i]->motion.p,
            &golem_platform_matrices[i], ranges[i], &golem_platform_bases[i]);
        njPopMatrixEx();
    }
}

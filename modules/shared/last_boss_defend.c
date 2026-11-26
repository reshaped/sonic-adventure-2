#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/light.h"
#include "draw/lines.h"
#include "shared/last_boss_defend.h"
#include "shared/last_boss_defend_art.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_sphere.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

/* the task level of the emitter, its flash and its sparks */
#define EFFECT_LEVEL 3
/* a flash's texture runs from 0 up to FLARE_LAST_TEXTURE. every flash shares
 * one model. a flash's step writes the texture into the low bits of the
 * model's plist[FLARE_TID]. the step keeps that halfword's NJD_FFM_BF bit. a
 * spark's texture runs from SPARK_FIRST_TEXTURE up to SPARK_LAST_TEXTURE. each
 * texture holds for TEXTURE_DELAY + 1 steps */
#define FLARE_TID 7
#define FLARE_LAST_TEXTURE 7
#define SPARK_FIRST_TEXTURE 8
#define SPARK_LAST_TEXTURE 13
#define TEXTURE_DELAY 2
/* an emitter lasts EMISSION_STEPS steps, and shows its flash on its first.
 * FLARE_BACK_DELAY steps later, it tries a second flash FLARE_BACK back along
 * the direction. while more than SPARKS_END steps are left, it makes a spark
 * every SPARK_INTERVAL steps of the stage clock. a spark starts SPARK_RADIUS,
 * plus up to SPARK_RADIUS_SPREAD, from the direction's axis, at SPARK_DISTANCE
 * along it */
#define EMISSION_STEPS 40
#define FLARE_BACK_DELAY 4
#define FLARE_BACK 2.0F
#define SPARKS_END 10
#define SPARK_INTERVAL 3
#define SPARK_RADIUS 4.0F
#define SPARK_RADIUS_SPREAD 1.0F
#define SPARK_DISTANCE 2.0F
/* where flare_create puts a flash: at the position, or FLARE_BACK back along
 * the direction */
enum flare_place { FLARE_AT, FLARE_BEHIND };
/* a flash's work holds its facing in ang. its size, scl.x, starts at
 * FLARE_SIZE and grows FLARE_GROWTH a step. its motion's frame, scl.y, runs
 * FLARE_FRAME_STEP a step through FLARE_FRAMES, round and round. once its
 * texture is FLARE_LAST_TEXTURE, scl.z falls from FLARE_LIFE by FLARE_FADE a
 * step. the flash ends once less than FLARE_FADE is left */
#define FLARE_SIZE 0.3F
#define FLARE_GROWTH 0.02F
#define FLARE_FRAMES 5.0F
#define FLARE_FRAME_STEP 0.66F
#define FLARE_LIFE 0.8F
#define FLARE_FADE 0.1F
/* the flash's model is scaled by its size, times FLARE_WIDTH in x and y and
 * FLARE_LENGTH in z */
#define FLARE_WIDTH 0.8F
#define FLARE_LENGTH 1.2F

/* a spark's work holds its size in scl.x and its alpha in scl.z. its sprite
 * is SPARK_SCALE times its size. the size lies from SPARK_SIZE up to
 * SPARK_SIZE + SPARK_SIZE_SPREAD. the alpha falls SPARK_FADE a step, and the
 * spark ends below SPARK_GONE */
#define SPARK_SCALE 0.109F
#define SPARK_SIZE 0.25F
#define SPARK_SIZE_SPREAD 0.5F
#define SPARK_FADE 0.06F
#define SPARK_GONE 0.05F

/* the flash the running emitter made, or null. flare_create sets it as it
 * makes a flash, and the emitter's destructor clears it */
static struct task *active_flare = NULL;

static void flare_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped == 0) {
        if (work->tick != FLARE_LAST_TEXTURE) {
            if (work->timer == 0) {
                NJS_CNK_MODEL *model;
                Sint16 filter;
                work->tick++;
                work->timer = TEXTURE_DELAY;
                model = defend_flare_object[0].model;
                filter = model->plist[FLARE_TID] & NJD_FFM_BF;
                model->plist[FLARE_TID] = filter | work->tick;
            } else {
                work->timer--;
            }
        } else {
            work->scl.z -= FLARE_FADE;
            if (work->scl.z < FLARE_FADE) {
                task_kill(task);
                return;
            }
        }
        work->scl.x += FLARE_GROWTH;
        work->scl.y += FLARE_FRAME_STEP;
        if (work->scl.y >= FLARE_FRAMES) {
            work->scl.y -= FLARE_FRAMES;
        }
    }
}

static void flare_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    light_apply(LAST_BOSS_EFFECT_LIGHT);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    scale.x = scale.y = work->scl.x * FLARE_WIDTH;
    scale.z = work->scl.x * FLARE_LENGTH;
    njScaleEx(&scale);
    njSetTexture(&defend_texlist);
    njCnkSimpleDrawShapeMotion(defend_flare_object, &defend_flare_motion,
                               &defend_flare_shape, work->scl.y);
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void flare_destroy(struct task *task)
{
}

static struct task *flare_create(NJS_POINT3 *position, NJS_VECTOR *direction,
                                 Sint8 place)
{
    struct task *task;
    struct task_work *work;
    NJS_VECTOR delta;
    if (active_flare) {
        return NULL;
    }
    task = task_create(TASK_WORK, EFFECT_LEVEL, flare_step, "BL1DefendExec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    /* @hack redundant: pos set here, which both branches below set again */
    work->pos = *position;
    work->scl.x = FLARE_SIZE;
    work->scl.z = FLARE_LIFE;
    work->timer = TEXTURE_DELAY;
    work->tick = 0;
    work->scl.y = 0.0F;
    if (direction) {
        direction_angles_xz(direction->x, direction->y, direction->z,
                            &work->ang.x, &work->ang.z);
    }
    work->ang.y = 0;
    if (place == FLARE_BEHIND) {
        last_boss_vector_length(direction, -FLARE_BACK, &delta);
        vector_sum(position, &delta, &work->pos);
    } else {
        work->pos = *position;
    }
    task->draw = flare_draw;
    task->destructor = flare_destroy;
    active_flare = task;
    return task;
}

static void spark_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped == 0 && game_state.unk_09 == 0) {
        if (work->tick < SPARK_LAST_TEXTURE) {
            if (work->timer == 0) {
                work->tick++;
                work->timer = TEXTURE_DELAY;
            } else {
                work->timer--;
            }
        }
        work->scl.z -= SPARK_FADE;
        if (work->scl.z < SPARK_GONE) {
            task_kill(task);
        }
    }
}

static void spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXANIM spark_cell = {
#include "shared/last_boss_defend/spark_cell.inc"
    };
    /* @hack type: the sprite is a one-element array, where one sprite
     * would do */
    NJS_SPRITE sprite[1] = {
        {{0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, 0, &defend_texlist, NULL}};
    njFogDisable();
    njSetTexture(&defend_texlist);
    sprite->sx = sprite->sy = work->scl.x * SPARK_SCALE;
    sprite->p = work->pos;
    sprite->tanim = &spark_cell;
    sprite->tanim->texid = work->tick;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(work->scl.z, work->scl.z, work->scl.z, work->scl.z);
    njDrawSprite3D(sprite, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void spark_destroy(struct task *task)
{
}

/* @hack redundant: angles and mode, which nothing reads */
static struct task *spark_create(NJS_POINT3 *position, struct angles *angles,
                                 Sint8 mode)
{
    struct task *task;
    struct task_work *work;
    task =
        task_create(TASK_WORK, EFFECT_LEVEL, spark_step, "BL1DefendPikaExec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    work->scl.x =
        SPARK_SIZE + ((Float)rand() * (SPARK_SIZE_SPREAD * RAND_SCALE));
    work->scl.z = 1.0F;
    work->timer = TEXTURE_DELAY;
    work->tick = SPARK_FIRST_TEXTURE;
    work->pos = *position;
    task->draw = spark_draw;
    task->destructor = spark_destroy;
    return task;
}

/* an emitter's work holds the direction in scl, and its steps left in timer */
static void emitter_step(struct task *task)
{
    struct task_work *work = task->work;
    if (work->timer > 0) {
        if (work->timer == EMISSION_STEPS) {
            flare_create(&work->pos, &work->scl, FLARE_AT);
        }
        /* @bug logic: makes no second flash where the first was made, since
         * the first set active_flare and flare_create returns null while it is
         * set (the BSR at 0x4a2 of the object's code, and flare_create's TST
         * of active_flare at 0x120) */
        if (work->timer == EMISSION_STEPS - FLARE_BACK_DELAY) {
            flare_create(&work->pos, &work->scl, FLARE_BEHIND);
        }
        if (work->timer > SPARKS_END &&
            game_state.steps % SPARK_INTERVAL == 0) {
            NJS_VECTOR radial = {0.0F, 0.0F, 0.0F};
            Angle angle;
            NJS_POINT3 center;
            NJS_POINT3 position;
            NJS_VECTOR delta;
            angle = RANDOM_TURN();
            last_boss_vector_length(&work->scl, SPARK_DISTANCE, &delta);
            vector_sum(&work->pos, &delta, &center);
            radial.x = SPARK_RADIUS +
                       ((Float)rand() * (SPARK_RADIUS_SPREAD * RAND_SCALE));
            matrix_push_unit();
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, angle);
            njCalcVector(NULL, &radial, &delta);
            njPopMatrix(1);
            vector_sum(&center, &delta, &position);
            spark_create(&position, &work->ang, 0);
        }
        work->timer--;
    } else {
        task_kill(task);
    }
}

static void emitter_destroy(struct task *task)
{
    active_flare = NULL;
}

struct task *last_boss_defend(NJS_POINT3 *position, NJS_VECTOR *direction)
{
    struct task *task;
    struct task_work *work;
    if (active_flare) {
        return NULL;
    }
    task =
        task_create(TASK_WORK, EFFECT_LEVEL, emitter_step, "BL1DefendCtlExec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    work->pos = *position;
    work->timer = EMISSION_STEPS;
    if (direction) {
        direction_angles_xz(direction->x, direction->y, direction->z,
                            &work->ang.x, &work->ang.z);
        work->scl = *direction;
    } else {
        work->scl.x = work->scl.z = 0.0F;
        work->scl.y = 1.0F;
    }
    work->ang.y = 0;
    task->destructor = emitter_destroy;
    return task;
}

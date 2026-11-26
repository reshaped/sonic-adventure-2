#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/attack_art.h"
#include "boss_last2/arena.h"
#include "boss_last2/reentry.h"
#include "boss_last2/stage.h"
#include "draw/texture_anim.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the ring's course on the clock, game_state.minute and second. in minute
 * FADE_MINUTE, its sound fades in under task, its volume rising
 * VOLUME_PER_SECOND a second. in minute GROW_MINUTE, the ring shows at
 * GROW_START of its strength, which rises GROW_PER_SECOND a second. its sound
 * then plays under task's key 1. from WHOLE_MINUTE on the ring is whole, and
 * its sound plays under task's key 2 */
#define FADE_MINUTE 2
#define GROW_MINUTE 3
#define WHOLE_MINUTE 4
#define VOLUME_PER_SECOND 0.33319998F
#define GROW_START 0.5F
#define GROW_PER_SECOND 0.00833F
/* a ring of strength s is 1 + s * WIDTH_PER_STRENGTH wide, up to WHOLE_WIDTH.
 * its sound's volume is GROW_VOLUME + s * GROW_VOLUME, up to WHOLE_VOLUME.
 * each play lasts SOUND_DURATION */
#define WIDTH_PER_STRENGTH 3.0F
#define WHOLE_WIDTH 4.0F
#define GROW_VOLUME 40.0F
#define WHOLE_VOLUME 80
#define SOUND_DURATION 2
/* the ring leans RING_TILT from upright about z. its scroll's time wraps at
 * SCROLL_MASK + 1 */
#define RING_TILT 0x3000
#define SCROLL_MASK 0xfff
/* an ember shows ember_colors[0] through its first BRIGHT_STEPS steps, and
 * then each next color a step. it dies past EMBER_LIFE steps */
#define BRIGHT_STEPS 2
#define EMBER_LIFE 10
#define EMBER_POINTS 8
#define WHITE_RGB 0x00ffffff

/* drift's entries: an ember's push as it starts, and the change in its push
 * each step. x is its entry plus up to the spread after it. y and z lie within
 * their entry either way. no code reads the UNUSED_ spreads */
enum drift_setting {
    START_X,
    START_X_SPREAD,
    START_Y,
    UNUSED_START_Y,
    START_Z,
    UNUSED_START_Z,
    STEP_X,
    STEP_X_SPREAD,
    STEP_Y,
    UNUSED_STEP_Y,
    STEP_Z,
    UNUSED_STEP_Z
};

/* the ring's uvs, laid out as struct texture_anim says: one key for fourteen
 * strips of the model */
static Sint16 ring_uvs[374] = {
#include "boss_last2/reentry/ring_uvs.inc"
};
static struct texture_anim ring_scroll = {
#include "boss_last2/reentry/ring_scroll.inc"
};
static Float unused_scale = 3.0F;
static NJS_POINT3 ring_pos = {
#include "boss_last2/reentry/ring_pos.inc"
};
static NJS_POLYGON_VTX ember_points[EMBER_POINTS] = {
#include "boss_last2/reentry/ember_points.inc"
};
static Uint32 ember_colors[] = {0xff000000, 0xdd000000, 0xbb000000,
                                0x99000000, 0x77000000, 0x55000000,
                                0x33000000, 0x11000000, 0};
/* clang-format off */
/* @hack type: an ember's drift an array that drift_setting indexes, where a
 * record would do */
static Float drift[] = {
    0.1F,   0.1F,   0.025F, 0.1F,  0.1F, 0.1F,
    0.025F, 0.025F, 0.015F, 0.05F, 0.2F, 0.05F
};
/* clang-format on */
static Float ember_scale = 1.5F;

static void reentry_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 volume;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.minute == FADE_MINUTE) {
        volume = game_state.second * VOLUME_PER_SECOND;
        sound_play_for(SOUND_REENTRY, task, 1, 0, SOUND_DURATION);
        sound_set_volume(task, SOUND_REENTRY, volume);
    } else if (game_state.minute == GROW_MINUTE) {
        /* @hack spelling: scl.y set to GROW_START and then raised by the
         * clock, where one assignment would do */
        work->scl.y = GROW_START;
        work->scl.y += game_state.second * GROW_PER_SECOND;
        work->scl.z = work->scl.y;
        work->scl.x = 1.0F + (work->scl.y * WIDTH_PER_STRENGTH);
        volume = GROW_VOLUME + (work->scl.y * GROW_VOLUME);
        sound_play_for(SOUND_REENTRY, FINALHAZARD_SOUND_KEY(task, 1), 1, 0,
                       SOUND_DURATION);
        sound_set_volume(FINALHAZARD_SOUND_KEY(task, 1), SOUND_REENTRY, volume);
    } else if (game_state.minute >= WHOLE_MINUTE) {
        work->scl.y = 1.0F;
        work->scl.z = work->scl.y;
        work->scl.x = WHOLE_WIDTH;
        sound_play_for(SOUND_REENTRY, FINALHAZARD_SOUND_KEY(task, 2), 1, 0,
                       SOUND_DURATION);
        sound_set_volume(FINALHAZARD_SOUND_KEY(task, 2), SOUND_REENTRY,
                         WHOLE_VOLUME);
    }
}

static void reentry_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    /* @hack type: argb, a signed copy of the color, where color alone would
     * do */
    Sint32 argb;
    Uint32 color;

    if (work->scl.z > 0.0F) {
        njSetTexture(&finalhazard_effect_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z - RING_TILT);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        scale = work->scl.x;
        njScale(NULL, scale, scale, scale);
        texture_anim_apply(&finalhazard_ring_model, &ring_scroll,
                           game_state.steps & SCROLL_MASK);
        /* the alpha runs up to 255, past Sint8. SHC converts it to an int and
         * keeps the low byte */
#ifdef NONMATCHING
        /* this side shifts the alpha as a Uint32. a float past Sint8's range in
         * the cast is undefined C */
        color = ((Uint32)(work->scl.z * 255.0F) << 24) | WHITE_RGB;
#else
        argb = ((Sint8)(work->scl.z * 255.0F) << 24) + WHITE_RGB;
        color = argb;
#endif
        model_draw_tinted(&finalhazard_ring_model, color);
        njPopMatrixEx();
    }
}

static void reentry_destroy(struct task *task)
{
}

struct task *finalhazard_reentry_ring(struct task *parent,
                                      const NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, reentry_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    task->draw = reentry_draw;
    task->destructor = reentry_destroy;
    if (pos) {
        work->pos = *pos;
    } else {
        work->pos = ring_pos;
    }
    work->scl.z = -1.0F;
    work->scl.y = 0.0F;
    work->scl.x = 1.0F;
    return task;
}

/* a random value within range of center */
#define SCATTER_FROM(center, range)                                            \
    ((center) + (range) - ((Float)rand() * (range) * (2.0F * RAND_SCALE)))
/* a random value from base up to base + spread */
#define PLUS_UP_TO(base, spread)                                               \
    ((base) + ((Float)rand() * (spread) * RAND_SCALE))

static void ember_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    if (!motion) {
        task_kill(task);
        return;
    }
    motion->acc.x += PLUS_UP_TO(drift[STEP_X], drift[STEP_X_SPREAD]);
    motion->acc.y += FINALHAZARD_SCATTER(drift[STEP_Y]);
    motion->acc.z += FINALHAZARD_SCATTER(drift[STEP_Z]);
    motion->spd.x += motion->acc.x;
    motion->spd.y += motion->acc.y;
    motion->spd.z += motion->acc.z;
    work->pos.x += motion->spd.x;
    work->pos.y += motion->spd.y;
    work->pos.z += motion->spd.z;
    if (work->timer++ >= EMBER_LIFE) {
        task_kill(task);
    }
}

static void ember_draw(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 frame;
    Float scale;
    if (work->timer < BRIGHT_STEPS) {
        frame = 0;
    } else {
        frame = work->timer - BRIGHT_STEPS;
    }
    /* @bug memory: reads ember_colors[9], past the array's end, at
     * 0x8c50bb28. ember_step leaves timer at 11 as it kills the task, at
     * 0x8c50ba7e, and the draw before the task dies reads that */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it draws nothing past
     * EMBER_LIFE */
    if (work->timer > EMBER_LIFE) {
        return;
    }
#endif
    /* @hack spelling: the even points' colors set in one chain and the odd
     * points' in another, where one loop over the points would do */
    ember_points[0].col = ember_points[2].col = ember_points[4].col =
        ember_points[6].col = ember_colors[frame];
    ember_points[1].col = ember_points[3].col = ember_points[5].col =
        ember_points[7].col = ember_colors[frame];
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    scale = ember_scale;
    njScale(NULL, scale, scale, scale);
    njDrawPolygon3DEx(ember_points, EMBER_POINTS, 1);
    njPopMatrixEx();
}

static void ember_destroy(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
}

struct task *finalhazard_ember_create(struct task *parent,
                                      const NJS_POINT3 *pos, Float range)
{
    struct task *task;
    struct task_work *work;
    struct task_motion *motion;
    if (!parent) {
        task = task_create(TASK_WORK | TASK_MOTION, 3, ember_step,
                           "BL2KogedamaExec");
    } else {
        task = task_create_child(TASK_WORK | TASK_MOTION, ember_step, parent);
    }
    if (!task) {
        return NULL;
    }
    task->draw = ember_draw;
    task->destructor = ember_destroy;
    work = task->work;
    motion = task->motion.p;
    work->pos.x = SCATTER_FROM(pos->x, range);
    work->pos.y = SCATTER_FROM(pos->y, range);
    work->pos.z = SCATTER_FROM(pos->z, range);
    motion->acc.x = PLUS_UP_TO(drift[START_X], drift[START_X_SPREAD]);
    motion->acc.y = FINALHAZARD_SCATTER(drift[START_Y]);
    motion->acc.z = FINALHAZARD_SCATTER(drift[START_Z]);
    return task;
}

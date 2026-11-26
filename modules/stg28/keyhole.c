/* the keyhole, KEYHOLE in Pyramid Cave's object list: a plate on a base, with
 * three lamps. once the key of its color locks in it, the plate sinks with the
 * key. the key door of its color then opens */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "draw/color.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/tint_pulse.h"
#include "sound/sound.h"
#include "stg28/key.h"
#include "stg28/keyhole.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"

#include "stg28/keyhole/text.inc"
#include "stg28/models/keyhole.h"

#define LAMPS 3
/* the plate sinks by the size of SINK_SPEED a step, down to SINK_DEPTH from
 * the work's pos */
#define SINK_SPEED (-0.02F)
#define SINK_DEPTH (-7.5F)
/* the hum, an id in pyramid_cave_sound_list. it plays while the keyhole waits
 * or sinks and player 0 lies within hum_range. a play lasts HUM_SOUND_STEPS + 1
 * updates, as sound_play_task_at_for says */
#define HUM_SOUND 7
#define HUM_SOUND_PRIORITY 0
#define HUM_SOUND_STEPS 15

#define CHANNEL_MAX 255.0F
#define WHITE 0xffffff
#define OPAQUE_WHITE 0xffffffff
/* @hack type: alpha converted through int, where a conversion to Uint32 would
 * do */
#define WHITE_AT(alpha) (((Uint32)(int)(alpha) << ALPHA_SHIFT) | WHITE)

/* while the keyhole waits, each lamp's alpha runs a cycle of LAMP_STEPS of
 * game_state.steps. it climbs to CHANNEL_MAX over the first LAMP_RISE, then
 * falls back to 0 by LAMP_FALL_RATE a step. each lamp lags LAMP_LAG steps
 * behind the one before. the glow's alpha climbs to GLOW_TOP over the cycle's
 * first GLOW_RISE steps, then falls back to 0. once the keyhole sinks, the
 * lamps pulse together as shared/tint_pulse.h says, and the glow is opaque */
#define LAMP_STEPS 180
#define LAMP_RISE 60
#define LAMP_LAG 30
/* the alpha a lamp loses a step, CHANNEL_MAX / (LAMP_STEPS - LAMP_RISE - 1)
 * rounded up */
#define LAMP_FALL_RATE 2.1428573F
#define GLOW_RISE 90
#define GLOW_TOP 192.0F

static void keyhole_destroy(struct task *task);
static void keyhole_step(struct task *task);
static void keyhole_draw(struct task *task);
static struct keyhole *keyhole_alloc(struct task *task);
static void keyhole_free(struct task *task);
static void keyhole_take_mode(struct task *task, int mode);
static void keyhole_run_mode(struct task *task);
static void keyhole_wait_start(struct task *task);
static void keyhole_wait_step(struct task *task);
static void keyhole_sink_start(struct task *task);
static void keyhole_sink_step(struct task *task);
static void keyhole_sunk_start(struct task *task);
static void keyhole_sunk_step(struct task *task);

KEPT static NJS_CNK_MODEL *plate_models[KEY_COLORS] = {
#include "stg28/keyhole/plate_models.inc"
};
KEPT static NJS_CNK_MODEL *plate_glow_models[KEY_COLORS] = {
#include "stg28/keyhole/plate_glow_models.inc"
};
KEPT static NJS_CNK_MODEL *base_models[KEY_COLORS] = {
#include "stg28/keyhole/base_models.inc"
};
KEPT static NJS_CNK_MODEL *base_glow_models[KEY_COLORS] = {
#include "stg28/keyhole/base_glow_models.inc"
};
KEPT static NJS_CNK_MODEL *lamp_models[LAMPS] = {
#include "stg28/keyhole/lamp_models.inc"
};
NJS_TEXLIST *cave_keyhole_plate_texlists[KEY_COLORS + 1] = {
#include "stg28/keyhole/cave_keyhole_plate_texlists.inc"
};
NJS_TEXLIST *cave_keyhole_base_texlists[KEY_COLORS + 1] = {
#include "stg28/keyhole/cave_keyhole_base_texlists.inc"
};
NJS_TEXLIST *cave_keyhole_lamp_texlists[LAMPS + 1] = {
#include "stg28/keyhole/cave_keyhole_lamp_texlists.inc"
};
static char *color_names[KEY_COLORS] = {
#include "stg28/keyhole/color_names.inc"
};
/* the hum's volume, and how near player 0 must lie for the step to play it */
static Sint32 hum_volume = 0;
static Float hum_range = 100.0F;
typedef void (*keyhole_mode_handler)(struct task *task);

KEPT static keyhole_mode_handler mode_starts[KEYHOLE_MODES] = {
#include "stg28/keyhole/mode_starts.inc"
};
KEPT static keyhole_mode_handler mode_steps[KEYHOLE_MODES] = {
#include "stg28/keyhole/mode_steps.inc"
};

void cave_keyhole_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    if (!keyhole_alloc(task)) {
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    cave_keyholes[KEY_COLOR(task->work)] = task;
    task->draw = keyhole_draw;
    task->step = keyhole_step;
    task->destructor = keyhole_destroy;
    /* @hack redundant: a block no path reaches, a store to the work's mode */
    if (0) {
        work->mode = 0;
    }
    object = world_pool_alloc();
    KEYHOLE(task)->object = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c504440).
     * keyhole_destroy tests the record's object for null (TST at
     * 0x8c5044ce) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the keyhole, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_keyhole_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    object->ang[1] = task->work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SIZE_CLASS(2) | WORLD_MOVING | WORLD_SOLID, task, object);
}

static void keyhole_destroy(struct task *task)
{
    if (KEYHOLE(task)->object) {
        world_remove(task, KEYHOLE(task)->object);
        world_pool_free(KEYHOLE(task)->object);
    }
    keyhole_free(task);
    cave_keyholes[KEY_COLOR(task->work)] = NULL;
}

static void keyhole_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: task->work read again, where work would do */
    cave_keyholes[KEY_COLOR(task->work)] = task;
    keyhole_run_mode(task);
    /* @hack redundant: a block no path reaches, a call of the task's draw */
    if (0) {
        task->draw(task);
    }
    work->flags |= WORLD_ADDED;
}

static void keyhole_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 lamp_colors[LAMPS];
    int lamp_steps[LAMPS];
    Uint32 glow;
    int i;
    int step;

    switch (KEYHOLE(task)->mode) {
    case KEYHOLE_MODE_WAIT:
        for (i = 0; i < LAMPS; i++) {
            lamp_steps[i] = (game_state.steps - (i * LAMP_LAG)) % LAMP_STEPS;
            if (lamp_steps[i] < LAMP_RISE) {
                lamp_colors[i] =
                    WHITE_AT(lamp_steps[i] * (CHANNEL_MAX / LAMP_RISE));
            } else {
                lamp_colors[i] =
                    WHITE_AT((LAMP_STEPS - 1 - lamp_steps[i]) * LAMP_FALL_RATE);
            }
        }
        step = game_state.steps % LAMP_STEPS;
        if (step < GLOW_RISE) {
            glow = WHITE_AT(step * (GLOW_TOP / (GLOW_RISE - 1)));
        } else {
            glow = WHITE_AT((LAMP_STEPS - 1 - step) *
                            (GLOW_TOP / (LAMP_STEPS - GLOW_RISE)));
        }
        break;
    default:
        for (i = 0; i < LAMPS; i++) {
            lamp_steps[i] = game_state.steps % TINT_PULSE_STEPS;
            if (lamp_steps[i] < TINT_PULSE_RISE) {
                lamp_colors[i] =
                    TINT_PULSE_RISE_ALPHA(lamp_steps[i]) | TINT_PULSE_COLORS;
            } else {
                lamp_colors[i] =
                    TINT_PULSE_FALL_ALPHA(lamp_steps[i]) | TINT_PULSE_COLORS;
            }
        }
        glow = OPAQUE_WHITE;
        break;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again, where work would do */
    njSetTexture(cave_keyhole_base_texlists[KEY_COLOR(task->work)]);
    njCnkEasyDrawModel(base_models[KEY_COLOR(task->work)]);
    model_draw_tinted(base_glow_models[KEY_COLOR(task->work)], glow);
    njTranslate(NULL, 0.0F, KEYHOLE(task)->sink, 0.0F);
    for (i = 0; i < LAMPS; i++) {
        njSetTexture(cave_keyhole_lamp_texlists[i]);
        model_draw_tinted(lamp_models[i], lamp_colors[i]);
    }
    njSetTexture(cave_keyhole_plate_texlists[KEY_COLOR(task->work)]);
    njCnkEasyDrawModel(plate_models[KEY_COLOR(task->work)]);
    model_draw_tinted(plate_glow_models[KEY_COLOR(task->work)], glow);
    njPopMatrixEx();

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- KEYHOLE(     )";
    }
}

static struct keyhole *keyhole_alloc(struct task *task)
{
    task->extra.p = syMalloc(sizeof(struct keyhole));
    if (!task->extra.p) {
        return NULL;
    }
    KEYHOLE(task)->sink = 0.0F;
    KEYHOLE(task)->object = NULL;
    keyhole_take_mode(task, KEYHOLE_MODE_WAIT);
    return task->extra.p;
}

static void keyhole_free(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void keyhole_take_mode(struct task *task, int mode)
{
    KEYHOLE(task)->mode = mode;
    KEYHOLE(task)->next_mode = KEYHOLE_NO_MODE;
    KEYHOLE(task)->timer = 0;
    mode_starts[KEYHOLE(task)->mode](task);
}

static void keyhole_set_mode(struct task *task, int mode)
{
    KEYHOLE(task)->next_mode = mode;
}

static void keyhole_run_mode(struct task *task)
{
    if (KEYHOLE(task)->next_mode != KEYHOLE_NO_MODE) {
        KEYHOLE(task)->mode = KEYHOLE(task)->next_mode;
        KEYHOLE(task)->next_mode = KEYHOLE_NO_MODE;
        KEYHOLE(task)->timer = 0;
        mode_starts[KEYHOLE(task)->mode](task);
    }
    mode_steps[KEYHOLE(task)->mode](task);
    KEYHOLE(task)->timer++;
}

static void keyhole_wait_start(struct task *task)
{
}

static void keyhole_wait_step(struct task *task)
{
    struct task_work *work = task->work;

    TASK_FORCES_OF(task)[0].speed.y = 0.0F;
    /* @hack spelling: task->work read again, where work would do */
    if (cave_keys[KEY_COLOR(task->work)] != NULL &&
        KEY(cave_keys[KEY_COLOR(task->work)])->mode == KEY_MODE_LOCKED) {
        keyhole_set_mode(task, KEYHOLE_MODE_SINK);
    }
    if (njDistanceP2P(&player_work[0]->pos, &work->pos) < hum_range) {
        sound_play_task_at_for(HUM_SOUND, task, HUM_SOUND_PRIORITY, hum_volume,
                               HUM_SOUND_STEPS);
    }
}

static void keyhole_sink_start(struct task *task)
{
}

static void keyhole_sink_step(struct task *task)
{
    struct task_work *work = task->work;
    Float step;

    /* @hack spelling: the size of SINK_SPEED taken at each use, where its
     * value would do */
    if (KEYHOLE(task)->sink - njAbs(SINK_SPEED) <= SINK_DEPTH) {
        /* @bug logic: the last step moves the key and the rider up by the
         * distance left, where it should move them down. its step is
         * SINK_DEPTH less the sink, the distance's negation (FNEG at
         * 0x8c504aca) */
        step = -KEYHOLE(task)->sink + SINK_DEPTH;
        KEYHOLE(task)->sink = SINK_DEPTH;
        keyhole_set_mode(task, KEYHOLE_MODE_SUNK);
    } else {
        step = njAbs(SINK_SPEED);
        KEYHOLE(task)->sink -= njAbs(SINK_SPEED);
    }
    /* @bug logic: moves only the riders' record of the player in slot 0
     * (FMOV.S at 0x8c504af6), so the plate carries no player of slot 1 down.
     * key_door_step moves both records (FMOV.S at 0x8c504eaa) */
    TASK_FORCES_OF(task)[0].speed.y = -step;
    KEYHOLE(task)->object->pos[1] = work->pos.y + KEYHOLE(task)->sink;
    /* @hack spelling: task->work read again, where work would do */
    if (cave_keys[KEY_COLOR(task->work)] != NULL) {
        cave_keys[KEY_COLOR(task->work)]->work->pos.y -= step;
    }
    if (njDistanceP2P(&player_work[0]->pos, &work->pos) < hum_range) {
        sound_play_task_at_for(HUM_SOUND, task, HUM_SOUND_PRIORITY, hum_volume,
                               HUM_SOUND_STEPS);
    }
}

static void keyhole_sunk_start(struct task *task)
{
}

static void keyhole_sunk_step(struct task *task)
{
    TASK_FORCES_OF(task)[0].speed.y = 0.0F;
}

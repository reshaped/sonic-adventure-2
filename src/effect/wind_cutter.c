/* the wind cutter: blades of wind that spin about a player, and rings that
 * fade in and out */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "effect/wind_cutter.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define WIND_LEVEL 3 /* a blade's and a ring's task level */
#define WIND_MODEL 28 /* the slot of model_table whose object they draw */
#define WIND_TURN 0x1000 /* how far a blade or a ring turns about y a step */

/* a blade waits fewer than BLADE_WAIT steps, at random, drawing nothing. it
 * then shrinks toward BLADE_SCALE until its timer reaches BLADE_FADE_AT. as
 * it fades, it shrinks toward BLADE_FADE_SCALE, and its alpha falls from
 * BLADE_ALPHA by BLADE_FADE_STEP a step. the step that finds the alpha at -1
 * or below ends it */
#define BLADE_WAIT 15
#define BLADE_FADE_AT 20
#define BLADE_SCALE 0.1F
#define BLADE_SCALE_STEP 0.02F /* how far a blade shrinks a step */
#define BLADE_ALPHA 1.3F
#define BLADE_FADE_SCALE 0.05F
#define BLADE_FADE_STEP 0.16F
/* a blade's scale starts at random from BLADE_SCALE_MIN up to
 * BLADE_SCALE_MIN + BLADE_SCALE_SPAN */
#define BLADE_SCALE_MIN 0.8F
#define BLADE_SCALE_SPAN 0.2F

/* a ring's alpha starts at RING_ALPHA, and moves RING_FADE_STEP a step. it
 * rises toward 0 while the ring's timer is below RING_SHOWN. it falls toward
 * -1 once the timer reaches RING_HIDE. the step that finds the timer at
 * RING_LAST ends the ring */
#define RINGS 4
#define RING_ALPHA (-0.95F)
#define RING_SHOWN 40
#define RING_HIDE 80
#define RING_LAST 120
#define RING_FADE_STEP 0.025F

/* a blade's or a ring's record, in its task's extra */
struct wind_cutter {
    /* the slot of player_tasks whose Sonic record holds the texture list it
     * draws with */
    Sint8 owner;
    Sint8 player; /* the slot of player_records it starts at */
    /* a blade's starts at 0 or below. the blade draws nothing while it is
     * below 0 */
    Sint16 timer;
    Float alpha; /* the alpha its draw passes constant_material_set */
    struct angles ang; /* its heading, in y */
    Float scale;
    NJS_POINT3 pos;
};

static void wind_cutter_origin(int player, NJS_POINT3 *pos)
{
    struct player *record = player_records[player];

    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        *pos = record->unk_218;
        break;
    /* @hack redundant: the cases that fall into default */
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
    default:
        *pos = record->unk_230;
        break;
    }
}

static void wind_cutter_draw(struct task *task)
{
    struct wind_cutter *wind = task->extra.p;

    if (wind->timer >= 0) {
        njFogDisable();
        constant_material_set(wind->alpha, 0.0F, 0.0F, 0.0F);
        njSetTexture(PLAYER_RECORD(wind->owner)->by_character.sonic.texlist);
        njPushMatrixEx();
        njTranslateEx(&wind->pos);
        njRotateXYZ(NULL, 0, wind->ang.y, 0);
        njScale(NULL, wind->scale, wind->scale, wind->scale);
        njCnkSimpleDrawObject(model_table[WIND_MODEL].object);
        njPopMatrixEx();
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njFogEnable();
    }
}

static void wind_cutter_fade(struct task *task)
{
    struct wind_cutter *wind = task->extra.p;

    if (game_state.time_stopped == 0) {
        if (wind->alpha <= -1.0F) {
            task_kill(task);
            return;
        }
        if (wind->timer++ >= 0) {
            wind->alpha = float_approach(wind->alpha, -1.0F, BLADE_FADE_STEP);
        }
        wind->scale =
            float_approach(wind->scale, BLADE_FADE_SCALE, BLADE_SCALE_STEP);
        wind->ang.y += WIND_TURN;
    }
}

static void wind_cutter_step(struct task *task)
{
    struct wind_cutter *wind = task->extra.p;

    if (game_state.time_stopped == 0) {
        /* @hack redundant: the else after this branch's return */
        if (wind->timer++ >= BLADE_FADE_AT) {
            task->step = wind_cutter_fade;
            return;
        } else if (wind->timer >= 0) {
            wind->scale =
                float_approach(wind->scale, BLADE_SCALE, BLADE_SCALE_STEP);
            wind->ang.y += WIND_TURN;
        }
    }
}

static void wind_ring_step(struct task *task)
{
    struct wind_cutter *wind = task->extra.p;

    if (game_state.time_stopped == 0) {
        if (wind->timer++ >= RING_LAST) {
            task_kill(task);
        } else {
            if (wind->timer >= RING_HIDE) {
                wind->alpha =
                    float_approach(wind->alpha, -1.0F, RING_FADE_STEP);
            } else if (wind->timer < RING_SHOWN) {
                wind->alpha = float_approach(wind->alpha, 0.0F, RING_FADE_STEP);
            }
            wind->ang.y += WIND_TURN;
        }
    }
}

/* a blade's task with its record, which the caller fills. wind_ring_add gives
 * it a ring's step. it returns null where the heap can't give both */
static struct task *wind_cutter_task_create(void)
{
    struct task *task;
    struct wind_cutter *wind;

    task = task_create(0, WIND_LEVEL, wind_cutter_step,
                       "EffectWindCutterExecutor");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((wind = syMalloc(sizeof(struct wind_cutter))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = wind;
        task->draw = wind_cutter_draw;
    }
    return task;
}

/* add a blade at the player in slot player, headed along heading */
static void wind_cutter_add(int owner, int player, Angle heading)
{
    struct task *task;
    struct wind_cutter *wind;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = wind_cutter_task_create()) != NULL) {
        wind = task->extra.p;
        wind->owner = owner;
        wind->player = player;
        wind_cutter_origin(player, &wind->pos);
        wind->ang.y = heading;
        wind->timer = -(rand() * (BLADE_WAIT * RAND_SCALE));
        wind->scale =
            (rand() * (BLADE_SCALE_SPAN * RAND_SCALE)) + BLADE_SCALE_MIN;
        wind->alpha = BLADE_ALPHA;
    }
}

/* add a ring at the player in slot player, headed along heading */
static void wind_ring_add(int owner, int player, Angle heading)
{
    struct task *task;
    struct wind_cutter *wind;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = wind_cutter_task_create()) != NULL) {
        wind = task->extra.p;
        task->step = wind_ring_step;
        wind->owner = owner;
        wind->player = player;
        wind_cutter_origin(player, &wind->pos);
        wind->ang.y = heading;
        wind->scale = 1.0F;
        wind->alpha = RING_ALPHA;
        wind->timer = 0;
    }
}

void wind_cutters_start(int owner, int player, int count)
{
    Angle heading;
    Angle spacing;
    int i;

    heading = RANDOM_TURN();
    spacing = FULL_TURN / count;
    while (count-- > 0) {
        wind_cutter_add(owner, player, heading);
        heading += spacing;
    }
    /* @hack spelling: spacing set again for the rings, where QUARTER_TURN
     * in their loop would do */
    spacing = QUARTER_TURN;
    for (i = 0; i < RINGS; i++) {
        wind_ring_add(owner, player, heading);
        heading += spacing;
    }
}

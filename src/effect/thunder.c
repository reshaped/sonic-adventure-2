/* thunder: bolts that walk a jagged line toward a point, and leave lines of
 * light that fade */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/lines.h"
#include "effect/texlist.h"
#include "effect/thunder.h"
#include "player/action.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the task levels of a light and of a bolt */
#define LIGHT_LEVEL 1
#define BOLT_LEVEL 3

#define BOLT_COLOR 0xffa0c0e0 /* a new light's color: pale blue, as ARGB */
/* what each step of a light takes from its blue, green and red. a channel
 * no more than 1 above its fade goes to 0 instead */
#define BLUE_FADE 4
#define GREEN_FADE 5
#define RED_FADE 6

/* the most segments a bolt walks in a step, and how far each runs along the
 * bolt's line. its end then moves at random */
#define SEGMENTS 32
#define SEGMENT_LENGTH 1.0F
/* a step that walks all its segments rolls a random number from 0 up to 1.
 * the step starts a branch where the roll comes out above BRANCH_ROLL. the
 * branch aims up to BRANCH_REACH along each of +x, +y and +z from where the
 * bolt stands */
#define BRANCH_ROLL 0.6F
#define BRANCH_REACH 30.0F

/* a struck player's bolts start in a cube STRIKE_SPREAD on a side. its center
 * lies STRIKE_HEIGHT above the player's pos, against world_gravity */
#define STRIKE_HEIGHT 100.0F
#define STRIKE_SPREAD 5.0F
#define STRIKE_STEPS 5 /* the steps a strike's bolts walk */

/* a bolt's flash: its size, the sparks it throws a step, and how much nearer
 * it draws */
#define FLASH_SCALE 9.0F
#define FLASH_SPARKS 32
#define FLASH_DEPTH_BIAS 6.0F

/* a light's record, in its task's extra. a light is the line a segment of a
 * bolt's walk leaves. it runs from where the walk stood to where the segment
 * took it */
struct thunder_light {
    NJS_COLOR color; /* the line's color, which fades to black */
    NJS_POINT3 ends[2];
};

/* a bolt's record, in its task's extra */
struct thunder_bolt {
    Uint8 unk_00;
    Uint8 unk_01;
    /* the steps the bolt has left to walk. the step that finds it at 0 or
     * below ends the bolt */
    Sint16 steps;
    Float length; /* how far target lies from where the bolt started */
    NJS_COLOR color; /* each light's color as it starts */
    NJS_POINT3 pos; /* where the walk stands */
    NJS_POINT3 target;
    /* the line from where the bolt started, along the unit vector toward
     * target */
    NJS_LINE line;
};

static void thunder_start(NJS_POINT3 *from, NJS_POINT3 *to, Sint16 steps);

/* lists of one texture each: barria_t10, barria_n08 and mutekiball */
static NJS_TEXLIST thunder_texlists[3] = {
#include "effect/thunder/thunder_texlists.inc"
};

/* where a bolt aims at the player in slot player: unk_218 of its record */
static void thunder_target(int player, NJS_POINT3 *pos)
{
    struct player *record = player_records[player];

    /* @hack redundant: a switch on the character whose arms do the same */
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        *pos = record->unk_218;
        break;
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
    default:
        *pos = record->unk_218;
        break;
    }
}

static void thunder_light_draw(struct task *task)
{
    struct thunder_light *light = task->extra.p;
    NJS_POINT3 ends[2];
    NJS_COLOR colors[2];

    colors[0] = colors[1] = light->color;
    /* @hack redundant: the copy of the ends, where light->ends would do */
    ends[0] = light->ends[0];
    ends[1] = light->ends[1];
    line_queue(ends, colors, LINE_ADDITIVE);
}

/* blue fades last, so the step that finds it at 0 ends the light */
static void thunder_light_step(struct task *task)
{
    struct thunder_light *light = task->extra.p;

    /* @hack spelling: the test of blue as an else after the return, where a
     * test of its own would do */
    if (game_state.time_stopped != 0) {
        return;
    } else if (light->color.argb.b == 0) {
        task_kill(task);
        return;
    }
    if (light->color.argb.b > BLUE_FADE + 1) {
        light->color.argb.b -= BLUE_FADE;
    } else {
        light->color.argb.b = 0;
    }
    if (light->color.argb.g > GREEN_FADE + 1) {
        light->color.argb.g -= GREEN_FADE;
    } else {
        light->color.argb.g = 0;
    }
    if (light->color.argb.r > RED_FADE + 1) {
        light->color.argb.r -= RED_FADE;
    } else {
        light->color.argb.r = 0;
    }
}

/* a light's task with its record, which the caller fills. it returns null
 * where the heap can't give both */
static struct task *thunder_light_task_create(void)
{
    struct task *task;
    struct thunder_light *light;

    task = task_create(0, LIGHT_LEVEL, thunder_light_step,
                       "EffectThunderLightExecutor");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((light = syMalloc(sizeof(struct thunder_light))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = light;
        task->draw = thunder_light_draw;
    }
    return task;
}

/* a flash at (x, y, z), where player_hooks.flash is set */
static void thunder_flash(Float x, Float y, Float z)
{
    if (player_hooks.flash != NULL) {
        player_hooks.flash(x, y, z, FLASH_SCALE, FLASH_SPARKS,
                           FLASH_DEPTH_BIAS);
    }
}

/* a step's walk stops at the first segment that takes it past length. the
 * walk has come as far as its end's projection onto line lies from where the
 * bolt started */
static void thunder_step(struct task *task)
{
    struct thunder_bolt *bolt = task->extra.p;
    struct task *light_task;
    struct thunder_light *light;
    NJS_POINT3 start;
    NJS_POINT3 pos;
    NJS_VECTOR step;
    /* @hack spelling: length, a local for SEGMENT_LENGTH, where the
     * constant would do */
    Float length;
    Float reach;
    int i;

    if (game_state.time_stopped != 0) {
        return;
    }
    if (bolt->steps-- <= 0) {
        task_kill(task);
        thunder_flash(bolt->pos.x, bolt->pos.y, bolt->pos.z);
        return;
    }
    start.x = bolt->line.px;
    start.y = bolt->line.py;
    start.z = bolt->line.pz;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    for (i = SEGMENTS; i > 0; i--) {
        pos = bolt->pos;
        length = SEGMENT_LENGTH;
        step.x = bolt->line.vx * length;
        step.y = bolt->line.vy * length;
        step.z = bolt->line.vz * length;
        njAddVector(&pos, &step);
        pos.x += 0.5F - (rand() * RAND_SCALE);
        pos.y += 0.5F - (rand() * RAND_SCALE);
        pos.z += 0.5F - (rand() * RAND_SCALE);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((light_task = thunder_light_task_create()) != NULL) {
            light = light_task->extra.p;
            light->ends[0] = bolt->pos;
            light->ends[1] = pos;
            light->color = bolt->color;
        }
        bolt->pos = pos;
        njDistanceP2L(&bolt->pos, &bolt->line, &pos);
        if (njDistanceP2P(&pos, &start) > bolt->length) {
            /* @bug memory: task_kill writes through a null light_task where
             * the heap couldn't give this segment's light (JSR at
             * 0x8c0a31e2). the test above expects the null (BT/S at
             * 0x8c0a3188) */
#ifdef NONMATCHING
            /* this side repairs the tag above: it kills only a light the
             * heap gave */
            if (light_task != NULL) {
                task_kill(light_task);
            }
#else
            task_kill(light_task);
#endif
            thunder_flash(bolt->pos.x, bolt->pos.y, bolt->pos.z);
            /* @bug logic: this return leaves on Ninja's stack the matrix
             * that the JSR at 0x8c0a30ee pushed. its BRA at 0x8c0a31f4 passes
             * over the step's pop, the JSR at 0x8c0a32de */
            return;
        }
    }
    if (rand() * RAND_SCALE > BRANCH_ROLL) {
        reach = rand() * (BRANCH_REACH * RAND_SCALE);
        pos.x = bolt->pos.x + (reach * rand() * RAND_SCALE);
        pos.y = bolt->pos.y + (reach * rand() * RAND_SCALE);
        pos.z = bolt->pos.z + (reach * rand() * RAND_SCALE);
        thunder_start(&bolt->pos, &pos, bolt->steps >> 1);
    }
    njPopMatrixEx();
}

/* a bolt's task with its record, which the caller fills. it returns null
 * where the heap can't give both */
static struct task *thunder_task_create(void)
{
    struct task *task;
    struct thunder_bolt *bolt;

    task = task_create(0, BOLT_LEVEL, thunder_step, "EffectThunderExecutor");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((bolt = syMalloc(sizeof(struct thunder_bolt))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = bolt;
    }
    return task;
}

/* start a bolt at from, aimed at to, that walks for steps steps. none starts
 * where the heap can't give its task and its record */
static void thunder_start(NJS_POINT3 *from, NJS_POINT3 *to, Sint16 steps)
{
    struct task *task;
    struct thunder_bolt *bolt;
    NJS_VECTOR dir;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = thunder_task_create()) != NULL) {
        bolt = task->extra.p;
        bolt->pos = *from;
        bolt->target = *to;
        dir = bolt->target;
        njSubVector(&dir, &bolt->pos);
        njUnitVector(&dir);
        bolt->line.vx = dir.x;
        bolt->line.vy = dir.y;
        bolt->line.vz = dir.z;
        bolt->line.px = bolt->pos.x;
        bolt->line.py = bolt->pos.y;
        bolt->line.pz = bolt->pos.z;
        bolt->steps = steps;
        bolt->color.color = BOLT_COLOR;
        bolt->unk_01 = 0;
        bolt->length = njDistanceP2P(&bolt->pos, &bolt->target);
    }
}

void thunder_strike(int player, int count)
{
    struct task_work *work = player_work[player];
    NJS_POINT3 above;
    NJS_POINT3 to;
    NJS_POINT3 from;
    /* @hack spelling: spread, a local for STRIKE_SPREAD, where the constant
     * would do */
    Float spread;

    spread = STRIKE_SPREAD;
    above.x = work->pos.x - (world_gravity.x * STRIKE_HEIGHT);
    above.y = work->pos.y - (world_gravity.y * STRIKE_HEIGHT);
    above.z = work->pos.z - (world_gravity.z * STRIKE_HEIGHT);
    while (count-- > 0) {
        from.x = above.x + (spread * (0.5F - (rand() * RAND_SCALE)));
        from.y = above.y + (spread * (0.5F - (rand() * RAND_SCALE)));
        from.z = above.z + (spread * (0.5F - (rand() * RAND_SCALE)));
        thunder_target(player, &to);
        thunder_start(&from, &to, STRIKE_STEPS);
    }
}

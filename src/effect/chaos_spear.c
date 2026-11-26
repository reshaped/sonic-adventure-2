/* the Chaos Spear: spears of light that close in on a player */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "effect/chaos_spear.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define SPEAR_LEVEL 3 /* a spear's task level */

/* a spear starts SPEAR_START_MOVES moves out from the player. it moves once at
 * each step that brings its timer to a value from 0 to SPEAR_LAST_MOVE. a spear
 * that waits first moves SPEAR_START_MOVES times, and ends on the player. the
 * spear that sets off first starts its timer at 0. it moves a time less, and
 * stops a move short. a spear then holds still until the step that finds its
 * timer at SPEAR_FADE_AT starts its fade. each step of the fade takes
 * SPEAR_FADE_STEP from its alpha, down to -1 */
#define SPEAR_START_MOVES 8.0F
#define SPEAR_LAST_MOVE 7
#define SPEAR_FADE_AT 24
#define SPEAR_FADE_STEP 0.16F
/* what a spear's draw adds to its model's red, green and blue: half dark */
#define SPEAR_SHADE (-0.5F)

/* a spear's timer starts SPEAR_WAIT_STEPS below 0 for each spear that sets
 * off before it. where more than SPEAR_WAVE_AHEAD set off before it, the
 * timer starts SPEAR_WAVE_STEPS lower still */
#define SPEAR_WAIT_STEPS 3
#define SPEAR_WAVE_AHEAD 4
#define SPEAR_WAVE_STEPS 12

/* the slot of model_table whose object a spear draws */
#define SPEAR_MODEL 101

/* a spear's record, in its task's extra */
struct chaos_spear {
    /* the slot of player_tasks whose Sonic record holds the texture list the
     * spear draws with */
    Sint8 owner;
    Sint8 player; /* the slot of player_records the spear closes in on */
    /* a step adds 1 to the timer while game_state.time_stopped is 0. the spear
     * waits, and draws nothing, while the timer is below 0 */
    Sint16 timer;
    /* the constant material's alpha in the spear's draw. under
     * NJD_CONTROL_3D_OFFSET_MATERIAL, it adds to the model's alpha. it is 0
     * until the fade takes it down to -1 */
    Float alpha;
    struct angles ang; /* its pitch and heading, in x and y */
    NJS_POINT3 pos;
    NJS_VECTOR offset; /* where it is from the player while it moves */
    NJS_VECTOR move; /* added to offset each time it moves */
};

static void chaos_spear_origin(int player, NJS_POINT3 *pos)
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

static void chaos_spear_draw(struct task *task)
{
    struct chaos_spear *spear = task->extra.p;

    if (spear->timer >= 0) {
        njFogDisable();
        constant_material_set(spear->alpha, SPEAR_SHADE, SPEAR_SHADE,
                              SPEAR_SHADE);
        njSetTexture(PLAYER_RECORD(spear->owner)->by_character.sonic.texlist);
        njPushMatrixEx();
        njTranslateEx(&spear->pos);
        njRotateXYZ(NULL, spear->ang.x, spear->ang.y, 0);
        njCnkSimpleDrawObject(model_table[SPEAR_MODEL].object);
        njPopMatrixEx();
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njFogEnable();
    }
}

static void chaos_spear_fade(struct task *task)
{
    struct chaos_spear *spear = task->extra.p;

    if (game_state.time_stopped == 0) {
        if (spear->alpha <= -1.0F) {
            task_kill(task);
            return;
        }
        spear->alpha = float_approach(spear->alpha, -1.0F, SPEAR_FADE_STEP);
    }
}

static void chaos_spear_step(struct task *task)
{
    struct chaos_spear *spear = task->extra.p;

    if (game_state.time_stopped == 0) {
        /* @hack redundant: the else after this branch's return */
        if (spear->timer++ >= SPEAR_FADE_AT) {
            spear->timer = 0;
            task->step = chaos_spear_fade;
            return;
        } else if (spear->timer >= 0 && spear->timer <= SPEAR_LAST_MOVE) {
            njAddVector(&spear->offset, &spear->move);
            chaos_spear_origin(spear->player, &spear->pos);
            njAddVector(&spear->pos, &spear->offset);
        }
    }
}

/* a spear's task with its record, which the caller fills. it returns null
 * where the heap can't give both */
static struct task *chaos_spear_task_create(void)
{
    struct task *task;
    struct chaos_spear *spear;

    task = task_create(0, SPEAR_LEVEL, chaos_spear_step,
                       "EffectChaosSpearExecutor");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((spear = syMalloc(sizeof(*spear))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = spear;
        task->draw = chaos_spear_draw;
    }
    return task;
}

/* a spear's move, before its pitch and heading turn it */
static const NJS_VECTOR spear_move = {0.0F, 0.0F, -6.0F};

/* add a spear that closes in on the player along ang. ahead counts the
 * spears that set off before it */
static void chaos_spear_add(int owner, int player, int ahead,
                            struct angles *ang)
{
    struct task *task;
    struct chaos_spear *spear;
    NJS_VECTOR move;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = chaos_spear_task_create()) != NULL) {
        spear = task->extra.p;
        spear->owner = owner;
        spear->player = player;
        spear->ang = *ang;
        move = spear_move;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateXYZ(NULL, ang->x, ang->y, ang->z);
        njCalcVector(NULL, &move, &spear->move);
        njPopMatrixEx();
        spear->offset.x = -(spear->move.x * SPEAR_START_MOVES);
        spear->offset.y = -(spear->move.y * SPEAR_START_MOVES);
        spear->offset.z = -(spear->move.z * SPEAR_START_MOVES);
        chaos_spear_origin(spear->player, &spear->pos);
        njAddVector(&spear->pos, &spear->offset);
        spear->alpha = 0.0F;
        if (ahead > SPEAR_WAVE_AHEAD) {
            spear->timer = -SPEAR_WAVE_STEPS - (ahead * SPEAR_WAIT_STEPS);
        } else {
            spear->timer = -(ahead * SPEAR_WAIT_STEPS);
        }
    }
}

void chaos_spears_start(int owner, int player, int count)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;
    struct angles ang;
    Angle heading;
    Angle spacing;

    heading = RANDOM_TURN();
    spacing = FULL_TURN / count;
    while (count-- > 0) {
        ang.x = (rand() * (HALF_TURN * RAND_SCALE)) - QUARTER_TURN;
        ang.y = heading;
        ang.z = 0;
        chaos_spear_add(owner, player, count, &ang);
        heading += spacing;
    }
}

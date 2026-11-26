/* footsteps: a task that plays a sound at each step of a player's feet */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/footsteps.h"
#include "sound/sound.h"
#include "system/task.h"

#define FOOTSTEPS_LEVEL 3
/* the most players a footsteps task can follow, slots 0 and 1 */
#define FOOTSTEPS_SLOTS 2
/* a Sonic's foot steps where njInnerProduct of its way down and the record's
 * unk_2cc passes SONIC_STEP_DOT */
#define SONIC_STEP_DOT 0.97F

/* the work keeps the player's slot, then each foot's sound and the place it
 * last played. the first foot is the player record's unk_20c, and the second
 * its unk_200 */
#define SLOT(work) ((work)->submode)
#define FIRST_SOUND(work) ((work)->ang.x)
#define SECOND_SOUND(work) ((work)->ang.y)
#define FIRST_PLAYED_AT(work) ((work)->pos)
#define SECOND_PLAYED_AT(work) ((work)->scl)

/* SONIC_STEP_DOT, for a Knuckles */
static Float knuckles_step_dot = 0.65F;

static void footsteps_step(struct task *task);
static void footsteps_destroy(struct task *task);
static void footsteps_draw(struct task *task);

struct task *footsteps_create(int slot, int first_sound, int second_sound,
                              struct task *parent)
{
    struct task *task;
    struct task_work *work;

    if (slot >= FOOTSTEPS_SLOTS || player_work[slot] == NULL) {
        return NULL;
    }
    if (parent != NULL) {
        task = task_create_child(TASK_WORK, footsteps_step, parent);
    } else {
        task = task_create(TASK_WORK, FOOTSTEPS_LEVEL, footsteps_step,
                           "ObjectAsiotoExec");
    }
    if (task != NULL) {
        work = task->work;
        /* @hack spelling: the draw set before the step, where struct task's
         * order would set the step first */
        task->draw = footsteps_draw;
        task->step = footsteps_step;
        task->destructor = footsteps_destroy;
        FIRST_SOUND(work) = first_sound;
        SECOND_SOUND(work) = second_sound;
        work->ang.z = 0;
        SECOND_PLAYED_AT(work) = player_records[slot]->unk_200;
        FIRST_PLAYED_AT(work) = player_records[slot]->unk_20c;
        SLOT(work) = slot;
    }
    return task;
}

static void footsteps_step(struct task *task)
{
    struct task_work *work = task->work;
    Float dot;

    if (player_work[SLOT(work)] == NULL) {
        return;
    }
    if (!(player_work[SLOT(work)]->flags & PLAYER_WORK_GROUNDED)) {
        return;
    }
    if (!(player_records[SLOT(work)]->ground_flags & GROUND_FOOTSTEPS)) {
        return;
    }
    dot = njInnerProduct(&player_records[SLOT(work)]->unk_2cc,
                         &player_records[SLOT(work)]->unk_2a8);
    if ((player_records[SLOT(work)]->character == PLAYER_SONIC &&
         dot > SONIC_STEP_DOT) ||
        (player_records[SLOT(work)]->character == PLAYER_KNUCKLES &&
         dot > knuckles_step_dot)) {
        if (njDistanceP2P(&FIRST_PLAYED_AT(work),
                          &player_records[SLOT(work)]->unk_20c) >
            player_records[SLOT(work)]->consts.radius) {
            sound_play(FIRST_SOUND(work), NULL, 0, 0);
            FIRST_PLAYED_AT(work) = player_records[SLOT(work)]->unk_20c;
        }
    }
    dot = njInnerProduct(&player_records[SLOT(work)]->unk_2cc,
                         &player_records[SLOT(work)]->unk_29c);
    if ((player_records[SLOT(work)]->character == PLAYER_SONIC &&
         dot > SONIC_STEP_DOT) ||
        (player_records[SLOT(work)]->character == PLAYER_KNUCKLES &&
         dot > knuckles_step_dot)) {
        if (njDistanceP2P(&SECOND_PLAYED_AT(work),
                          &player_records[SLOT(work)]->unk_200) >
            player_records[SLOT(work)]->consts.radius) {
            sound_play(SECOND_SOUND(work), NULL, 0, 0);
            SECOND_PLAYED_AT(work) = player_records[SLOT(work)]->unk_200;
        }
    }
}

static void footsteps_destroy(struct task *task)
{
}

static void footsteps_draw(struct task *task)
{
}

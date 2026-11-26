#include <ninja.h>
#include <sg_xpt.h>
#include "boss_last1/contacts.h"
#include "boss_last1/stage.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/last_boss_arena.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"
#include "vmu/vibration.h"

/* a splash's flag: its point lies under the water, or hasn't yet risen clear
 * of it. the point splashes as it falls below SPLASH_HEIGHT, and is clear
 * above CLEAR_HEIGHT */
#define SUBMERGED 1
#define SPLASH_HEIGHT 1.0F
#define CLEAR_HEIGHT 4.0F
/* a splash's sounds play at SPLASH_A_VOLUME and SPLASH_B_VOLUME. within
 * RUMBLE_NEAR of the splash, player 0's pad rumbles by vibration_start's
 * NEAR_POWER, NEAR_FREQ and NEAR_TIME. within RUMBLE_FAR, it rumbles by the
 * FAR_ levels */
#define SPLASH_A_VOLUME 30
#define SPLASH_B_VOLUME 40
#define RUMBLE_NEAR 150.0F
#define NEAR_POWER 3
#define NEAR_FREQ 5
#define NEAR_TIME 1
#define RUMBLE_FAR 250.0F
#define FAR_POWER 1
#define FAR_FREQ 5
#define FAR_TIME 0

/* a swing's flags. SWING_CROSSED: second has crossed first's bearing. it
 * sounds at CROSS_VOLUME once it lies SWING_ANGLE past it. SWING_RAISED:
 * second has risen RAISE_HEIGHT above first. it sounds at FALL_VOLUME once it
 * falls below first's height plus LOWER_HEIGHT */
#define SWING_CROSSED 0x100
#define SWING_RAISED 0x200
#define SWING_ANGLE 512.0F
#define RAISE_HEIGHT 100.0F
#define LOWER_HEIGHT 5.0F
#define CROSS_VOLUME 60
#define FALL_VOLUME 100

/* a tracker's task_work. first, second and last_turn lie in ang's words, and
 * offset in scl.x */
struct contact_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    const NJS_POINT3 *first;
    const NJS_POINT3 *second;
    Angle last_turn;
    NJS_POINT3 pos;
    Float offset;
    Float scl_y;
    Float scl_z;
    struct collision_work *collision;
};
static Float splash_speed = 1.2F;
static Float splash_size = 5.8F;

static void joint_splash_step(struct task *task)
{
    struct contact_work *work = (struct contact_work *)task->work;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    work->pos = *work->first;
    work->pos.y += work->offset;
    if (!(work->flags & SUBMERGED)) {
        if (work->pos.y < SPLASH_HEIGHT) {
            big_splash_create(&work->pos, NULL, splash_size, splash_speed);
            sound_play_at(BIOLIZARD_SOUND_SPLASH_A, NULL, 0, SPLASH_A_VOLUME,
                          &work->pos);
            sound_play_at(BIOLIZARD_SOUND_SPLASH_B, NULL, 0, SPLASH_B_VOLUME,
                          &work->pos);
            if (vector_distance(&work->pos, &player_work[0]->pos) <
                RUMBLE_NEAR) {
                vibration_start(0, NEAR_POWER, NEAR_FREQ, NEAR_TIME);
            }
            if (vector_distance(&work->pos, &player_work[0]->pos) <
                RUMBLE_FAR) {
                vibration_start(0, FAR_POWER, FAR_FREQ, FAR_TIME);
            }
            work->flags |= SUBMERGED;
        }
    } else if (work->pos.y > CLEAR_HEIGHT) {
        work->flags &= ~SUBMERGED;
    }
}

static void joint_splash_destroy(struct task *task)
{
}

struct task *biolizard_track_splash(struct task *parent,
                                    const NJS_POINT3 *point, Float offset)
{
    struct task *task;
    struct contact_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, joint_splash_step, parent);
    if (!task) {
        return NULL;
    }
    work = (struct contact_work *)task->work;
    work->first = point;
    work->offset = offset;
    work->flags |= SUBMERGED;
    task->destructor = joint_splash_destroy;
    return task;
}

static void swing_step(struct task *task)
{
    struct contact_work *work = (struct contact_work *)task->work;
    NJS_POINT3 second;
    NJS_POINT3 first;
    Angle turn;

    if (game_state.time_stopped) {
        return;
    }
    first = *work->first;
    second = *work->second;
    work->pos = second;
    turn = last_boss_angle_difference(&first, &second);
    /* @hack spelling: a branch for each way turn's sign can change, where
     * one test of the change would do */
    if (turn >= 0 && work->last_turn < 0) {
        work->flags |= SWING_CROSSED;
    } else if (turn < 0 && work->last_turn >= 0) {
        work->flags |= SWING_CROSSED;
    }
    if (work->flags & SWING_CROSSED) {
        if (njAbs((Float)turn) > SWING_ANGLE) {
            sound_play_at(BIOLIZARD_SOUND_SWING, NULL, 0, CROSS_VOLUME,
                          &work->pos);
            work->flags &= ~SWING_CROSSED;
        }
    }
    work->last_turn = turn;
    if (second.y - first.y > RAISE_HEIGHT) {
        work->flags |= SWING_RAISED;
    }
    if (work->flags & SWING_RAISED) {
        if (second.y - first.y < LOWER_HEIGHT) {
            sound_play_at(BIOLIZARD_SOUND_SWING, NULL, 0, FALL_VOLUME,
                          &work->pos);
            work->flags &= ~SWING_RAISED;
        }
    }
}

static void swing_destroy(struct task *task)
{
}

struct task *biolizard_track_swing(struct task *parent, const NJS_POINT3 *first,
                                   const NJS_POINT3 *second)
{
    struct task *task;
    struct contact_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, swing_step, parent);
    if (!task) {
        return NULL;
    }
    work = (struct contact_work *)task->work;
    work->first = first;
    work->second = second;
    work->pos = *second;
    work->last_turn = last_boss_angle_difference(first, second);
    task->destructor = swing_destroy;
    return task;
}

/* a garden Chao taking up loose headgear it touches. after examining it, a
 * Chao that has eaten a skeleton dog and wears no hat puts it on. any other
 * Chao throws it over its shoulder */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/carry.h"
#include "chao/fruit.h"
#include "chao/garden.h"
#include "chao/icon.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/parts.h"
#include "chao/sleep.h"
#include "chao/sound.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/animal.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#define THROW_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
/* a Chao turns to the headgear REACH_TURN a frame. once it faces the
 * headgear, it waits REACH_FRAMES frames for the headgear to touch it */
#define REACH_TURN 0x600
#define REACH_FRAMES 60
static int take_headgear(struct task *chao);
static int examine_headgear(struct task *chao);
static int wear_headgear(struct task *chao);
static int throw_headgear(struct task *chao);

int chao_try_take_headgear(struct task *chao)
{
    chao_behavior_fn action = chao_current_behavior(chao);
    struct task *item;
    if (action != chao_eat_fruit_behavior && action != chao_sleep_behavior &&
        action != take_headgear) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((item = collision_first_with_id(chao, GARDEN_HEADGEAR_HIT)) !=
                NULL &&
            !garden_object_locked(item)) {
            mover_set_target(chao, &item->work->pos);
            chao_set_behavior(chao, take_headgear);
            return TRUE;
        }
    }
    return FALSE;
}

static int take_headgear(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task *item;
    /* @hack spelling: a switch of one case, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_CAPTURE_MORAU, 15);
        behavior->mode++;
        behavior->timer = REACH_FRAMES;
        break;
    }
    if (mover_turn_to_target_by(chao, REACH_TURN) < (Angle)ANGLE_PER_DEGREE) {
        item = collision_first_with_id(chao, GARDEN_HEADGEAR_HIT);
        if (item) {
            if (!garden_object_locked(item)) {
                player_let_go(0);
                chao_take_in_both_hands(chao, item);
                chao_set_behavior(chao, examine_headgear);
                if (chao_has_eaten_minimal(chao, ANIMAL_BONE) &&
                    !CHAO_WORK(chao)->profile.parameters.hat) {
                    chao_queue_behavior(chao, wear_headgear);
                } else {
                    chao_queue_behavior(chao, throw_headgear);
                }
            }
        } else if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
    }
    return BEHAVIOR_CONTINUE;
}

static int examine_headgear(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    /* @hack spelling: a switch of one case, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_MOTU, 35);
        behavior->mode++;
        chao_show_mark(chao, CHAO_MARK_QUESTION, 120);
        sound_play_at(GARDEN_QUESTION_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
        behavior->timer = RANDOM_COUNT(120, 61);
        break;
    }
    if (behavior->timer-- <= 0 || !chao_place_held_item(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static int wear_headgear(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct garden_entry *entry;
    struct task_work *item;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_KABURU, 35);
        behavior->mode++;
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 120);
        break;
    case 1:
        if (chao_motion_held(chao)) {
            entry = garden_mutual_partner(chao);
            if (entry && entry->task) {
                item = entry->task->work;
                chao_set_headgear(chao, HEADGEAR_KIND(item));
                task_kill(entry->task);
            }
            /* @bug logic: case 2's wait for CHAO_ANIM_KABURU_END never runs,
             * since nothing advances mode past 1. the behavior ends once
             * chao_place_held_item finds the hat gone. mode's only write is
             * at 0x0c533a86, and case 2 is at 0x0c533ad2 */
            chao_link_motion_frames(chao, CHAO_ANIM_KABURU_END, 35);
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (!chao_place_held_item(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static int throw_headgear(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct garden_entry *entry;
    struct task *item;
    struct task_work *held_work;
    Angle heading;
    NJS_VECTOR velocity;
    Float x;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_THROW, 35);
        behavior->mode++;
        break;
    case 1:
        if (chao_motion_ended(chao)) {
            entry = garden_mutual_partner(chao);
            if (entry) {
                item = entry->task;
                /* @hack redundant: the reads of the item's work and the
                 * thrower's x, which nothing uses */
                held_work = item->work;
                x = chao->work->pos.x;
                (void)held_work;
                (void)x;
                heading = chao->work->ang.y + THROW_ANGLE;
                velocity.x = njSin(heading) * 0.3F;
                velocity.y = 0.7F;
                velocity.z = njCos(heading) * 0.3F;
                mover_set_spd(item, &velocity);
            }
            garden_separate_partners(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (!chao_place_held_item(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

/* a racer's behaviors at the course's obstacles: the box, the tree and its
 * fruit, and the scavenger hunt */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/carry.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chaostgrace/movement.h"
#include "chaostgrace/obstacles.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "chaostgrace/racer.h"
#include "chaostgrace/scavenger.h"
#include "object/mover.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a racer stopped at an obstacle keeps SLOWING of its speed each frame on the
 * floor */
#define SLOWING 0.9F
/* a racer's reach is a RACE_FIELD_ENTRANT field of REACH_RADIUS, REACH_AHEAD
 * in front of it and REACH_RISE up. it lasts REACH_FRAMES frames. a box opens
 * to it, and a tree shakes */
#define REACH_AHEAD 3.0F
#define REACH_RISE 1.0F
#define REACH_RADIUS 1.5F
#define REACH_FRAMES 2
/* a racer at a box understands it after more than UNDERSTAND_FRAMES frames.
 * opening it, the racer reaches for it each frame after the first
 * OPEN_DELAY */
#define UNDERSTAND_FRAMES 60
#define OPEN_DELAY 60
/* a racer looks for a toy of its choice PICKY_SEARCHES times, then takes any
 * toy. each search comes after SEARCH_FRAMES frames */
#define PICKY_SEARCHES 5
#define SEARCH_FRAMES 60
/* a racer bringing a toy waits its turn within LANE_NEAR of its lane. it
 * submits the toy within LANE_REACHED. it pushes at BRING_SPEED of
 * chao_tuning.walk_speed, and its motion plays at BRING_RATE times its speed */
#define LANE_NEAR 15.0F
#define LANE_REACHED 2.0F
#define BRING_SPEED 0.8F
#define BRING_RATE 18.0F
/* a racer chooses race_scavenger_target at a chance of RIGHT_CHANCE for each
 * point of its intelligence. else it chooses any kind */
#define RIGHT_CHANCE 0.0001F
/* the frames each behavior shows its mark */
#define NOTICE_MARK 120
#define UNDERSTAND_MARK 40
#define THINK_MARK 60
#define STARTLED_MARK 300
#define SEARCH_MARK 60
/* a racer with the right toy poses for VICTORY_FRAMES frames. a wrong toy's
 * penalty knocks it back at KNOCK_BACK and up at KNOCK_UP, and startles it
 * for PENALTY_FRAMES */
#define VICTORY_FRAMES 120
#define KNOCK_BACK 0.2F
#define KNOCK_UP 1.0F
#define PENALTY_FRAMES 500
/* the steps of a turn, in angle units a frame */
#define TREE_TURN 0x280
#define LANE_TURN 0x100

static int wait_in_lane(struct task *chao);
static int wrong_answer(struct task *chao);

int race_wait_at_box(struct task *chao)
{
    /* @hack redundant: mover, a local nothing reads */
    struct mover *mover = chao->motion.p;
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_STAND);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_notice_obstacle(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_OSYABURI);
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, NOTICE_MARK);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_understand_box(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HIRAMEKI);
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, UNDERSTAND_MARK);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        if (behavior->timer++ > UNDERSTAND_FRAMES) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_think_at_obstacle(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HATENA_HATE2);
        chao_show_mark(chao, CHAO_MARK_QUESTION, THINK_MARK);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

#ifdef NONMATCHING
/* this side returns the field, or NULL where it can't make it */
static struct task *reach_forward(struct task *chao, NJS_POINT3 *position)
{
    struct chao_work *work = CHAO_WORK(chao);

    position->x = work->base.pos.x + (REACH_AHEAD * njSin(work->base.ang.y));
    position->y = work->base.pos.y + REACH_RISE;
    position->z = work->base.pos.z + (REACH_AHEAD * njCos(work->base.ang.y));
    return garden_create_timed_field(chao, RACE_FIELD_ENTRANT, position,
                                     REACH_RADIUS, REACH_FRAMES);
}
#define REACH_FORWARD() reach_forward(chao, &position)
#else
#define REACH_FORWARD()                                                        \
    do {                                                                       \
        position.x =                                                           \
            work->base.pos.x + (REACH_AHEAD * njSin(work->base.ang.y));        \
        position.y = work->base.pos.y + REACH_RISE;                            \
        position.z =                                                           \
            work->base.pos.z + (REACH_AHEAD * njCos(work->base.ang.y));        \
        garden_create_timed_field(chao, RACE_FIELD_ENTRANT, &position,         \
                                  REACH_RADIUS, REACH_FRAMES);                 \
    } while (0)

#endif

int race_open_box(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    NJS_POINT3 position;

    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_MOTU);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        if (behavior->timer++ > OPEN_DELAY) {
            REACH_FORWARD();
        }
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

#define STARTLED()                                                             \
    do {                                                                       \
        chao_link_motion(chao, CHAO_ANIM_KUSAI_BATAN);                         \
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);         \
        chao_set_timed_mouth(chao, CHAO_MOUTH_DISTRESSED, CHAO_FACE_HOLD);     \
        chao_show_mark(chao, CHAO_MARK_ANGER, STARTLED_MARK);                  \
        RACE_ADVANCE(behavior);                                                \
    } while (0)

int race_startled(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        STARTLED();
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_approach_tree_item(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct mover *mover = chao->motion.p;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_walk_motion(chao);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    /* @hack spelling: the target through CHAO_WORK(chao), where
     * work->race_target would do */
    garden_turn_to_object(chao, CHAO_WORK(chao)->race_target, TREE_TURN);
    if (mover->flags & MOVER_ON_FLOOR) {
        race_run_forward(chao);
    }
    return BEHAVIOR_CONTINUE;
}

int race_shake_tree(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    NJS_POINT3 position;

    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_YURASU_SWING);
        RACE_ADVANCE(behavior);
#ifdef NONMATCHING
        /* this side reaches forward in the next phase, which repeats a reach
         * that can't make its field */
        behavior->phase = 1;
#else
        REACH_FORWARD();
#endif
        /* fall through */
    case 1:
#ifdef NONMATCHING
        /* this side reaches forward until reach_forward makes the field, and
         * only then looks for the fruit */
        if (behavior->phase) {
            if (!REACH_FORWARD()) {
                break;
            }
            behavior->phase = 0;
        }
#endif
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, RACE_FIELD_FRUIT_DROP)) {
            struct task *fruit = race_tree_fruit(chao);
            /* @hack spelling: the target through CHAO_WORK(chao), where
             * work->race_target would do */
            CHAO_WORK(chao)->race_target = fruit;
            if (fruit) {
                chao_set_behavior(chao, race_approach_tree_item);
            } else {
                chao_set_behavior(chao, race_run);
            }
        }
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_eat_fruit(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_EAT);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    garden_command_partner(chao, PARTNER_USE);
    if (garden_take_command(chao) == PARTNER_USED_UP) {
        chao_set_behavior(chao, race_run);
    }
    if (!chao_place_held_item(chao)) {
        chao_set_behavior(chao, race_run);
    }
    return BEHAVIOR_CONTINUE;
}

int race_choose_answer(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    Uint16 intelligence = chao_skill_points(chao, CHAO_INTELLIGENCE);

    if (intelligence * RIGHT_CHANCE > rand() * RAND_SCALE) {
        work->race_choice = race_scavenger_target;
    } else {
        work->race_choice = (int)(rand() * (TOY_KIND_DRAW * RAND_SCALE));
    }
    return BEHAVIOR_FINISH;
}

int race_find_answer_item(struct task *chao)
{
    /* @hack redundant: mover, a local nothing reads */
    struct mover *mover = chao->motion.p;
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    int index;

    switch (behavior->mode) {
    case 0:
        behavior->count = 0;
        /* fall through */
    case 1:
        chao_link_motion(chao, CHAO_ANIM_HATENA_HATE2);
        chao_show_mark(chao, CHAO_MARK_QUESTION, SEARCH_MARK);
        behavior->mode++;
        behavior->timer = 0;
        /* fall through */
    case 2:
        if (behavior->timer++ > SEARCH_FRAMES) {
#ifdef NONMATCHING
            /* whether a toy failed to spawn, which this side's racer then stops
             * waiting for */
            Bool shortage = race_retry_scavenger_items();
#endif
            for (index = 0; index < SCAVENGER_ITEMS; index++) {
                struct task *item = race_scavenger_items[index];
#ifdef NONMATCHING
                /* the plain form the hack tag names, which also takes the toy
                 * only where garden_set_target accepts it */
                if (item &&
                    (behavior->count >= PICKY_SEARCHES ||
                     TOY_KIND(item->work) == work->race_choice) &&
                    garden_set_target(chao, item)) {
                    race_scavenger_items[index] = NULL;
                    return BEHAVIOR_FINISH;
                }
#else
                /* @hack spelling: the toy taken in two branches, where one
                 * test of the count or the kind would do */
                if (item) {
                    if (behavior->count < PICKY_SEARCHES) {
                        if (TOY_KIND(item->work) == work->race_choice) {
                            garden_set_target(chao, item);
                            race_scavenger_items[index] = NULL;
                            return BEHAVIOR_FINISH;
                        }
                    } else {
                        garden_set_target(chao, item);
                        race_scavenger_items[index] = NULL;
                        return BEHAVIOR_FINISH;
                    }
                }
#endif
            }
#ifdef NONMATCHING
            /* a racer past PICKY_SEARCHES that finds no toy while one failed
             * to spawn runs on. it would otherwise wait for a toy that may
             * never spawn */
            if (shortage && behavior->count >= PICKY_SEARCHES) {
                chao_set_behavior(chao, race_run);
                return BEHAVIOR_CONTINUE;
            }
#endif
            behavior->mode = 1;
            behavior->count++;
        }
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_bring_answer(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;
    Float speed;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_HAKOBU);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        race_join_scavenger_lane(chao);
        /* @hack spelling: the flags through CHAO_WORK(chao), where
         * work->flags would do */
        CHAO_WORK(chao)->flags &= ~CHAO_COURSE_AIM;
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    mover_turn_to_target_by(chao, LANE_TURN);
    if (chao_target_distance_squared(chao) < LANE_NEAR * LANE_NEAR) {
        struct task *first = race_scavenger_occupant(work->race_slot);
        if (first != chao) {
            chao_set_behavior(chao, wait_in_lane);
        }
        if (chao_target_distance_squared(chao) < LANE_REACHED * LANE_REACHED) {
            chao_set_behavior(chao, race_submit_answer);
        }
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x =
            njSin(work->base.ang.y) * chao_tuning.walk_speed * BRING_SPEED;
        mover->acc.z =
            njCos(work->base.ang.y) * chao_tuning.walk_speed * BRING_SPEED;
    }
    speed = njScalor(&mover->spd);
    chao_set_motion_speed(chao, speed * BRING_RATE);
    chao_slow_on_floor(chao, SLOWING);
    if (!chao_place_held_item(chao)) {
        chao_set_behavior(chao, race_run);
    }
    return BEHAVIOR_CONTINUE;
}

static int wait_in_lane(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_MOTU);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    {
        struct task *first = race_scavenger_occupant(work->race_slot);
        if (first == chao) {
            chao_set_behavior(chao, race_bring_answer);
        }
    }
    chao_slow_on_floor(chao, SLOWING);
    if (!chao_place_held_item(chao)) {
        chao_set_behavior(chao, race_run);
    }
    return BEHAVIOR_CONTINUE;
}

int race_submit_answer(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    NJS_VECTOR velocity;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_link_motion(chao, CHAO_ANIM_MOTU);
        /* @hack spelling: the lane through CHAO_WORK(chao), where
         * work->race_slot would do */
        race_scavenger_submitted[CHAO_WORK(chao)->race_slot] = TRUE;
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        RACE_ADVANCE(behavior);
        /* fall through */
    case 1:
        break;
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, RACE_FIELD_RIGHT_ANSWER)) {
        chao_set_timed_behavior(chao, race_victory_pose, VICTORY_FRAMES);
        chao_queue_behavior(chao, race_run);
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, RACE_FIELD_PENALTY)) {
        velocity.x = njSin(work->base.ang.y + HALF_TURN) * KNOCK_BACK;
        velocity.y = KNOCK_UP;
        velocity.z = njCos(work->base.ang.y + HALF_TURN) * KNOCK_BACK;
        mover_set_spd(chao, &velocity);
        chao_set_timed_behavior(chao, wrong_answer, PENALTY_FRAMES);
        chao_queue_behavior(chao, race_run);
    }
    chao_slow_on_floor(chao, SLOWING);
    if (!chao_place_held_item(chao)) {
        chao_set_behavior(chao, race_run);
    }
    return BEHAVIOR_CONTINUE;
}

static int wrong_answer(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        STARTLED();
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

int race_fall_in_pit(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;

    /* @hack spelling: a switch on the mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        STARTLED();
        /* fall through */
    case 1:
        break;
    }
    chao_slow_on_floor(chao, SLOWING);
    return BEHAVIOR_CONTINUE;
}

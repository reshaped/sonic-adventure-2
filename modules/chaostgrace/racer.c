/* a racer's step. each frame the racer meets the fields along its course,
 * checks the water, and runs its behavior queue. a field sets the racer's
 * floor, or the behavior its stretch of the course needs */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/movement.h"
#ifdef NONMATCHING
#else
/* garden_objects_running, declared at its address as the call below makes it */
extern Bool garden_objects_running_as_called(void);
#endif
#include "chao/carry.h"
#include "chao/face.h"
#include "chao/fruit.h"
#include "chao/ground.h"
#include "chao/icon.h"
#include "chao/idle.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/race.h"
#include "chao/residents.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chaostgrace/grid.h"
#include "chaostgrace/movement.h"
#include "chaostgrace/obstacles.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "chaostgrace/racer.h"
#include "chaostgrace/track_events.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

#define WATER_ENTERED 1
#define IN_WATER 4
#define SPLASH_SOUND 0x1020

Bool race_check_water(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct mover *mover = chao->motion.p;
    struct chao_behavior *behavior = &work->behavior;
    Bool submerged = FALSE;

    switch (garden_race_course()->track) {
    case TRACK_BEG_SWIM:
        race_water_span = beg_swim_water;
        break;
    case TRACK_BEG_FLY:
        race_water_span = beg_fly_water;
        break;
    case TRACK_BEG_RUN:
        race_water_span = beg_run_water;
        break;
    case TRACK_BEG_POWER:
        race_water_span = beg_power_water;
        break;
    case TRACK_JWL_SWIM:
        race_water_span = jwl_swim_water;
        break;
    case TRACK_JWL_FLY:
        race_water_span = jwl_fly_water;
        break;
    case TRACK_JWL_RUN:
        race_water_span = jwl_run_water;
        break;
    case TRACK_JWL_POWER:
        race_water_span = jwl_power_water;
        break;
    case TRACK_JWL_IL:
        race_water_span = jwl_il_water;
        break;
    case TRACK_JWL_LONG:
        race_water_span = jwl_long_water;
        break;
    }
    mover->flat_y = RACE_NO_WATER;
    while (race_water_span->start >= 0.0F) {
        if (race_water_span->start < work->race_progress &&
            work->race_progress < race_water_span->end) {
            mover->flat_y = race_water_span->height;
            break;
        }
        race_water_span++;
    }
    if (mover->flat_y - RACE_SUBMERGED_DEPTH > work->base.pos.y) {
        submerged = TRUE;
    }
    if (submerged) {
        if (!(behavior->flags & WATER_ENTERED)) {
            behavior->flags |= WATER_ENTERED;
            /* @hack spelling: the position through chao->work, where
             * work->base.pos would do */
            sound_play_at(SPLASH_SOUND, NULL, 0, 0, &chao->work->pos);
            chao_set_behavior(chao, race_swim);
        }
        behavior->flags |= IN_WATER;
    } else {
        behavior->flags &= ~WATER_ENTERED;
        behavior->flags &= ~IN_WATER;
    }
    return submerged;
}

/* each frame, a racer runs its track's step from race_track_steps. only the
 * beginner swim track's does anything */
void race_step_beg_swim(struct task *chao)
{
    /* @hack redundant: behavior, a local nothing reads */
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    RACE_USE_STAGE(chao);
}

/* each other track's step, a function of its own that does nothing */
/* @hack redundant: behavior, a local nothing reads, and a read of the flags
 * that nothing uses */
#define EMPTY_TRACK_STEP(name)                                                 \
    void name(struct task *chao)                                               \
    {                                                                          \
        struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;           \
        (void)CHAO_WORK(chao)->flags;                                          \
    }
EMPTY_TRACK_STEP(race_step_beg_fly)
EMPTY_TRACK_STEP(race_step_beg_run)
EMPTY_TRACK_STEP(race_step_beg_power)
EMPTY_TRACK_STEP(race_step_jwl_swim)
EMPTY_TRACK_STEP(race_step_jwl_fly)
EMPTY_TRACK_STEP(race_step_jwl_run)
EMPTY_TRACK_STEP(race_step_jwl_power)
EMPTY_TRACK_STEP(race_step_jwl_il)
EMPTY_TRACK_STEP(race_step_jwl_long)

/* the course's width in eighths, one for each pit. the racer's lane becomes
 * the middle of the eighth its pit stands in */
#define PIT_WIDTH 0.125F
#define PIT_MIDDLE 0.0625F

static void choose_pitfall_lane(struct task *chao)
{
    /* @hack spelling: lane_width, a local holding PIT_WIDTH, where PIT_WIDTH
     * would do */
    Float lane_width = PIT_WIDTH;
    CHAO_WORK(chao)->lane = (lane_width * race_choose_pitfall()) + PIT_MIDDLE;
}

/* the fields along a course that set a racer's behavior or its floor, by
 * id. props.h's race_field names the box's and the surprise's, and
 * chao/fruit.h's GARDEN_FRUIT_HIT the fruit's */
enum race_trigger {
    TRIGGER_TREE = 137,
    TRIGGER_FIND_TREE = 178,
    TRIGGER_FLY = 179,
    TRIGGER_JUMP = 180,
    TRIGGER_FINISH = 184,
    TRIGGER_STEER = 185,
    TRIGGER_QUESTION = 187,
    TRIGGER_ITEMS = 188,
    TRIGGER_PITFALL = 189,
    TRIGGER_STOP = 190,
    TRIGGER_COURSE_FLOOR = 191,
    TRIGGER_STAGE_FLOOR = 192,
    TRIGGER_BOTH_FLOORS = 193
};

/* a racer at a box notices it for BOX_NOTICE frames. it then thinks, opens
 * the box for BOX_OPEN and waits BOX_WAIT more. one at the question notices
 * it for QUESTION_NOTICE */
#define BOX_NOTICE 20
#define BOX_OPEN 80
#define BOX_WAIT 50
#define QUESTION_NOTICE 30
/* THINK_FRAMES is the frames a racer thinks at an obstacle, fewer the more
 * intelligent it is */
#define THINK_POINTS 15000
#define POINTS_PER_FRAME 50
#define THINK_FRAMES(chao)                                                     \
    ((THINK_POINTS - chao_skill_points((chao), CHAO_INTELLIGENCE)) /           \
     POINTS_PER_FRAME)
#define STARTLED_FRAMES 500

static void course_events(struct task *chao)
{
    const struct garden_race_course *course = garden_race_course();
    chao_behavior_fn action = chao_current_behavior(chao);

    race_track_steps[course->track](chao);
    if (action == race_run || action == race_trip) {
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, RACE_FIELD_BOX)) {
            chao_set_timed_behavior(chao, race_notice_obstacle, BOX_NOTICE);
            chao_queue_timed_behavior(chao, race_think_at_obstacle,
                                      THINK_FRAMES(chao));
            chao_queue_behavior(chao, race_understand_box);
            chao_queue_timed_behavior(chao, race_open_box, BOX_OPEN);
            chao_queue_timed_behavior(chao, race_wait_at_box, BOX_WAIT);
            chao_queue_behavior(chao, race_run);
            CHAO_WORK(chao)->flags &= ~CHAO_RACE_FIXED_HEADING;
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_FLY)) {
            RACE_USE_STAGE(chao);
            chao_set_behavior(chao, race_fly);
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_JUMP)) {
            RACE_USE_STAGE(chao);
            chao_set_behavior(chao, race_jump);
        }
        if ((CHAO_WORK(chao)->flags & CHAO_RACE_FIXED_HEADING) &&
            garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_STEER)) {
            CHAO_WORK(chao)->flags &= ~CHAO_RACE_FIXED_HEADING;
        }
        if (!(CHAO_WORK(chao)->flags & CHAO_COURSE_AIM_NEAR) &&
            garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_PITFALL)) {
            CHAO_WORK(chao)->flags |= CHAO_COURSE_AIM_NEAR;
            choose_pitfall_lane(chao);
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_FIND_TREE)) {
            CHAO_WORK(chao)->race_target = race_claim_tree();
            chao_set_behavior(chao, race_approach_tree_item);
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_QUESTION)) {
            chao_set_timed_behavior(chao, race_notice_obstacle,
                                    QUESTION_NOTICE);
            chao_queue_timed_behavior(chao, race_think_at_obstacle,
                                      THINK_FRAMES(chao));
            chao_queue_behavior(chao, race_choose_answer);
            chao_queue_behavior(chao, race_run_to_items);
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_FINISH)) {
            chao_set_behavior(chao, race_cross_finish);
            race_record_finish(chao);
            RACE_USE_STAGE(chao);
        }
    }
    if (action == race_fly) {
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_FINISH)) {
            chao_set_behavior(chao, race_cross_finish);
            race_record_finish(chao);
            RACE_USE_STAGE(chao);
        }
    }
    if (chao_current_behavior(chao) == race_cross_finish) {
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_STOP)) {
            chao_set_behavior(chao, race_end_finish);
            RACE_USE_STAGE(chao);
        }
    }
    if (action == race_run_to_items) {
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_ITEMS)) {
            chao_set_behavior(chao, race_find_answer_item);
            chao_queue_behavior(chao, chao_walk_to_target_behavior);
            chao_queue_behavior(chao, chao_pick_up_target_behavior);
            chao_queue_behavior(chao, race_bring_answer);
            chao_queue_behavior(chao, race_submit_answer);
        }
    }
    if (action == race_approach_tree_item) {
        struct task *object;
        object = garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_TREE);
        if (object && object == CHAO_WORK(chao)->race_target) {
            chao_set_behavior(chao, race_shake_tree);
        }
        object = garden_hit_kind(chao, CHAO_PUSH_SHAPE, GARDEN_FRUIT_HIT);
        if (object && object == CHAO_WORK(chao)->race_target) {
            chao_set_behavior(chao, race_eat_fruit);
            chao_take_in_both_hands(chao, CHAO_WORK(chao)->race_target);
            CHAO_WORK(chao)->race_target = NULL;
        }
    }
    if (chao_current_behavior(chao) != race_startled) {
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, RACE_FIELD_SURPRISE)) {
            chao_set_timed_behavior(chao, race_startled, STARTLED_FRAMES);
            chao_queue_behavior(chao, chao_stand_up_behavior);
            chao_queue_behavior(chao, race_run);
        }
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_COURSE_FLOOR)) {
        CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
        CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_STAGE_FLOOR)) {
        RACE_USE_STAGE(chao);
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, TRIGGER_BOTH_FLOORS)) {
        CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
        CHAO_WORK(chao)->flags |= CHAO_RACE_STAGE_COLLISION;
    }
}

/* a racer takes a new lane at a chance of LANE_CHANCE every LANE_PERIOD
 * frames. it doesn't while it climbs or has CHAO_COURSE_AIM_NEAR. the lane
 * lies at least LANE_EDGE from either edge */
#define LANE_PERIOD 200
#define LANE_CHANCE 0.3F
#define LANE_EDGE 0.1F
#define LANE_SPAN 0.8F

static void think(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    int result;

    if (!(work->flags & CHAO_RUNS_BEHAVIOR)) {
        return;
    }
    course_events(chao);
    if (race_mode() < RACE_PODIUM) {
        race_check_water(chao);
    }
    /* @hack spelling: the flags through CHAO_WORK(chao), where work->flags
     * would do */
    if (!(CHAO_WORK(chao)->flags & CHAO_COURSE_AIM_NEAR) &&
        chao_current_behavior(chao) != race_climb &&
        work->frames % LANE_PERIOD == 0 && rand() * RAND_SCALE < LANE_CHANCE) {
        work->lane = (rand() * (LANE_SPAN * RAND_SCALE)) + LANE_EDGE;
    }
    if (behavior->queue[behavior->current]) {
        result = behavior->queue[behavior->current](chao);
    } else {
        result = BEHAVIOR_FINISH;
    }
    if (behavior->frames[behavior->current] > 0) {
        behavior->frames[behavior->current]--;
        if (behavior->frames[behavior->current] == 0) {
            result = BEHAVIOR_FINISH;
        }
    }
    if (result == BEHAVIOR_FINISH) {
        if (!chao_advance_behavior(chao)) {
            chao_set_behavior(chao, race_run);
        }
    }
}

static void racer_step(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);

    /* @hack spelling: a switch on mode, where an if would do */
    switch (work->base.mode) {
    case 0:
        chao_set_behavior(chao, race_wait_for_start);
        /* @hack spelling: the step through chao->work, where work->base.mode
         * would do */
        chao->work->mode++;
        break;
    }
    /* @bug mismatched-call: the call at 0x0c9ccf50 passes
     * garden_objects_running no task, which it takes and never reads */
#ifdef NONMATCHING
    /* this side keeps the tag's result, as garden_objects_running reads no
     * argument. only symbols.txt places garden_objects_running_as_called, so
     * this side calls the function by its declared name */
    if (!garden_objects_running(chao)) {
        return;
    }
#else
    if (!garden_objects_running_as_called()) {
        return;
    }
#endif
    think(chao);
    chao_motion_step(chao);
    chao_icon_step(chao);
    chao_face_step(chao);
    chao_step_body(chao);
    mover_step(chao);
    chao_meet_floor(chao);
    work->frames++;
    if (work->flags & CHAO_COLLIDES) {
        collision_register(chao);
    } else {
        collision_reset(chao);
    }
    /* @hack spelling: a switch on the body, where an if would do */
    switch (work->profile.parameters.body) {
    case CHAO_BODY_MINIMAL:
        motion_step(&work->minimal_motion);
        break;
    }
}

static void racer_destroy(struct task *chao)
{
    chao_free_body(chao);
    chao_behavior_destroy(chao);
    garden_unregister_object(chao);
}

static void racer_draw(struct task *chao)
{
    chao_actor_draw(chao);
}

struct task *race_enter_saved_chao(struct chao_save *save, NJS_POINT3 *position,
                                   Angle yaw)
{
    struct task *chao = garden_spawn_chao(save, position, yaw);
#ifdef NONMATCHING
    /* this side returns null where garden_spawn_chao makes no Chao. the
     * matching side writes through the null */
    if (!chao) {
        return NULL;
    }
#endif
    chao->step = racer_step;
    chao->destructor = racer_destroy;
    chao->draw = racer_draw;
    CHAO_WORK(chao)->flags |= CHAO_COURSE_AIM;
    return chao;
}

struct task *race_create_opponent(const struct chao_gene *gene,
                                  const struct chao_profile *profile,
                                  const struct garden_race_chao *appearance,
                                  NJS_POINT3 *position, Angle yaw)
{
    struct task *chao =
        chao_create(gene, profile, appearance, position, yaw, NULL);
#ifdef NONMATCHING
    /* this side returns null where chao_create makes no Chao. the matching side
     * writes through the null */
    if (!chao) {
        return NULL;
    }
#endif
    chao->step = racer_step;
    chao->destructor = racer_destroy;
    chao->draw = racer_draw;
    ((struct mover *)chao->motion.p)->flags &= ~MOVER_CAN_REST;
    CHAO_WORK(chao)->flags |= CHAO_COURSE_AIM;
    return chao;
}

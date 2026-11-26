/* a garden Chao taking up a seed it touches, which it then examines */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/behavior.h"
#include "chao/carry.h"
#include "chao/fruit.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/planting.h"
#include "chao/sleep.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"

/* a Chao turns to the seed REACH_TURN a frame. once it faces the seed, it
 * waits REACH_FRAMES frames for the seed to touch it */
#define REACH_TURN 0x600
#define REACH_FRAMES 60

static int take_seed(struct task *chao);

int chao_try_take_seed(struct task *chao)
{
    chao_behavior_fn action = chao_current_behavior(chao);
    struct task *seed;
    if (action == chao_sleep_behavior || action == chao_eat_fruit_behavior) {
        return FALSE;
    }
    if (chao_skill_points(chao, CHAO_RUN) < chao_tuning.walk_slow_points) {
        return FALSE;
    }
    if (garden_mutual_partner(chao)) {
        return FALSE;
    }
    if (action == take_seed) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((seed = collision_first_with_id(chao, GARDEN_SEED_HIT)) != NULL &&
        !garden_object_locked(seed)) {
        mover_set_target(chao, &seed->work->pos);
        chao_set_behavior(chao, take_seed);
        return TRUE;
    }
    return FALSE;
}

static int take_seed(struct task *chao)
{
    struct chao_behavior *behavior =
        &((struct chao_work *)chao->work)->behavior;
    struct task *seed;
    /* @hack spelling: a switch of one case, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_CAPTURE_MORAU, 15);
        behavior->mode++;
        behavior->timer = REACH_FRAMES;
        break;
    }
    if (mover_turn_to_target_by(chao, REACH_TURN) < (Angle)ANGLE_PER_DEGREE) {
        seed = collision_first_with_id(chao, GARDEN_SEED_HIT);
        if (seed) {
            if (!garden_object_locked(seed)) {
                player_let_go(0);
                chao_take_in_both_hands(chao, seed);
                chao_set_behavior(chao, chao_examine_seed);
            }
        } else if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
    }
    return BEHAVIOR_CONTINUE;
}

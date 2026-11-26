#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/race.h"
#include "chao/race_opponents.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chaostgentrance/entrance.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

struct task *entrance_racers[RACE_ENTRANTS];
/* each racer's place off the elevator, where entrance_release_racers walks
 * its Chao */
static NJS_POINT3 places[RACE_ENTRANTS] = {
#include "chaostgentrance/racers/places.inc"
};

/* a racer's Chao spawns on the elevator at a random angle about the point
 * 0, SPAWN_Y, SPAWN_Z. it stands NEAREST to NEAREST + SPREAD from the point */
#define SPAWN_Y (-16.0F)
#define SPAWN_Z (-75.0F)
#define NEAREST 1.0F
#define SPREAD 8.0F

static int wait_on_elevator(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    /* @hack spelling: a switch of one case, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_OSYABURI);
        behavior->mode++;
        break;
    }
    return BEHAVIOR_CONTINUE;
}

void entrance_racers_create(void)
{
    NJS_POINT3 pos;
    Float radius;
    Angle angle;
    int slot;

    if (garden_race_group() != RACE_MULTI) {
        garden_race_build_opponents();
        for (slot = 0; slot < RACE_OPPONENTS; slot++) {
            if (race_opponents[slot].record.head[0].type > CHAO_EGG) {
                garden_race_entrants[slot + 1] = &race_opponents[slot];
            } else {
                garden_race_entrants[slot + 1] = NULL;
            }
        }
    }
    for (slot = 0; slot < RACE_ENTRANTS; slot++) {
        entrance_racers[slot] = NULL;
    }
    for (slot = 0; slot < RACE_ENTRANTS; slot++) {
        if (garden_race_entrants[slot]) {
            radius = (rand() * (SPREAD / (RAND_MAX + 1))) + NEAREST;
            angle = RANDOM_TURN();
            /* @hack redundant: 0.0F added to the x, which changes nothing */
            pos.x = 0.0F + (radius * njSin(angle));
            pos.y = SPAWN_Y;
            pos.z = SPAWN_Z + (radius * njCos(angle));
            entrance_racers[slot] = garden_spawn_chao(
                garden_race_entrants[slot], &pos, RANDOM_TURN());
            if (entrance_racers[slot]) {
                chao_set_behavior(entrance_racers[slot], wait_on_elevator);
                mover_set_target(entrance_racers[slot], &places[slot]);
                CHAO_WORK(entrance_racers[slot])->flags |= CHAO_NO_INTENTIONS;
                CHAO_WORK(entrance_racers[slot])->flags &= ~CHAO_BODY_SPRINGS;
            }
        }
    }
}

void entrance_release_racers(void)
{
    int slot;
    for (slot = 0; slot < RACE_ENTRANTS; slot++) {
        if (entrance_racers[slot]) {
            chao_set_behavior(entrance_racers[slot], chao_face_point_behavior);
            chao_queue_behavior(entrance_racers[slot],
                                chao_walk_to_point_behavior);
            chao_queue_behavior(entrance_racers[slot],
                                chao_turn_in_place_behavior);
        }
    }
}

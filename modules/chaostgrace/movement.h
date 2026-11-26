/* a racer's behaviors as it runs, jumps, flies, swims and climbs the course.
 * each is a chao_behavior_fn that returns BEHAVIOR_CONTINUE */
#ifndef CHAOSTGRACE_MOVEMENT_H
#define CHAOSTGRACE_MOVEMENT_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* push the racer along its heading, harder with its run skill */
void race_run_forward(struct task *chao);
int race_jump(struct task *chao);
int race_swim(struct task *chao);
int race_wait_for_start(struct task *chao);
int race_trip(struct task *chao);
int race_fly(struct task *chao);
int race_run_to_items(struct task *chao);
int race_climb(struct task *chao);
/* whether the racer is more than RACE_SUBMERGED_DEPTH under the water at its
 * course progress. it sets the mover's flat_y to that water's height, or to
 * RACE_NO_WATER off the water. a racer that goes under starts race_swim */
#define RACE_SUBMERGED_DEPTH 1.5F
#define RACE_NO_WATER (-100000.0F)
Bool race_check_water(struct task *chao);

/* the up vectors the run's wall test and the climb's floor test compare a
 * normal with */
extern const NJS_VECTOR race_wall_up, race_ledge_up;
#endif

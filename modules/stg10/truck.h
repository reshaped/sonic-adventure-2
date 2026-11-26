/* the truck, TRUCK in Metal Harbor's object list. it rolls to and fro along a
 * rail on two wheels, and rebounds at each end. its lamp blinks */
#ifndef STG10_TRUCK_H
#define STG10_TRUCK_H
#include <ninja.h>

struct task;

/* the object list's step for TRUCK. unless placement_try_leave(task) returns
 * TRUE, it gives task a record in task->motion. it also gives task the truck's
 * own step, draw and destructor. where the heap can't give the record, this
 * stays task's step and tries again on the next. the init puts a copy of the
 * truck's collision object in the collision world. it gives task a child,
 * where the heap allows, whose box pushes players, enemies and objects.
 *
 * task needs a work and TASK_FORCE's records. pos places the rail's middle,
 * and ang.y turns the rail. each end lies scl.x from the middle along the
 * rail's z. the truck's top speed is scl.y + 0.1. it starts in the middle and
 * rolls toward +z. at each end it rebounds, settles for 31 steps, and rolls
 * back. its wheels turn 7 degrees a step for each unit of its speed. they stand
 * still while it settles.
 *
 * each step moves the copy and the child with the truck. it puts how far the
 * copy moved in both force records' speed. the world collects the copy only
 * while a player is near. a player is near within the model's radius plus 5
 * of its center. the lamp is gray for the first 21 of every 48 steps of
 * game_state.steps. it is white for the other 27 */
void harbor_truck_init(struct task *task);

/* the truck's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_truck_texlists[];

#endif

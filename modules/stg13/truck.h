/* the truck, TRUCK in City Escape's object list: a G.U.N. truck with a trailer.
 * it drives a path behind the players, and crashes at its end */
#ifndef STG13_TRUCK_H
#define STG13_TRUCK_H

#include <ninja.h>

struct task;

/* the texture lists of the truck, its lamps and its dust, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *truck_texlists[];

/* the object list's step for TRUCK. it does nothing where
 * placement_try_leave(task) returns TRUE. it also does nothing until
 * player_near finds a player within scl.y of pos. truck_init then gives task a
 * record in task->motion, and the truck's own step, draw and destructor. it
 * gives task a child where the heap allows. where the heap can't give the
 * record, truck_init stays task's step and tries again on the next step.
 *
 * task needs a work, and nothing in motion, force or extra. the truck drives
 * its own path from the path's second point, wherever pos lies. scl.x is its
 * cruising speed, in units a step.
 *
 * truck_init calls car_set_truck(task), so the stage's cars meet the truck.
 * task->force points at two matrices: the transform from the truck's box into
 * the world, then its inverse. task->extra holds the truck's speed along its
 * path, as a Float. the destructor frees the record and clears motion, force
 * and extra, so task_destroy frees none of them. it then calls
 * car_set_truck(NULL).
 *
 * the child is a model object with two boxes. they hurt what they touch while
 * the truck is on the ground, and only the second pushes. a touch of player 0
 * on the child throws that player ahead along the path. so does any touch in
 * the 9 steps after a throw. neither throws while the truck's speed is under
 * 0.1, but a crashed truck throws too. the truck slows while player 0 is near,
 * and speeds up while player 0 is far. past an end of the path it crashes, and
 * stays crashed. its child is then one box, which pushes and doesn't hurt */
void truck_init(struct task *task);

#endif

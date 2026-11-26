/* the triple spring, 3SPRING in a stage's object list: a slanted board on
 * three springs. it throws a player who touches it */
#ifndef SHARED_TRIPLE_SPRING_H
#define SHARED_TRIPLE_SPRING_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the spring's texture lists, ended by null. a stage's texture groups hold
 * the array */
extern NJS_TEXLIST *triple_spring_texlists[];

/* the object list's step for 3SPRING. task needs a work, whose pos and ang
 * place the spring. the angles turn it by njRotateZ, njRotateX and
 * njRotateY, in that order. task->motion must be null.
 *
 * unless placement_try_leave(task) returns TRUE, the call gives task the
 * spring's step, draw and destructor. it puts a record of its own in
 * task->motion. it also gives task a collision record, which the checks list
 * as a target. the heap must have room for the collision record, unchecked.
 * without room for the record in motion, the call stays task's step, and
 * tries again at the next.
 *
 * the step throws the first player it finds touching the spring. it throws
 * none where that player touched it fewer than 10 steps before. the throw
 * moves the player, then hands it to fun_8c049304. the player takes the
 * spring's angles, with ang.y turned half a turn. it takes a speed of 5 plus
 * scl.x along its own y. a touching player must be in slot 0 or 1, unchecked.
 * the record keeps a wait for those two alone */
void triple_spring_init(struct task *task);

#endif

/* the sandglass door, DOOR in Pyramid Cave's object list. it opens while a
 * sandglass of its id runs, and shuts in notches as the sand runs out */
#ifndef STG28_SANDGLASS_DOOR_H
#define STG28_SANDGLASS_DOOR_H
#include <ninja.h>

struct task;

/* the doors' and the frames' texture lists for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *cave_sandglass_door_texlists[];

/* the object list's step for DOOR. task needs a work, a motion record and
 * TASK_FORCE's records. the current camera must be in use. the works of
 * player 0 and of that camera's player must exist.
 *
 * the work's pos places the shut door, and its ang.y turns it. ang.x's
 * absolute value, modulo 5, is the kind: WALL_HOLE, GROUND, GROUND_HOLE,
 * WALL_DOOR or GROUND_HOLE_R. the WALL_ kinds stand upright and open upward.
 * the others lie flat, and slide open along the z axis that ang.y turns. the
 * _HOLE kinds draw a frame too. scl.z's absolute value, truncated and modulo
 * 4, is the color: BLUE, GREEN, RED or NORMAL. the low byte of ang.z is the
 * id of the sandglass the door follows. with bit 8 of ang.z set, the door is
 * 4 m. it then draws at 0.8 of its models' size. otherwise it is 5 m. the
 * door keeps values of its own in scl.x and scl.y.
 *
 * the call does nothing where placement_try_leave(task) is TRUE. a WALL_
 * door's collision record rides a child task. where task_create_child can't
 * give it, the call does nothing, and runs again at task's next step. the door
 * puts a copy of its kind's collision object in the collision world. it keeps
 * the copy in task->extra. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none.
 *
 * while cave_sandglasses holds no sandglass of its id, the door stays as it
 * stands. its sandglass's run must leave the closing at least one whole step
 * after the opening and the bounce. a shorter run makes the closing divide
 * by 0 */
void cave_sandglass_door_init(struct task *task);

#endif

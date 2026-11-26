/* the sandglass door, DOOR in Death Chamber's object list. it opens while a
 * sandglass of its id runs, and shuts in notches as the sand runs out */
#ifndef STG25_SANDGLASS_DOOR_H
#define STG25_SANDGLASS_DOOR_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the doors' and the frames' texture lists for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *chamber_sandglass_door_texlists[];

/* compile the doors' and frames' 20 models at buf, and return buf past the
 * builds. a model compiles once, at the first call that finds its texture
 * list loaded. the doors draw the builds only once one call has compiled all
 * 20. while objects_fog_off isn't 0, the call turns the fog off for the
 * builds and on after them */
Uint8 *chamber_sandglass_door_build(Uint8 *buf);

/* the object list's step for DOOR. task needs a work, a motion record and
 * TASK_FORCE's records. the works of player 0 and of the current camera's
 * player must exist. the work's fields mean:
 * - pos places the shut door, and ang.y turns it.
 * - ang.x's absolute value, modulo 4, is the kind: 0 WALL_HOLE, 1 GROUND,
 *   2 GROUND_HOLE or 3 WALL_DOOR. a WALL_ door stands upright and opens
 *   upward. a GROUND door lies flat and slides open along its z. a _HOLE door
 *   draws a frame too.
 * - scl.z's absolute value, truncated and modulo 4, is the color: 0 blue,
 *   1 green, 2 red or 3 normal.
 * - ang.z's low byte is the id of the sandglass the door follows. the door
 *   is 5 m across, a square 50 on a side. with bit 8 of ang.z set, it is
 *   4 m, 40 on a side. it then draws at 0.8 of its models' size.
 * the door keeps values of its own in scl.x and scl.y.
 *
 * the call returns at once where placement_try_leave(task) returns TRUE, as
 * each later step does. where task_create_child fails, it tries again at
 * task's next step.
 *
 * the door follows the sandglass that chamber_sandglass_door_find_glass finds
 * for its id. it opens over the run's first steps, bounces, and then shuts in
 * notches over the rest of the run. where the sandglass stops running, the
 * door slams shut at once. without a sandglass of its id, the door stays as it
 * stands. the run must leave the closing a whole step after the opening and
 * the bounce. otherwise a notch is 0 steps long, and the closing step divides
 * by 0. the door's travel then comes from an infinity or a NaN.
 *
 * the call adds a copy of its kind's collision object to the collision world,
 * and keeps it in task->extra. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies near the door's pos: within 10 plus 71 times the door's scale.
 * a child task carries the door's collision record. its one shape is a
 * cylinder along the edge that leads as the door shuts. the cylinder pushes
 * the shapes of every group, and resists their push */
void chamber_sandglass_door_init(struct task *task);

/* the sandglass that the doors of id follow, or null where no registered
 * sandglass has id. it is the running one of id with the fewest steps run, the
 * first of a tie. where none of id runs, it is the first of id in the table */
struct task *chamber_sandglass_door_find_glass(int id);

#endif

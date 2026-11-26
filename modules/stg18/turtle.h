/* the turtle, KAME in Dry Lagoon's object list. the swimmer swims a path, and
 * a swimming Rouge can ride it. the ferry carries player 0 between two
 * stops */
#ifndef STG18_TURTLE_H
#define STG18_TURTLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the turtle's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *turtle_texture_group[];

/* the object list's step for KAME. task needs a work, a placement and
 * TASK_FORCE's records. bit 0 of ang.z makes the turtle the ferry, and the
 * call clears the other bits. slot 0 must hold a player, unchecked.
 *
 * the swimmer swims its path at scl.y above it, and scl.x scales its speed.
 * the ferry stands at places of its own, and reads nothing of its work but
 * that bit. the placement's state keeps the turtle's mode. the ferry starts
 * at the stop that mode is at or bound for, or else at the start. only a
 * ferry that waits at the start leaves by placement_try_leave(task).
 *
 * the call takes the turtle's record and three matrices from the Shinobi
 * heap. where the heap can't give them, the call returns, and task's next step
 * calls it again. otherwise task takes the turtle's own step, draw and
 * destructor. the turtle stands in the collision world as an object
 * world_pool_alloc lends it. where the pool has none to lend, the call writes
 * through null. the call puts a sphere among the camera's marked zones. where
 * their table is full, the step writes through null. task's first force record
 * moves player 0 with the turtle while player 0 stands on it.
 *
 * the swimmer takes a collision record of a sphere, which a swimming Rouge
 * touches to ride it. the ferry takes a child task with a collision record of
 * a sphere, which the turtle carries. the heap must have room for the child
 * and its record, unchecked. the ferry waits at the start while enemy group
 * 0xff holds an enemy. it then comes to the south stop. at a stop, player 0
 * hops onto the turtle's back once it stands on the turtle. a ride to the
 * other stop starts once player 0 takes PLAYER_MODE_ROUGE_RIDE_FERRY. it fades
 * the screen out and in, and moves player 0 and camera 0 to the other stop.
 * game_state's pause_disabled is 1 and its idle_dim 0 while it lasts */
void turtle_init(struct task *task);

#endif

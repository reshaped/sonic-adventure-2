/* the golem fights' reel, UDREEL in their object lists: a handle on a wire. a
 * player who grabs the handle rides it up to the reel or down from it */
#ifndef SHARED_GOLEM_REEL_H
#define SHARED_GOLEM_REEL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the reel's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *golem_reel_texlists[];

/* build at buf each model not built yet, if its texture list is loaded. the
 * models are the reel's top, wire and handle. it returns buf past the builds.
 * a reel set up after a call that built all three draws its top from its
 * build. its handle draws from its build while held, and in the first 23 of
 * every 46 of game_state.steps. where objects_fog_off isn't 0, the call builds
 * with the fog off, then turns the fog on */
Uint8 *golem_reel_build(Uint8 *buf);

/* the length of wire in a unit of a reel's scl.x and scl.y */
#define GOLEM_REEL_LENGTH_UNIT 20.0F
/* a bit of a reel's ang.x. with it set, the handle starts at the bottom, and
 * goes back down by itself once let go at the top. without it, the handle
 * starts at the top */
#define GOLEM_REEL_RETURNS 0x1

/* the object list's step for UDREEL. it puts the reel's record, from the
 * Shinobi heap, in task->motion. where the heap has no room for it, the call
 * returns and runs again on task's next step. with the record, task takes the
 * reel's own step, draw and destructor. it also takes a child task, which
 * holds the handle's collision record. the reel's record holds a copy of the
 * reel's collision box, which goes into the collision world. task_create_child
 * must have a task to give, and the heap room for the collision record,
 * unchecked. the non-matching side instead frees what it made where either
 * fails, and tries again on task's next step.
 *
 * task needs a work, whose pos places the reel and whose ang.y turns it. with
 * the handle at the top, the wire paid out is scl.x GOLEM_REEL_LENGTH_UNITs.
 * at the bottom it is scl.y + 1 of them. golem_reel_init swaps scl.x and scl.y
 * where scl.y is the smaller, so that the top lies above the bottom. ang.x may
 * hold GOLEM_REEL_RETURNS.
 *
 * a player who touches the handle grabs it, once the handle has gone more than
 * 60 steps unheld. the reel then carries the player up or down, hanging below
 * the handle. it throws the player after the player's work leaves
 * PLAYER_MODE_HANG. the throw's speed is 1.1 plus scl.z. it leans from straight
 * up by 0x3400 plus ang.z, about the reel's z axis. while game_state.cleared is
 * 1, the reel lets its player go without a throw. a held player's slot must
 * stay filled, unchecked. the reel keeps its state in the work's mode, submode,
 * timer and tick.
 *
 * the handle is a COLLISION_TARGET only at the bottom. a grab there turns
 * player 0's pad off, and a held ride down turns it on */
void golem_reel_init(struct task *task);

#endif

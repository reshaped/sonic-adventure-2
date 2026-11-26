/* LASTSTEP, the moving platform that holds its rider, in Cannon's Core, Lost
 * Colony and Eternal Engine */
#ifndef SHARED_LAST_STEP_H
#define SHARED_LAST_STEP_H
#include <ninja.h>

struct task;

/* the platform's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *last_step_texlists[];
/* the set editor's names of the paths, in enum core_path_kind's order */
extern const char *last_step_path_names[6];
/* the volume of the sound Cannon's Core's platform plays as its panels open
 * fully */
extern Sint32 last_step_panel_volume;

extern NJS_TEXLIST last_step_texlist;

/* the object list's step for LASTSTEP. it does nothing where
 * placement_try_leave(task) returns TRUE. task needs a work, and the platform
 * keeps a record of its own in task->motion. where the heap can't give one,
 * the call does nothing and the next step tries again. the platform's node
 * goes into the world as a moving solid. where task has a force, its two
 * records carry the riders of players 0 and 1.
 *
 * the work's pos places the platform, and ang.y turns it. ang.x's low nibble
 * chooses the path, as shared/core_platform_path.h's PATH_KIND reads it.
 * scl.x is the path's length, or an arc's radius. scl.y + 0.1 is the speed,
 * in units a step. a platform that goes and comes back waits scl.z steps at
 * each end. ang.z turns a horizontal path, and an arc's pivot about pos. in
 * Lost Colony, the platform makes a light volume as its child.
 *
 * a rising or sinking platform sets off as player 0 boards it after more than
 * 30 steps without a rider. let b be ang.x's second byte. at its far end, the
 * platform also comes back for a player within (b * 10 + 10) * 2 of its start.
 * it holds player 0 from when it moves, and lets the player go after 120 steps
 * aboard. where b is odd, Eternal Engine's lets go only once it has also
 * stopped. where b is even, Eternal Engine's holds player 0 on any path while
 * the reactor lid stands. it holds the player up to 50 steps after the lid
 * breaks, and lets go only once those steps have passed */
void last_step_init(struct task *task);

#endif

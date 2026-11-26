/* the stars about a player's camera */
#ifndef BOSS_LAST2_STARS_H
#define BOSS_LAST2_STARS_H
#include <ninja.h>
struct task;
/* the stars' texture list. finalhazard_sky_groups lists it, so
 * last2_stage_init loads it with the backdrop's */
extern NJS_TEXLIST finalhazard_star_texlist;

/* create count stars about player's camera, in a task of level 1. they draw
 * while camera_current_index is player. where owner isn't null, *owner gets
 * the task. the task's destructor clears *owner while it still holds the
 * task. returns the task. it returns null where the heap can't give the
 * task, the stars' record or their points. *owner then stays as it was.
 * camera_views[player] must not be null while the task lives */
struct task *finalhazard_starfield_create(int player, int count,
                                          struct task **owner);
#endif

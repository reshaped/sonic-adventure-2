/* the stars, the task HosiExec: a cloud of glowing points about a player.
 * the points hang still in the world while the cloud follows the player */
#ifndef STG03_STARS_H
#define STG03_STARS_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST stars_texlist;

/* create a HosiExec task at task level 1, the stars of the player in slot
 * player. the cloud keeps count / 10 points in each of its 10 groups. it drops
 * the rest of count. it returns the task. it returns null where the slot has no
 * work. it returns null too where the heap can't give the task or its records.
 * where owner isn't null, *owner takes the task. the task sets *owner back to
 * null as it ends, while *owner still holds it.
 *
 * the cloud is a sphere of radius 350. its center lies halfway between the
 * player and camera_views[player]. where that camera is null, it lies on the
 * player. a point that leaves the sphere comes back on its far side. the points
 * rise slowly, and each group wavers on a wave of its own. while
 * game_state.time_stopped isn't 0, they neither rise nor waver.
 *
 * the cloud draws only while camera_current_index is player and the work's mode
 * is 0. while the mode isn't 0, the points ride with the center, unmoved about
 * it. they do so too on a step that moves the center farther than 100. the task
 * destroys itself once the player's slot has no work */
struct task *stars_create(int player, int count, struct task **owner);

#endif

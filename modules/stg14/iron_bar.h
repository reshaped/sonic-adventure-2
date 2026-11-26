/* the iron bar, IRONBAR in Radical Highway's object list: a bar a player
 * swings round. the arc is a band the bar draws about the player's grip. a
 * press of A while the swing faces it throws the player toward the work's
 * scl */
#ifndef STG14_IRON_BAR_H
#define STG14_IRON_BAR_H
#include <ninja.h>

struct task;

/* the bar's texture lists for texture_load_shared: none, only the null that
 * ends the list */
extern NJS_TEXLIST *highway_iron_bar_texlists[];

/* the object list's step for IRONBAR. it does nothing while
 * game_state.cleared is 1, nor where the set file's bar finds
 * placement_try_leave(task) TRUE. otherwise it gives task a record in
 * task->motion and a collision record of kind COLLISION_OBJECT. it also gives
 * task the bar's own step, draw and destructor. where the heap can't give the
 * record, this stays task's step and tries again on the next.
 *
 * task needs a work. pos is the bar's middle, and ang.y turns it. the bar
 * reaches the low byte of ang.x, plus 1, each way along its x. the low byte of
 * ang.z adds to its radius of 3. a throw ends at the player's grip moved by
 * scl - pos. the init turns ang.y half a turn where it faces more than a
 * quarter turn from scl.
 *
 * the work's timer is the slot of the player the bar serves, 0 for the set
 * file's bar. as that bar starts, it makes a child copy of itself where slot
 * 1 holds a player. the copy serves slot 1. only the bar's own player can take
 * hold of it. the bar kills itself once game_state.cleared is 1. the swing and
 * the throw hold while game_state.time_stopped isn't 0. a time stop already on
 * as the player took hold doesn't hold them */
void highway_iron_bar_init(struct task *task);

#endif

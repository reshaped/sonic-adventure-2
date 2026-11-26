/* the iron bar, IRONBAR in City Escape's object list: a bar a player swings
 * round. the arc is a band the bar draws about the player's grip. a press of
 * A while the swing faces it throws the player toward the work's scl */
#ifndef STG13_IRON_BAR_H
#define STG13_IRON_BAR_H
#include <ninja.h>

struct task;

/* the bar's texture lists for texture_load_shared: none, only the null that
 * ends the list */
extern NJS_TEXLIST *iron_bar_texlists[];

/* the object list's step for IRONBAR. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion and a collision record
 * of kind COLLISION_OBJECT. it also gives task the bar's own step, draw and
 * destructor. where the heap can't give the record, this stays task's step
 * and tries again on the next. the bar's own step tests placement_try_leave
 * too, while no player holds the bar.
 *
 * task needs a work. pos is the bar's middle, and ang.y turns it. the bar
 * reaches the low byte of ang.x, plus 1, each way along its x. the low byte of
 * ang.z adds to its radius of 3. a throw ends at the player's grip moved by
 * scl - pos. the init turns ang.y half a turn where it faces more than a
 * quarter turn from scl.
 *
 * each step fills index 4 of lights with the bar's light. the bar selects it
 * for its player while the swing faces the arc. a throw puts a camera mode of
 * its own on its player's camera. the mode comes off as the throw or the task
 * ends */
void iron_bar_init(struct task *task);

#endif

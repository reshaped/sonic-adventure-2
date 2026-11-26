/* the iron bar, IRONBAR in Pyramid Cave's object list: a chained bar that a
 * player swings round. the arc is a band the bar draws about the player's
 * grip. a press of A while the swing faces it throws the player toward the
 * work's scl */
#ifndef STG28_IRON_BAR_H
#define STG28_IRON_BAR_H

#include <ninja.h>

struct task;

/* the bar's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_iron_bar_texlists[];

/* the object list's step for IRONBAR. task needs a work. its pos places the
 * bar, and its ang.y turns it. ang.x is the chain's length, in hundredths of
 * its model's. the low four bits of ang.z add to the bar's radius of 3. the
 * call clears the rest. a throw ends at the player's grip moved by
 * scl - pos. the call turns ang.y half a turn where it faces more than a
 * quarter turn from scl.
 *
 * the call gives task a record at task->motion, and a child that holds the
 * frame's two posts. where either can't be allocated, the call returns, and
 * runs again on the next step. where the record fails, the child stays, and
 * the next run makes another. the heap must have room for the bar's collision
 * record, or the call reads through null */
void cave_iron_bar_init(struct task *task);

#endif

/* the Finalhazard's weak point, which shows at one of sixteen slots on its
 * body */
#ifndef BOSS_LAST2_PANELS_H
#define BOSS_LAST2_PANELS_H
#include <ninja.h>
#include "system/angle.h"
struct task;

/* the weak point's slots, a row on each player's side of the body, row 0 on
 * player 0's. a slot lies at pos, turned by angles, in the matrix of
 * finalhazard_panel_bases[base] */
#define PANEL_ROWS 2
#define PANEL_COLUMNS 8
struct panel_slot {
    Sint8 base;
    struct angles angles;
    NJS_POINT3 pos;
};
extern struct panel_slot finalhazard_panel_slots[PANEL_ROWS][PANEL_COLUMNS];

/* the matrices finalhazard_bind_joints keeps as the body draws. a base is the
 * matrix of one of the first FINALHAZARD_PANEL_BASES joints' nodes.
 * finalhazard_joint_inverse undoes the view's matrix. a point turned by a base
 * and then by finalhazard_joint_inverse lies in the world */
#define FINALHAZARD_PANEL_BASES 5
extern NJS_MATRIX finalhazard_panel_bases[FINALHAZARD_PANEL_BASES];
extern NJS_MATRIX finalhazard_joint_inverse;

/* a pointer may be null only where a comment says so. every task argument
 * but a parent is the weak point's */

/* a weak point keeps a next slot, where it grows once it has shrunk away.
 * with none, it stays hidden then. a new weak point's next slot is row 0's
 * first */

/* create the weak point, hidden, as a child of parent, in row. it shows once
 * finalhazard_panel_move moves it. returns the task, or null without parent
 * or where the heap can't give the task or its extra. the heap must give the
 * collision record, unchecked */
struct task *finalhazard_panel_create(struct task *parent, Sint8 row);
/* move the weak point to a slot of row picked at random, other than its own.
 * one that shows shrinks away, with that slot as its next. one that is
 * shrinking takes the slot at once and shrinks on there. its next slot stays
 * as it was. any other grows at the slot now. nothing hurts it until it has
 * shrunk away or grown */
void finalhazard_panel_move(struct task *task, Sint8 row);
/* rearm the weak point, and have it shrink away with no next slot */
void finalhazard_panel_hide(struct task *task);
/* clear the weak point's hit, and let it be hit again. without task it does
 * nothing */
void finalhazard_panel_rearm(struct task *task);
/* 1 from a hit on the weak point until it is rearmed, else 0. -1 without
 * task */
int finalhazard_panel_hit(struct task *task);
/* the cannon a lone laser fires from at the weak point's slot. it is 0 for
 * the attack's first point, or 1 for its second. -1 without task */
int finalhazard_panel_cannon(struct task *task);
/* create, as children of parent, a weak point growing at each slot, for the
 * test menu. the heap must give each, unchecked */
void finalhazard_show_weak_points(struct task *parent);
/* finalhazard_show_weak_points, under a second name declared with the player
 * arena.c passes, which it doesn't read */
void finalhazard_preview_for_player(struct task *parent, int player);
#endif

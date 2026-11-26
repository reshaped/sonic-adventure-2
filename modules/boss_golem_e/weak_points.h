/* the weak points of BOSS_GOLEM_E.PRS's Egg Golem, and the golem's rubble.
 * golem_e_rubble_throw reads golem_e, and so do the weak points, their targets
 * and the rubble. golem_e must not be null while any of them runs */
#ifndef BOSS_GOLEM_E_WEAK_POINTS_H
#define BOSS_GOLEM_E_WEAK_POINTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

#define GOLEM_E_WEAK_POINTS 3
/* the targets an open weak point shows */
#define GOLEM_E_WEAK_POINT_TARGETS 3

/* a weak point's record, in its task's extra. the weak point rides its
 * sensor's node at pos, turned by ang */
struct golem_e_weak_point {
    Sint32 mode; /* closed, open or broken */
    Sint32 next_mode; /* the mode the next step takes, or -1 for none */
    Sint32 mode_steps; /* the steps the mode has run */
    Sint32 sensor; /* the golem's sensor whose node the weak point rides */
    Sint32 index; /* the weak point's index in golem_e_weak_points */
    NJS_POINT3 pos;
    struct angles ang;
    /* the steps left of a hit's flash. the golem's step counts them down */
    Sint32 flash_steps;
    Sint32 threshold; /* the count of the golem's hits that opens it */
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    /* TRUE for each target of an open weak point that a hit has struck */
    Bool struck[GOLEM_E_WEAK_POINT_TARGETS];
    /* TRUE from a closed weak point's step until its next. the golem counts a
     * hit on the weak point only then */
    Bool active;
};
#define GOLEM_E_WEAK_POINT(task) ((struct golem_e_weak_point *)(task)->extra.p)

/* each weak point's task, or null once it is destroyed */
extern struct task *golem_e_weak_points[GOLEM_E_WEAK_POINTS];

/* compile six models at buf, and return buf past them. they are the three weak
 * points', their mechanism's, and the two that rubble draws. a model compiles
 * once, at the first call that finds its texture list loaded. once one call has
 * compiled all six, golem_e_parts_built is TRUE. while objects_fog_off isn't 0,
 * the call turns the fog off for the builds and on after them */
Uint8 *golem_e_parts_build(Uint8 *buf);

/* create the weak points, closed, each a task of its own at task level 2.
 * golem_task isn't read. the weak points open in turn, as the golem's hits
 * reach 20, 40 and 60. an open weak point shows its targets. it breaks once a
 * hit has struck each, and the break takes 1.501 of the golem's power. where
 * an allocation fails, the call or a later step reads or writes through
 * null */
#ifdef NONMATCHING
/* this side returns FALSE where an allocation fails, and TRUE otherwise. the
 * weak points made by then stay in golem_e_weak_points */
Bool golem_e_weak_points_create(struct task *golem_task);
#else
void golem_e_weak_points_create(struct task *golem_task);
#endif

/* each call below throws pieces of rubble in a task of their own. a piece
 * flies until it drops below y 0 or its life ends. the task kills itself once
 * every piece has ended. golem_e's rubble_count gains each piece
 * golem_e_rubble_throw makes. it loses every piece that ends, also those
 * golem_e_rubble_hit and an opening weak point throw. where the heap can't give
 * the task or its pieces, nothing is thrown */

/* throw seven pieces of rubble from *pos, as task's child */
void golem_e_rubble_hit(struct task *task, NJS_POINT3 *pos);
/* throw rubble from the place of the golem's sensor, as golem_task's child. a
 * throw needs the sensor to have moved more than 2 since its last place. a
 * chance of 0.0333 must pass too. no call throws while golem_e's rubble_count
 * is above 32 */
void golem_e_rubble_throw(struct task *golem_task, int sensor);

#endif

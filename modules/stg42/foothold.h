/* DODAI and DODAIGEN in the object list of the second fight of Sonic and
 * Shadow. they are the foothold and its generator. a foothold drops a while
 * after player 0 stands on it. the generator lays footholds along x ahead of
 * the players */
#ifndef STG42_FOOTHOLD_H
#define STG42_FOOTHOLD_H
#include <ninja.h>

struct task;

/* the generator lays a foothold every FOOTHOLD_SPACING along x, from x 0 */
#define FOOTHOLD_SPACING 300.0F

/* the foothold's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *foothold_texture_group[];

/* the object list's step for DODAI. task needs a work and TASK_FORCE's
 * records, and the work's pos and ang place the foothold. the foothold stands
 * in the world as a copy of its collision model. where the heap can't give
 * the copy, the step tries again at the next.
 *
 * player 0's first stand on the foothold sets the work's timer to 720. the
 * timer then counts the steps while game_state.time_stopped is 0, until the
 * stage's clear. the foothold drops once the timer, read as a Sint16, passes
 * 900. below y -100 it falls faster, to a limit, and ends once
 * placement_try_leave finds it out of range. a placement's foothold can end
 * so at any step */
void foothold_step(struct task *task);

/* the object list's step for DODAIGEN. task needs a work. the generator lays
 * its footholds up to 2100 past each player. it
 * starts each one's timer at 0.4 times how far behind its player it lies. a
 * foothold whose timer starts past 900 drops at its second step. so does each
 * one laid after 36000 of the generator's steps while time runs. at the
 * stage's clear the generator lays one last foothold at x -300, and no
 * more */
void foothold_generator_step(struct task *task);

#endif

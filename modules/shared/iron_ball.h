/* the iron balls, IRONBALL2 in a stage's object list. a bar turns about its
 * hub, with an iron ball at each end. the balls roll as the bar turns */
#ifndef SHARED_IRON_BALL_H
#define SHARED_IRON_BALL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* bits of the work's ang.z, as the placement gives them. IRON_BALL_NO_DUST
 * keeps the balls from raising dust. IRON_BALL_FLOAT has the bar rise and
 * fall in place of resting on the floor */
#define IRON_BALL_NO_DUST 0x1
#define IRON_BALL_FLOAT 0x2

/* the ball's texture list, which a stage's table of texture lists names */
extern NJS_TEXLIST iron_ball_texlist;
/* the iron balls' and their dust's texture lists for texture_load_shared,
 * ended by null */
extern NJS_TEXLIST *iron_ball_texlists[];

/* build at buf the ball's, the hub's and the bar's models, each not built yet.
 * a model waits until its texture list is loaded. return buf past what it
 * built. iron balls set up after a single call has built all three draw the
 * builds. those set up before draw the models */
Uint8 *iron_ball_build(Uint8 *buf);
/* the object list's step for IRONBALL2. unless placement_try_leave(task)
 * returns TRUE, it gives task the iron balls' step, draw and destructor.
 * without room for the balls' record, task keeps this step and tries again at
 * its next step. the heap must have room for a collision
 * record too, unchecked. task needs a work, and no record in motion, where the
 * balls keep theirs.
 *
 * the hub stands 10 above pos. each ball lies 20 * (scl.x + 1) from the hub.
 * the bar turns scl.y + 1 degrees a step. a floating bar rises and falls up to
 * scl.z from pos.y as placed. the call clears ang.x, ang.y and every bit of
 * ang.z but IRON_BALL_NO_DUST and IRON_BALL_FLOAT.
 *
 * while game_state.time_stopped isn't 0, the bar holds its turn and the balls
 * raise no dust. a floating bar still rises and falls.
 *
 * a bar that doesn't float moves pos to the highest floor at or below its hub.
 * a bar that finds no floor looks again every 40 steps. the balls raise no dust
 * until the bar finds a floor */
void iron_ball_init(struct task *task);

#endif

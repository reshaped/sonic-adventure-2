/* the garden's ball, and what a Chao reads of it to chase it */
#ifndef CHAO_GARDEN_BALL_H
#define CHAO_GARDEN_BALL_H
#include <sg_xpt.h>

struct task;

/* the ball made last, from its first step on. it still points at that ball
 * once the ball is destroyed. the GCC side clears it then */
extern struct task *garden_ball;
/* the ball rolls free faster than 0.4 a step */
extern Bool garden_ball_rolling;
/* the ball floats on a flat surface, as water's, this step */
extern Bool garden_ball_floating;
/* garden_bounce_on_floor for the ball */
void garden_ball_land(struct task *ball);

#endif

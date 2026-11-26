/* a garden Chao in water, and its climb out */
#ifndef CHAO_SWIM_H
#define CHAO_SWIM_H
#include <sg_xpt.h>

struct task;

int chao_climb_out_behavior(struct task *chao);
/* return TRUE where chao isn't held and lies more than 2 below its garden's
 * water surface. return FALSE otherwise. a Chao newly in water splashes and
 * starts to swim */
Bool chao_try_swimming(struct task *chao);
#endif

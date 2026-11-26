/* the water ripple: a ring on the water that grows, fades and ends */
#ifndef SHARED_WATER_RIPPLE_H
#define SHARED_WATER_RIPPLE_H
#include <ninja.h>
#include <sg_xpt.h>

/* make a ripple of size scale at pos, a task named ObjectWaterripple in task
 * level 3. it makes nothing where the heap has no room for the task. a null
 * growth stands for an x of 0.025 and a z of 1, and growth->y isn't read.
 *
 * the size is the ripple's scale in x and z. from the task's second step, each
 * step adds growth->x to it. once the size passes growth->z, the ripple fades
 * out over 25 steps, still growing, and ends. it draws dimmer on
 * STAGE_DEATH_CHAMBER, and dimmer still on STAGE_EGG_QUARTERS */
void ripple_create(NJS_POINT3 *pos, NJS_VECTOR *growth, Float scale);

/* the ripple's texture list and model. the water's bubbles draw their own
 * ripples with them */
extern NJS_TEXLIST ripple_texlist;
extern NJS_CNK_OBJECT ripple_object;

#endif

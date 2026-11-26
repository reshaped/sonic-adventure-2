/* the pickup object, PIC in a stage's object list: an object a player picks
 * up and holds */
#include <ninja.h>

#include "shared/common_object.h"
#include "shared/pickup_object.h"

NJS_TEXLIST *pickup_object_texlists[] = {&pickup_object_texlist, NULL};
/* how far a holder's draw turns the nodes it holds the object with.
 * holdable_init keeps it in the motion record's hold_turn. pickup_object_2.c
 * compiles this source again with a hold angle of 0x200 */
#ifndef PICKUP_OBJECT_HOLD_ANGLE
#define PICKUP_OBJECT_HOLD_ANGLE 0x300
#endif
static Angle hold_angle = PICKUP_OBJECT_HOLD_ANGLE;

void pickup_object_init(struct task *task)
{
    holdable_init(task, &pickup_object_texlist, &pickup_object_model,
                  hold_angle, OBJECT_MAY_LEAVE);
}

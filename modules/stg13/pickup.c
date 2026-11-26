/* the pickup, PIC in City Escape's object list: an object a player picks up
 * and holds */
#include <ninja.h>

#include "shared/common_object.h"
#include "stg13/pickup.h"

NJS_TEXLIST *pickup_texlists[] = {&pickup_texlist, NULL};
/* how far a holder's draw turns the nodes it holds the pickup with.
 * holdable_init keeps it in the motion record's hold_turn */
static Angle hold_angle = 0x300;

void pickup_init(struct task *task)
{
    holdable_init(task, &pickup_texlist, &pickup_model, hold_angle,
                  HOLDABLE_BOUNCY | HOLDABLE_ROLLS | OBJECT_MAY_LEAVE);
}

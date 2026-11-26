/* CASHBOX in Security Hall's object list: the safes */
#ifndef STG08_CASH_BOX_H
#define STG08_CASH_BOX_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST *security_safe_textures[];
/* the object list's init for CASHBOX. the placement's ang.y turns the safe,
 * and ang.x picks its areas, as security_hall.h's SAFE_AREA and SAFE_TEXTURE
 * read them. a player's touch opens the safe. once no player is near, the
 * open safe kills its placement, for 20 points.
 *
 * a placement out of range keeps its safe's front in one of 120 slots, which
 * security_cash_boxes_draw draws. the placement's state holds the slot + 1,
 * or 0 for none. the init frees the slot once the placement is in range */
void security_cash_box_init(struct task *task);

/* draw the fronts that CASHBOX placements out of range keep */
void security_cash_boxes_draw(void);

#endif

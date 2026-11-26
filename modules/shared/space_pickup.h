/* the space pickup, PIC in Meteor Herd's and Mad Space's object lists. a
 * player picks it up and holds it */
#ifndef SHARED_SPACE_PICKUP_H
#define SHARED_SPACE_PICKUP_H
#include <ninja.h>

struct task;

extern NJS_CNK_MODEL space_pickup_model;
extern NJS_TEXLIST space_pickup_first_texlist;
extern NJS_TEXLIST space_pickup_second_texlist;
extern NJS_TEXLIST space_pickup_third_texlist;
/* the pickup's three texture lists, ended by null */
extern NJS_TEXLIST *space_pickup_textures[];

/* the object list's step for PIC. it makes task a holdable object of
 * shared/common_object.h, as holdable_init does, with OBJECT_MAY_LEAVE its
 * only mode bit. unlike holdable_init's, the pickup releases no animal. it
 * draws space_pickup_model with the list of space_pickup_textures that scl.x
 * picks: its whole part, taken unsigned, modulo 3 */
void space_pickup_init(struct task *task);

#endif

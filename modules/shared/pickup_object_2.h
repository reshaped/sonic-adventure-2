/* the pickup object of shared/pickup_object.h compiled a second time, PIC
 * in a stage's object list. only the hold angle differs: 0x200 here, and 0x300
 * in pickup_object.c. holdable_init keeps it in the motion record's hold_turn.
 *
 * STG08.PRS, STG22.PRS, STG30.PRS and STG40.PRS hold this object. STG10.PRS
 * and STG14.PRS hold pickup_object.c's */
#ifndef SHARED_PICKUP_OBJECT_2_H
#define SHARED_PICKUP_OBJECT_2_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST pickup_object_2_texlist;
extern NJS_CNK_MODEL pickup_object_2_model;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pickup_object_2_texlists[];

/* the object list's step for PIC: pickup_object_init's, with this object's
 * model, texture list and hold angle */
void pickup_object_2_init(struct task *task);

#endif

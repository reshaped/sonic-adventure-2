/* PIC in Final Chase's object list, and SIGNSHIP in Final Rush's. its model
 * has three parts: one plain, one whose tint pulses, and one whose texture
 * scrolls */
#ifndef SHARED_PIC_H
#define SHARED_PIC_H
#include <ninja.h>

#include "draw/texture_anim.h"

struct task;

/* @hack type: the three objects declared as arrays, where single objects
 * would do */
extern NJS_CNK_OBJECT pic_scroll_object[];
extern NJS_CNK_OBJECT pic_pulse_object[];
extern NJS_CNK_OBJECT pic_object[];
/* pic_anim scrolls the model of pic_scroll_object */
extern struct texture_anim pic_anim;

extern NJS_TEXLIST pic_texlist;

/* the object's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pic_texlists[];

/* the object list's step for PIC and SIGNSHIP. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * object's own step, draw and destructor, and one capsule of kind
 * COLLISION_OBJECT. task needs a work, whose pos places the model. its ang
 * turns the model about y first, then x, then z. game_state.steps drives the
 * pulse and the scroll */
void pic_init(struct task *task);

#endif

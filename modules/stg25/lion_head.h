/* the lion's head, LIONHEAD in Death Chamber's object list: a head a player
 * can pick up and carry. the first time a player holds it, it may release a
 * small animal */
#ifndef STG25_LION_HEAD_H
#define STG25_LION_HEAD_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the head's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *chamber_lion_head_texlists[];

/* compile the head's model at buf, and return buf past the build. the model
 * compiles once, at the first call that finds its texture list loaded. the
 * heads draw the build once a call has compiled it. while objects_fog_off isn't
 * 0, the call turns the fog off for the build and on after it */
Uint8 *chamber_lion_head_build(Uint8 *buf);

/* the object list's step for LIONHEAD. task needs a work and a motion record.
 * the work's pos places the head, and its ang.y turns it. the heap must have
 * room for the head's body: without one, each step reads and writes through
 * null.
 *
 * the low byte of ang.z holds a chance and an animal's kind. the head's first
 * hold releases a small animal of that kind by that chance. the chance is
 * 0.05 plus 0.1 times the byte's high four bits. high bits of 0 release none.
 * the low four bits, modulo 3, pick the kind. a hold sets the state of the
 * head's placement to 1. it releases an animal only where the state was 0 */
void chamber_lion_head_init(struct task *task);

#endif

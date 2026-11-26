/* the small pot, MINITSUBO in Dry Lagoon and Egg Quarters: a pot a player can
 * pick up and carry. the first time a player holds it, it may release a
 * small animal */
#ifndef SHARED_SMALL_POT_H
#define SHARED_SMALL_POT_H
#include <ninja.h>

struct task;

extern NJS_CNK_MODEL small_pot_model;
extern NJS_TEXLIST small_pot_texlist;

/* the pot's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *small_pot_texlists[];

/* the object list's step for MINITSUBO. task needs a work and a motion
 * record. the work's pos places the pot, and its ang.y turns it. the heap
 * must have room for the pot's body: without one, each step reads and writes
 * through null.
 *
 * the low byte of ang.z holds a chance and an animal's kind. the pot's first
 * hold releases a small animal of that kind by that chance. the chance is
 * 0.05 plus 0.1 times the byte's high four bits. high bits of 0 release none.
 * the low four bits, modulo 3, pick the kind. a hold sets the state of the
 * pot's placement to 1. it releases an animal only where the state was 0 */
void small_pot_init(struct task *task);

#endif

/* SIGNL, the carrier stages' gun under a hinged lid */
#ifndef SHARED_DECK_GUN_H
#define SHARED_DECK_GUN_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST *deck_gun_texlists[];

/* SIGNL's init. the gun turns toward player 0 while it lies within 300 units
 * of pos. otherwise it turns toward player 1 within that range. a hit on the
 * gun breaks it off, and a hit on the lid sets the lid swinging. the
 * placement's state keeps each hit */
void deck_gun_init(struct task *task);

#endif

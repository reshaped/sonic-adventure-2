/* SHUT and SHUTS, the carrier stages' shutters, which break once their
 * dynamite is gone */
#ifndef SHARED_DECK_SHUTTER_H
#define SHARED_DECK_SHUTTER_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *deck_shutter_texlists[];
extern NJS_TEXLIST *deck_shutter_small_texlists[];

/* the inits of SHUT and SHUTS. ang.x's low byte is the id of the shutter's
 * charges. a shutter breaks once dynamite_count finds none of them left, but
 * one of id 0 never breaks. the placement's state keeps a shutter broken. SHUT
 * scores 20 points as it breaks. SHUTS is the narrower, and scores none */
void deck_shutter_init(struct task *task);
void deck_shutter_small_init(struct task *task);

#endif

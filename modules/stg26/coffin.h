#ifndef STG26_COFFIN_H
#define STG26_COFFIN_H
#include <ninja.h>

#include "stg26/prop_builds.h"

struct task;

/* the coffin's texture lists and its pieces', each in CHAMBER_COLORS order
 * and null-ended */
extern NJS_TEXLIST *quarters_coffin_texlists[];
extern NJS_TEXLIST *quarters_coffin_pieces_texlists[];

void quarters_coffin_init(struct task *task);

#endif

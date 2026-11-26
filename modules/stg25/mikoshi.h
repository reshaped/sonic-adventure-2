/* the mikoshi, MIKOSHI in Death Chamber's object list: a portable shrine that
 * a Knuckles or a Rouge breaks */
#ifndef STG25_MIKOSHI_H
#define STG25_MIKOSHI_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the whole mikoshi and of its
 * pieces. a color's lies at its index, and each array ends in null */
extern NJS_TEXLIST *chamber_mikoshi_texlists[];
extern NJS_TEXLIST *chamber_mikoshi_pieces_texlists[];

/* the object list's step for MIKOSHI. task needs a work and a placement. the
 * work's pos places the mikoshi, and its ang.y turns it. CHAMBER_COLOR of
 * stg25/chamber_color.h picks its color. a mikoshi whose placement's state
 * isn't 0 is broken, and draws nothing.
 *
 * the call gives an unbroken mikoshi a collision record of one box. the heap
 * must have room for the record, unchecked. the box pushes players, enemies
 * and objects, and resists their push. the mikoshi breaks where
 * chamber_hunter_attacking holds for the first player its record touched. it
 * breaks into twelve pieces, which vanish at the floor under it. the break sets
 * the placement's state to 1, and kills the placement with placement_kill */
void chamber_mikoshi_init(struct task *task);

#endif

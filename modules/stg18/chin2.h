/* CHIN2 in Dry Lagoon's object list: the piece of stg18/chin.h over a model
 * of its own */
#ifndef STG18_CHIN2_H
#define STG18_CHIN2_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* this piece's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *chin2_texlists[];

/* do what chin_build does, for this piece's model */
Uint8 *chin2_build(Uint8 *buf);

/* do what chin_init does, for CHIN2's piece. the copy collides only while a
 * player lies within 45 * (scl.x + 1) + 10 of pos. it keeps NJD_EVAL_UNIT_SCL,
 * so it collides at its model's own size, whatever scl.x */
void chin2_init(struct task *task);

#endif

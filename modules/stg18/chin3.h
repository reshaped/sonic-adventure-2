/* CHIN3 in Dry Lagoon's object list: the piece of stg18/chin.h over a model
 * of its own */
#ifndef STG18_CHIN3_H
#define STG18_CHIN3_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* this piece's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *chin3_texlists[];

/* do what chin_build does, for this piece's model */
Uint8 *chin3_build(Uint8 *buf);

/* do what chin_init does, for CHIN3's piece. the copy collides only while a
 * player lies within 39 * (scl.x + 1) + 10 of pos */
void chin3_init(struct task *task);

#endif

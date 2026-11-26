/* SHELF in Dry Lagoon's object list: the piece of stg18/chin.h over a model
 * of its own */
#ifndef STG18_SHELF_H
#define STG18_SHELF_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* this piece's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *shelf_texlists[];

/* do what chin_build does, for this piece's model */
Uint8 *shelf_build(Uint8 *buf);

/* do what chin_init does, for SHELF's piece. the piece and its copy turn
 * about y alone, by ang.y. the copy collides only while a player lies within
 * 60 * (scl.x + 1) + 10 of pos */
void shelf_init(struct task *task);

#endif

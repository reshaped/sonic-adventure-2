/* the electric pillars of the route's object list, of two kinds, ELEPIL and
 * ELEPIL2. each is a solid cylinder that draws an arc, which shows a new frame
 * every step */
#ifndef STG34_ELECTRIC_PILLAR_H
#define STG34_ELECTRIC_PILLAR_H
#include <ninja.h>

struct task;
extern NJS_CNK_MODEL electric_pillar_model;
extern NJS_CNK_MODEL tall_pillar_model;
/* @hack type: declared as an array of one, where a single model would do */
extern NJS_CNK_MODEL electric_arc_model[1];
extern NJS_TEXLIST electric_pillar_texlist;
extern NJS_TEXLIST electric_arc_texlist;
extern NJS_TEXLIST tall_pillar_texlist;
extern NJS_TEXLIST *electric_pillar_texlists[], *tall_pillar_texlists[];
/* coordinate + 2 rounded down to a multiple of 16, where it isn't negative.
 * only the arc's draw calls it, with each u and v of the arc's model. each
 * lies within a unit of an edge of its cell. the call returns that edge */
int electric_pillar_cell_edge(int coordinate);
/* the steps the object list names for ELEPIL and ELEPIL2. the task needs a
 * work, whose pos places the pillar */
void electric_pillar_init(struct task *task);
void tall_pillar_init(struct task *task);

#endif

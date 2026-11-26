/* the cage, ORI in a stage's object list: a box that a missile of its id
 * breaks. a CAGE_BREAK_SHAPE_ID shape's touch breaks it too. it falls when a
 * box under it breaks */
#ifndef SHARED_CAGE_H
#define SHARED_CAGE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the id of the shape whose touch breaks a cage */
#define CAGE_BREAK_SHAPE_ID 17

/* compile the cage's model and its three pieces' at buf, and return buf past
 * them. each compiles once, and only while its texture list is loaded. the
 * cages draw and throw the compiled models only once one call has compiled all
 * four */
Uint8 *cage_build(Uint8 *buf);
/* the object list's step for ORI. task needs a work, and nothing in motion or
 * extra. the cage keeps a record of its own in motion, and its floor's height
 * in extra. the work's pos places the cage, and its ang.y turns it. the cage
 * ends where placement_try_leave returns TRUE. where the heap has no room for
 * that record, cage_init stays task's step to try again. the heap must have
 * room for the cage's collision record too.
 *
 * the low 5 bits of ang.z are the cage's missile target id, as missile.h says.
 * the low 4 bits of ang.x, modulo 3, pick what the cage's pieces do. 0 bounces
 * them on its floor, 1 lets them fall through it, and 2 throws none. cage_init
 * clears the other bits of both. a broken cage scores 20 through hud.h's
 * score_add.
 *
 * the cage joins the stack list of shared/common_object.h, so it falls when a
 * box under it breaks. a cage the list can't take never falls */
void cage_init(struct task *task);

/* the cage's texture list, model and object, and each of its three pieces' */
extern NJS_TEXLIST cage_texlist;
extern NJS_CNK_MODEL cage_model;
extern NJS_CNK_OBJECT cage_object;
extern NJS_TEXLIST cage_piece1_texlist;
extern NJS_CNK_MODEL cage_piece1_model;
extern NJS_CNK_OBJECT cage_piece1_object;
extern NJS_TEXLIST cage_piece2_texlist;
extern NJS_CNK_MODEL cage_piece2_model;
extern NJS_CNK_OBJECT cage_piece2_object;
extern NJS_TEXLIST cage_piece3_texlist;
extern NJS_CNK_MODEL cage_piece3_model;
extern NJS_CNK_OBJECT cage_piece3_object;

/* the cage's texture list and its three pieces', for texture_load_shared,
 * ended by null */
extern NJS_TEXLIST *cage_texlists[];
/* a box the cage's size, as a basic model, which no code reads */
extern NJS_OBJECT cage_bounds_object;

#endif

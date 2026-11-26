/* the pot, TSUBO in Sand Ocean's and Hidden Base's object lists, which an
 * attack breaks into five pieces */
#ifndef SHARED_OCEAN_POT_H
#define SHARED_OCEAN_POT_H
#include <ninja.h>

struct task;

/* Both the Sand Ocean and Hidden Base pots scatter five pieces. */
#define OCEAN_POT_PIECES 5

/* Sand Ocean's pot and its pieces, which ocean_pot_art.c defines */
extern NJS_CNK_MODEL ocean_pot_model;
extern NJS_CNK_MODEL ocean_pot_piece_model_0;
extern NJS_CNK_MODEL ocean_pot_piece_model_1;
extern NJS_CNK_MODEL ocean_pot_piece_model_2;
extern NJS_CNK_MODEL ocean_pot_piece_model_3;
extern NJS_CNK_MODEL ocean_pot_piece_model_4;
extern NJS_TEXLIST ocean_pot_piece_texlist_0;
extern NJS_TEXLIST ocean_pot_piece_texlist_1;
extern NJS_TEXLIST ocean_pot_piece_texlist_2;
extern NJS_TEXLIST ocean_pot_piece_texlist_3;
extern NJS_TEXLIST ocean_pot_piece_texlist_4;
extern NJS_TEXLIST ocean_pot_texlist;
/* the pot's texture lists and its pieces', for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *ocean_pot_texlists[];

/* the object list's step for TSUBO. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise task takes the pot's own
 * step, draw and destructor, and a collision record of kind COLLISION_OBJECT.
 * the record holds one cylinder, 15 tall in Sand Ocean and 20 in Hidden Base.
 * the heap must have room for the record, which the step reads.
 *
 * task needs a work, whose pos places the pot and whose ang.y turns it. in
 * Hidden Base, an even whole part of |scl.z| picks the bright pot. an odd one
 * picks the dark pot. the pot breaks once its record touches a task whose
 * work's id is WORK_ID_ATTACK. its pieces then fly as debris_break_flags throws
 * them. the break sound plays at pos, and placement_kill ends the task */
void ocean_pot_init(struct task *task);

#endif

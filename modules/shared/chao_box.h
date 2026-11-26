/* the Chao box, CONTCHAO in a stage's object list: a box a player's attack
 * breaks. it leaves the stage's Chao key, and it falls when a box under it
 * breaks */
#ifndef SHARED_CHAO_BOX_H
#define SHARED_CHAO_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* compile the box's model and its first two pieces' at buf, and return buf
 * past what it compiled. each compiles once, and only while its texture list
 * is loaded. the boxes made after a call that compiles all three draw the
 * compiled model */
Uint8 *chao_box_build(Uint8 *buf);
/* the object list's step for CONTCHAO. task needs a work, whose pos is the
 * center of the box's bottom and whose ang.y turns it. the call clears ang.x
 * and ang.z. task's extra must not hold a record, which the box would
 * overwrite with its floor's height. the heap must have room for the box's
 * collision record, unchecked.
 *
 * stack_add of shared/common_object.h enters the box in stack_tasks. the box
 * falls STACK_HEIGHT for each drop stack_drop gives it. a broken box drops
 * the tasks stacked on it. where stack_add returns 0, the box never falls.
 *
 * once its pieces have flown, a box leaves the Chao key, or small animals
 * where chao_key_taken returns TRUE. the first box to release animals
 * releases four, the second releases one, and later boxes release none.
 * CHAO_BOX_FOUR_ANIMALS and CHAO_BOX_LAST_ANIMAL of object/fall.h record the
 * two releases. a box without a placement, or whose placement has
 * PLACEMENT_USED or PLACEMENT_UNK_20, leaves nothing. a box with a placement
 * sets both flags as it ends */
void chao_box_init(struct task *task);

/* the box's model, and the models of the three pieces it breaks into */
extern NJS_TEXLIST chao_box_texlist;
extern NJS_CNK_MODEL chao_box_model;
extern NJS_CNK_OBJECT chao_box_object;
extern NJS_TEXLIST chao_box_piece1_texlist;
extern NJS_CNK_MODEL chao_box_piece1_model;
extern NJS_CNK_OBJECT chao_box_piece1_object;
extern NJS_TEXLIST chao_box_piece2_texlist;
extern NJS_CNK_MODEL chao_box_piece2_model;
extern NJS_CNK_OBJECT chao_box_piece2_object;
extern NJS_TEXLIST chao_box_piece3_texlist;
extern NJS_CNK_MODEL chao_box_piece3_model;
extern NJS_CNK_OBJECT chao_box_piece3_object;

#endif

/* the air item box, ITEMBOXAIR in a stage's object list. a player breaks it
 * for the item inside, and the box then swells, fades out and ends */
#ifndef SHARED_ITEM_BOX_AIR_H
#define SHARED_ITEM_BOX_AIR_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the box, its icon and its
 * puff, ended by null */
extern NJS_TEXLIST *item_box_air_texlists[];

/* the object list's step for ITEMBOXAIR. task needs a work, and must be created
 * without TASK_FORCE. the box clears task->force as it is destroyed, so
 * TASK_FORCE's records would never be freed. the box keeps an enemy_work in
 * task->motion, and its item's index in its work's tick. the heap must have
 * room for its collision record and its enemy_work, unchecked.
 *
 * the work's scl.x, cut to a whole number, picks the item from the list
 * item_box_step's contract gives. the box keeps the number's low byte, and a
 * byte above 10 counts as 10. ang.y and ang.z turn the box.
 *
 * the box breaks and gives its item as an ITEMBOX does. a broken box swells to
 * 5 times its size as it fades out, then kills its task.
 *
 * a box that breaks sets PLACEMENT_USED in its placement. a box whose placement
 * has the flag kills its task on its first step. a box of ITEM_1UP whose
 * placement has PLACEMENT_UNK_20 holds ITEM_EMPTY instead. a box that gives its
 * 1-up sets PLACEMENT_UNK_20 in its placement, through null where it has none.
 *
 * a box whose work's submode isn't 0 is a child box, whose task must have a
 * parent, unchecked. a child box never leaves range. it holds ITEM_EMPTY in
 * place of ITEM_1UP where its own placement or its parent's has
 * PLACEMENT_UNK_20. it sets PLACEMENT_USED in each of the two placements that
 * exists. it sets PLACEMENT_UNK_20 in its parent's placement alone, through
 * null where the parent has none. where the parent's placement has
 * PLACEMENT_USED, the child's first step kills the parent. a child that has
 * faded out kills its parent too, and task_destroy destroys the child with
 * it */
void item_box_air_step(struct task *task);

#endif

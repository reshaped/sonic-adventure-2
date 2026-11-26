/* the solid box, SOLIDBOX in a stage's object list: a box no player breaks.
 * it falls when a box under it breaks */
#ifndef SHARED_SOLID_BOX_H
#define SHARED_SOLID_BOX_H
#include <ninja.h>

struct task;

/* the box's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *solid_box_texlists[];

/* the object list's step for SOLIDBOX. task needs a work, whose pos is the
 * center of the box's bottom and whose ang.y turns it. the call clears ang.x
 * and ang.z. task's extra must not hold a record, which the box would
 * overwrite with its floor's height.
 *
 * stack_add of shared/common_object.h enters the box in stack_tasks. the box
 * falls STACK_HEIGHT for each drop stack_drop gives it. where stack_add
 * returns 0, the box never falls */
void solid_box_init(struct task *task);

#endif

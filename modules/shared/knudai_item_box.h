/* the item box a flute stand calls up, KDITEMBOX in a stage's object list.
 * an air item box appears once the object's stand is played */
#ifndef SHARED_KNUDAI_ITEM_BOX_H
#define SHARED_KNUDAI_ITEM_BOX_H
#include <ninja.h>

struct task;

/* the ring sparkles' texture list for texture_load_shared, ended by null.
 * nothing of the object draws with it */
extern NJS_TEXLIST *knudai_item_box_texlists[];

/* the object list's step for KDITEMBOX. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * object's step and destructor. task needs a work. its ang.x holds the id of
 * the object's stand.
 *
 * the object waits until knudai_played returns 1 for its stand, and then sets
 * its placement's state to 1. an object whose placement's state isn't 0
 * skips the wait. after the wait, the object makes an air item box of
 * shared/item_box_air.h as task's child. where task_create_child fails, the
 * next step tries again. the object makes one box at most, and the box takes
 * task's pos, ang and scl. the object then sets its work's mode to 0, which
 * nothing of the object reads.
 *
 * the box acts on task as item_box_air.h says a child box acts on its
 * parent. task ends once it leaves range, and the box ends with it */
void knudai_item_box_init(struct task *task);

#endif

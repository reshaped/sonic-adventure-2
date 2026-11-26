/* the dummy object, DMYOBJ in a stage's object list. it loads chunks,
 * textures and texture animations as dummy_request asks, and draws them */
#ifndef SHARED_DUMMY_OBJECT_H
#define SHARED_DUMMY_OBJECT_H

struct task;

/* the object list's step for DMYOBJ. unless placement_try_leave(task) returns
 * TRUE, it gives task the dummy object's own step, draw and destructor. it
 * counts the object in dummy_object_count, and sets dummy_request.ready to
 * TRUE. task needs a work, whose submode it zeroes.
 *
 * the object's step serves dummy_request, as object/fall.h says. the object
 * draws at the work's pos, turned by its ang. it draws a grid while scl.x is
 * 1 or more. it draws what the dummy objects have loaded while
 * dummy_request.show is TRUE. the dummy objects share what they load, and the
 * last one's destructor frees it */
void dummy_object_init(struct task *task);

#endif

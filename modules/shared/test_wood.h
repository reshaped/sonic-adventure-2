/* the test wood, TESTWOOD in a stage's object list: one model of scenery,
 * turned and scaled. nothing collides with it */
#ifndef SHARED_TEST_WOOD_H
#define SHARED_TEST_WOOD_H
#include <ninja.h>

struct task;

/* the wood's texture list, of three textures. a stage's texture groups hold
 * it */
extern NJS_TEXLIST test_wood_texlist;

/* the object list's step for TESTWOOD. unless placement_try_leave_by(task,
 * 2050 squared) returns TRUE, it gives task the wood's own step, draw and
 * destructor. the step only makes the same test.
 *
 * task needs a work. the draw places the model at pos, turned by ang through
 * njRotateZ, njRotateX and njRotateY, in that order. it scales the model by 1
 * more than each of scl's fields */
void test_wood_init(struct task *task);

#endif

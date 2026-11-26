/* the fallen tree, TOBOKU and TOBOKU_FAR in a stage's object list: a log
 * that the players collide with */
#ifndef SHARED_FALLEN_TREE_H
#define SHARED_FALLEN_TREE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST fallen_tree_texlist;

/* the object list's steps for TOBOKU and TOBOKU_FAR. TOBOKU's tree leaves at
 * its placement's range, and TOBOKU_FAR's at 810. task needs a work. pos
 * places the tree, and ang turns it by njRotateZ, njRotateY, then njRotateX.
 *
 * the call puts a copy of the tree's collision object in the world as a
 * solid. task->motion keeps the copy. world_pool_alloc lends the copy.
 * the pool must have one to lend, unchecked: without one, the call copies
 * through null. the copy collides only while a player lies within the
 * model's radius plus 10 of its center. the destructor takes the copy out of
 * the world and back to the pool */
void fallen_tree_init(struct task *task);
void fallen_tree_far_init(struct task *task);

#endif

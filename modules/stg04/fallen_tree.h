/* White Jungle's fallen tree, TOBOKU and TOBOKU_FAR in the stage's object
 * list. it is the fallen tree of shared/fallen_tree.h over a model, a shape
 * and textures of its own */
#ifndef STG04_FALLEN_TREE_H
#define STG04_FALLEN_TREE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST white_jungle_fallen_tree_texlist;

/* the object list's steps for TOBOKU and TOBOKU_FAR. they do what
 * fallen_tree_init and fallen_tree_far_init do, and the work's fields mean
 * what they mean there */
void white_jungle_fallen_tree_init(struct task *task);
void white_jungle_fallen_tree_far_init(struct task *task);

#endif

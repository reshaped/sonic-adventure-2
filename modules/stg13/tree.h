/* the tree, TREEST in City Escape's object list: a tree that faces the
 * camera */
#ifndef STG13_TREE_H
#define STG13_TREE_H
#include <ninja.h>

struct task;

/* the tree's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *tree_texlists[];

/* the object list's step for TREEST. unless placement_try_leave(task) returns
 * TRUE, it gives task the tree's own step, draw and destructor. it also gives
 * task a collision record of the trunk.
 *
 * task needs a work. pos places the tree, and ang.y turns its model's root,
 * a quad. the root's child, the tree itself, turns to the current camera's
 * ang.y */
void tree_init(struct task *task);

#endif

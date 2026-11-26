/* a plain tree: TREESTNB in City Escape's object list, and TREEST in the Big
 * Foot stage's. City Escape's own TREEST also draws a quad on the ground under
 * its tree. a plain tree's model has a child that turns about y to face the
 * camera */
#ifndef SHARED_PLAIN_TREE_H
#define SHARED_PLAIN_TREE_H
#include <ninja.h>

struct task;

/* the tree's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *plain_tree_texlists[];

/* the object list's step for TREESTNB and TREEST. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the tree's
 * own step, draw and destructor, and sets its work's mode to 0. task needs a
 * work, whose pos places the tree and whose ang.y turns it. the tree takes
 * no collision */
void plain_tree_init(struct task *task);

extern NJS_TEXLIST plain_tree_texlist;
extern NJS_CNK_OBJECT plain_tree_object;

#endif

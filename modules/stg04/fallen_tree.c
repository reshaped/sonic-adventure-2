/* the fallen tree of stg04/fallen_tree.h: shared/fallen_tree.c compiled a
 * second time. each name the tree exports begins with white_jungle_ here. its
 * textures, model and shape are its own, and its polygon list is shorter */
#include "stg04/fallen_tree.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define fallen_tree_init white_jungle_fallen_tree_init
#define fallen_tree_far_init white_jungle_fallen_tree_far_init
#define fallen_tree_texlist white_jungle_fallen_tree_texlist
/* NOLINTEND(readability-identifier-naming) */
#define FALLEN_TREE_POLYGON_HALVES 185

#include "shared/fallen_tree.c" /* NOLINT(bugprone-suspicious-include) */

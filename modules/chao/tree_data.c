/* the phases of a tree's branch sway and fruit swing, and the fruit counts.
 * the trees count their fruit into one of tree_fruit_counts while the other
 * holds the last pass's count. garden_advance_fruit_counts swaps the two */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/tree.h"
#include "chao/garden_save.h"

/* six positions for trees in each garden, which no code reads */
NJS_POINT3 garden_tree_layouts[CHAO_GARDENS][GARDEN_TREES] = {
#include "chao/tree_data/garden_tree_layouts.inc"
};

Angle tree_branch_phases[5] = {0x1000, 0x3200, 0x5300, 0xb500, 0xa000};
Angle tree_fruit_phases[4] = {0x1000, 0x3200, 0x5300, 0x8000};
Sint32 tree_fruit_counts[2] = {0, 0};
Sint32 tree_fruit_node_index = 0;

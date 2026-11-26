/* the garden controller's objects, a row of GARDEN_ROW_SLOTS for each object
 * category, and how many each row holds. garden_view is the matrix the
 * controller took as the view, and garden_view_inverse its inverse */
#ifndef CHAO_WORLD_SLOTS_H
#define CHAO_WORLD_SLOTS_H
#include "chao/world.h"

#define GARDEN_ROW_SLOTS 16
#ifdef NONMATCHING
/* this side's table is flat. so toy slots 16 to 31 reach the seed row's
 * entries, as in the image, inside the table */
typedef struct garden_entry
    garden_entry_table[GARDEN_OBJECT_CATEGORIES * GARDEN_ROW_SLOTS];
#else
typedef struct garden_entry garden_entry_table[GARDEN_OBJECT_CATEGORIES]
                                              [GARDEN_ROW_SLOTS];
#endif

extern NJS_MATRIX garden_view, garden_view_inverse;
extern Sint32 garden_object_counts[GARDEN_OBJECT_CATEGORIES];
extern garden_entry_table garden_entries;
extern struct task *garden_controller;

#endif

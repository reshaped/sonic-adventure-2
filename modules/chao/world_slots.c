#include <ninja.h>
#include <sg_xpt.h>
#include "chao/world.h"
#include "system/task.h"

#include "chao/world_slots.h"

NJS_MATRIX garden_view;
NJS_MATRIX garden_view_inverse;
Sint32 garden_object_counts[GARDEN_OBJECT_CATEGORIES];
garden_entry_table garden_entries;
struct task *garden_controller;

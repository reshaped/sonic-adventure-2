#include <sg_xpt.h>

#include "chao/terrain.h"

/* clang-format off */
Sint32 neutral_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS] = {
#include "chao/terrain/neutral_spawn_grid.inc"
};

Sint32 hero_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS] = {
#include "chao/terrain/hero_spawn_grid.inc"
};

Sint32 dark_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS] = {
#include "chao/terrain/dark_spawn_grid.inc"
};

/* clang-format on */

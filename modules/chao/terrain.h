/* each garden's terrain kind for each cell of its floor, 0 for none. a cell
 * is ten units on a side. row 20 starts at world z 0, and column 16 at
 * world x 0. z grows with the row, and x with the column */
#ifndef CHAO_TERRAIN_H
#define CHAO_TERRAIN_H
#include <sg_xpt.h>

#define GARDEN_GRID_ROWS 40
#define GARDEN_GRID_COLUMNS 32
extern Sint32 neutral_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS];
extern Sint32 hero_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS];
extern Sint32 dark_spawn_grid[GARDEN_GRID_ROWS][GARDEN_GRID_COLUMNS];

#endif

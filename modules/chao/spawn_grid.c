/* points on the current garden's floor, picked by terrain kind from
 * terrain.h's grids */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <string.h>

#include "chao/garden.h"
#include "chao/spawn_grid.h"
#include "chao/terrain.h"
#include "system/util.h"

enum {
    ORIGIN_COLUMN = GARDEN_GRID_COLUMNS / 2,
    ORIGIN_ROW = GARDEN_GRID_ROWS / 2
};
#define TERRAIN_KINDS 16
#define CELLS_PER_KIND 512
#define CELL_SIZE 10.0F
#define CELLS_PER_UNIT (1.0F / CELL_SIZE)
/* the bounds garden_spawn_kind reads the grid within */
#define KIND_MIN_X (-150.0F)
#define KIND_MAX_X 150.0F
#define KIND_MIN_Z (-200.0F)
#define KIND_MAX_Z 150.0F
/* the cells garden_spawn_kind adds before it truncates, which keep the
 * operand positive, so the truncation rounds down */
#define CELL_BIAS 100

struct grid_cell {
    Sint32 column, row;
};

struct terrain_cells {
    Sint32 count;
    struct grid_cell cells[CELLS_PER_KIND];
};

static struct terrain_cells garden_spawn_cells[TERRAIN_KINDS];

void garden_spawn_grid_init(void)
{
    int row;
    int column;
    int kind;
    struct terrain_cells *terrain;

    memset(garden_spawn_cells, 0, sizeof(garden_spawn_cells));
    for (row = 0; row < GARDEN_GRID_ROWS; row++) {
        for (column = 0; column < GARDEN_GRID_COLUMNS; column++) {
            kind = 0;
            switch (chao_current_area()) {
            case CHAO_AREA_NEUTRAL:
                kind = neutral_spawn_grid[row][column];
                break;
            case CHAO_AREA_HERO:
                kind = hero_spawn_grid[row][column];
                break;
            case CHAO_AREA_DARK:
                kind = dark_spawn_grid[row][column];
                break;
            }
            if (kind >= 1 && kind < TERRAIN_KINDS) {
                terrain = &garden_spawn_cells[kind];
                terrain->cells[terrain->count].row = row;
                terrain->cells[terrain->count].column = column;
                terrain->count++;
            }
        }
    }
}

Bool garden_spawn_pick(int kind, NJS_POINT3 *pos)
{
    struct terrain_cells *terrain = &garden_spawn_cells[kind];
    int count = terrain->count;
    Float pick;
    int index;
    int column;
    int row;

    if (count > 0) {
        /* @hack spelling: hold the random index in a Float before truncating
         * it, where a cast of the expression would do */
        pick = rand() * (count - 0.00001F) * RAND_SCALE;
        index = (int)pick;
        column = terrain->cells[index].column;
        row = terrain->cells[index].row;
        pos->x = ((column - ORIGIN_COLUMN) + (rand() * RAND_SCALE)) * CELL_SIZE;
        pos->z = ((row - ORIGIN_ROW) + (rand() * RAND_SCALE)) * CELL_SIZE;
    } else {
        return FALSE;
    }
    return TRUE;
}

int garden_spawn_kind(const NJS_POINT3 *pos)
{
    Float x = pos->x;
    Float z = pos->z;
    int kind = 0;
    int column;
    int row;

    if (!(x < KIND_MIN_X || x > KIND_MAX_X || z < KIND_MIN_Z ||
          z > KIND_MAX_Z)) {
        column = (int)((x * CELLS_PER_UNIT) + CELL_BIAS) -
                 (CELL_BIAS - ORIGIN_COLUMN);
        row =
            (int)((z * CELLS_PER_UNIT) + CELL_BIAS) - (CELL_BIAS - ORIGIN_ROW);
        switch (chao_current_area()) {
        case CHAO_AREA_NEUTRAL:
            kind = neutral_spawn_grid[row][column];
            break;
        case CHAO_AREA_HERO:
            kind = hero_spawn_grid[row][column];
            break;
        case CHAO_AREA_DARK:
            kind = dark_spawn_grid[row][column];
            break;
        }
        return kind;
    }
    /* @bug uninitialized: a point out of bounds gets no kind. the tests at
     * 0x0c50ff76, 0x0c50ff80, 0x0c50ff88 and 0x0c50ff8c branch past the
     * result at 0x0c50ffe4. R0 then holds the address the last MOVA loaded
     * a bound from: 0x0c510000, 0x0c510004 or 0x0c510008 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a point out of bounds takes 8, as
     * spawn_grid.h says */
    return 8;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

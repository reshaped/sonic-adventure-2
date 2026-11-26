/* points on the current garden's floor, picked by terrain kind from
 * terrain.h's grids */
#ifndef CHAO_SPAWN_GRID_H
#define CHAO_SPAWN_GRID_H
#include <ninja.h>
#include <sg_xpt.h>

/* the terrain kinds a Chao seeks by name. kind 0 is no terrain */
enum garden_terrain {
    TERRAIN_WANDER = 1, /* where a Chao picks the point it wanders to */
    TERRAIN_WATER = 9
};

/* list the current garden's cells by kind, for garden_spawn_pick. the lists
 * hold until the next call */
void garden_spawn_grid_init(void);
/* set pos's x and z to a random point in a cell of kind and return TRUE.
 * kind must be from 0 to 15. where the garden has no cell of kind, return
 * FALSE and leave pos */
Bool garden_spawn_pick(int kind, NJS_POINT3 *pos);
/* the terrain kind at pos, for x from -150 to 150 and z from -200 to 150.
 * outside those bounds the image returns an address that is no kind, and the
 * non-matching build returns 8 */
int garden_spawn_kind(const NJS_POINT3 *pos);

#endif

/* Egg Quarters' land blocks, as bits of land_block_mask. north lies toward
 * +z and east toward +x */
#ifndef STG26_QUARTERS_AREA_H
#define STG26_QUARTERS_AREA_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

enum quarters_area {
    QUARTERS_WEST = 0x2,
    QUARTERS_EAST = 0x4,
    QUARTERS_SOUTH = 0x8,
    QUARTERS_NORTH = 0x00200000,
    QUARTERS_FAR_NORTHWEST = 0x00400000,
    QUARTERS_FAR_NORTHEAST = 0x00800000,
    QUARTERS_NORTHWEST = 0x01000000,
    QUARTERS_NORTHEAST = 0x02000000,
    QUARTERS_INNER_NORTHWEST = 0x04000000,
    QUARTERS_INNER_SOUTHWEST = 0x08000000,
    QUARTERS_INNER_NORTHEAST = 0x10000000,
    QUARTERS_INNER_SOUTHEAST = 0x20000000,
    QUARTERS_CENTER = 0x40000000
};

/* the land block that holds pos, by its x and z */
enum quarters_area quarters_area_at(NJS_POINT3 *pos);
/* whether the guard is within guard_view_distance of player 0, in a block
 * that camera 0's block sees. the center sees only the south. each block of
 * the ring around it sees itself and the two beside it in the ring. no other
 * block sees any */
Bool quarters_guard_visible(struct task *guard);

#endif

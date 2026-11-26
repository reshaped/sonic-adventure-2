/* Death Chamber's lit floors: a task named ObjectFloorExec draws two floors of
 * tiles, 3 rows of 8 each. the tiles flash white and dim again, in three groups
 * by turns */
#ifndef STG25_FLOOR_TILES_H
#define STG25_FLOOR_TILES_H
#include <ninja.h>

struct task;

/* the tiles' texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_floor_tiles_texlists[];

/* make the floors' task, ObjectFloorExec, at task level 1, and return it.
 * the heap must have room for the task, unchecked. the hall's floor draws
 * while camera 0's land blocks hold HALL_BLOCKS of stg25/death_chamber.h. the
 * far east's draws while they hold FAR_EAST_BLOCKS */
struct task *chamber_floor_tiles_create(void);

#endif

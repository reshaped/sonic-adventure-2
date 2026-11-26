/* Sky Rail, the stage of STG06. the overlay's head names its start, its end
 * and its task's step */
#ifndef STG06_SKY_RAIL_H
#define STG06_SKY_RAIL_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table sky_rail_land;
extern const char sky_rail_land_filenames[302];
extern NJS_TEXLIST sky_rail_land_texlist;

struct task;

/* start the stage. among the rest, it starts the land, the set manager and
 * the background task, BgExec. it sets stage_hooks. it fills each of
 * player_hooks' drop_rings, unk_04, add_item_icon, create_piece and
 * show_piece_sign that is null. it starts the fog from stg06_fog.bin, then
 * turns Ninja's fog off. it gives player 0 a smoke task, and player 1 one in a
 * two-player game */
void sky_rail_init(void);
/* end the stage. it frees the hints, the messages and the set file, and
 * releases the textures. it stops the set manager, the object lights, the
 * fog, the particles and the land. it leaves Ninja's fog off. it takes the
 * stage's motions out of the motion table. it clears each of player_hooks'
 * drop_rings, unk_04, add_item_icon, create_piece and show_piece_sign that
 * still holds the stage's call. it leaves stage_hooks set, and destroys none
 * of the stage's tasks */
void sky_rail_exit(void);
/* the stage task's step, which does nothing. task needs a work */
void sky_rail_step(struct task *task);
/* the stage's object list, which sky_rail_init gives to set_start */
extern struct object_list sky_rail_objects;

#endif

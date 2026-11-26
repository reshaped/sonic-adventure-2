/* Prison Lane, the stage of STG09. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG09_PRISON_LANE_H
#define STG09_PRISON_LANE_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table prison_lane_land;
extern const char prison_lane_land_filenames[367];
extern NJS_TEXLIST prison_lane_terrain_textures;

/* the stage's kinds of object, which the set file's entries index.
 * prison_lane_init loads the textures in prison_lane_pvms before set_start
 * makes the kinds' tasks */
#define PRISON_LANE_KINDS 99
extern struct object_kind prison_lane_kinds[PRISON_LANE_KINDS];
extern struct texture_pvm prison_lane_pvms[];
extern NJS_TEXLIST **prison_lane_texture_groups[];

/* start the stage. among the rest, it starts the land, the object list and
 * the fog. it starts the background task, BgExec, and the kill areas. it
 * fills each of player_hooks' drop_rings, add_item_icon and draw_blast_ring
 * that is null, and sets stage_hooks. it leaves Ninja's fog off */
void prison_lane_init(void);
/* end the stage. it frees the hints, the messages, the block grid and the set
 * file, and releases the textures. it stops the fog, the particles, the set
 * manager and the land. it takes the stage's motions out of the motion table.
 * it clears each of player_hooks' drop_rings, add_item_icon and draw_blast_ring
 * that still holds the stage's call. it sets the back color to black */
void prison_lane_exit(void);
/* the stage task's step, which sets the back color to the dark red
 * 0xff250d00. task needs a work */
void prison_lane_step(struct task *task);
/* the stage's object list, which prison_lane_init gives to set_start */
extern struct object_list prison_lane_objects;

#endif

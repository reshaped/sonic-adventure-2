/* Sand Ocean, stg21: the stage's init, exit and step, and its object list */
#ifndef STG21_SAND_OCEAN_H
#define STG21_SAND_OCEAN_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

extern struct land_table sand_ocean_land;
extern const char sand_ocean_land_filenames[226];
extern NJS_TEXLIST sand_ocean_land_texlist;
extern NJS_TEXLIST **sand_ocean_texture_groups[];
extern struct texture_pvm sand_ocean_pvms[];

/* the overlay's init and exit, which stg21/head.c gives overlay_run_init and
 * overlay_run_exit.
 *
 * sand_ocean_init starts the stage. among the rest, it starts the land, the
 * set manager and the music. it also starts the paths, the kill areas and the
 * fog, and the tasks BgClip and BgExec. BgClip sets camera 0's
 * land_block_mask to the block of stg21/quicksand.h that holds the current
 * camera. BgExec plays the sand's sound. its draw shows the stars, the moon,
 * the quicksand and the pyramids seen from that block. sand_ocean_init fills
 * each of player_hooks' drop_rings, add_item_icon and draw_blast_ring that is
 * null. it sets stage_hooks too, and tints the stacked platforms' fragments.
 *
 * sand_ocean_exit ends the stage. it frees the set file, the hints and the
 * messages, and releases the textures. it destroys the object light, and stops
 * the set manager, the fog, the particles and the land. it leaves Ninja's fog
 * off. it takes the stage's motions out of the motion table. it clears each of
 * player_hooks' drop_rings, add_item_icon and draw_blast_ring that still holds
 * the stage's call */
void sand_ocean_init(void);
void sand_ocean_exit(void);
/* the stage task's step, which does nothing. task needs a work, whose mode it
 * reads */
void sand_ocean_step(struct task *task);

#define SAND_OCEAN_KINDS 89
extern struct object_kind sand_ocean_kinds[SAND_OCEAN_KINDS];
/* the stage's object list, which sand_ocean_init gives to set_start */
extern struct object_list sand_ocean_objects;

#endif

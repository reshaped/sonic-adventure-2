/* Sand Ocean's two-player stage, stg45: the stage's init, exit and step */
#ifndef STG45_SAND_OCEAN_2P_H
#define STG45_SAND_OCEAN_2P_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table sand_ocean_2p_land;
extern const char sand_ocean_2p_land_filenames[196];
extern NJS_TEXLIST sand_ocean_2p_land_texlist;
extern NJS_TEXLIST **sand_ocean_2p_texture_groups[];
extern struct texture_pvm sand_ocean_2p_pvms[];

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * sand_ocean_2p_init starts the stage. among the rest, it starts the land,
 * the set manager, the kill area and the background task, BgExec. it adds the
 * stage's motions to the motion table too. it starts smoke_task_create's task
 * for player 0, and for player 1 in a TWO_PLAYERS_3_POINTS match. it starts
 * broken_down_smoke_create's task for player 0, and for player 1 where player
 * 1 has a task. it fills player_hooks' drop_rings, add_item_icon and
 * draw_blast_ring with the stage's own calls, each only where it is empty. it
 * sets stage_hooks too, and njClipZ's planes.
 *
 * sand_ocean_2p_exit ends the stage. it frees the set file and releases the
 * textures. it stops the set manager, the object lights, the fog, the
 * particles and the land. it takes the stage's motions out of the motion
 * table. it empties each of those three calls that still holds the stage's
 * own. it leaves njClipZ's planes as the init set them */
void sand_ocean_2p_init(void);
void sand_ocean_2p_exit(void);
/* the stage task's step, which does nothing but read the work's mode. task
 * needs a work */
void sand_ocean_2p_step(struct task *task);
/* the stage's object list, which sand_ocean_2p_init gives to set_start */
extern struct object_list sand_ocean_2p_objects;

#endif

/* Pumpkin Hill, the stage of STG05. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG05_PUMPKIN_HILL_H
#define STG05_PUMPKIN_HILL_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table pumpkin_hill_land;
extern NJS_TEXLIST pumpkin_hill_land_texlist;

/* start the stage. among the rest, it starts the land, the set manager and
 * the background task, BgExec. the set manager takes pumpkin_hill_2p_objects
 * in a two-player game, and pumpkin_hill_objects otherwise. it sets
 * stage_hooks. it fills each of player_hooks' drop_rings, unk_04,
 * add_item_icon, create_piece, show_piece_sign, explode, throw_skull and flash
 * that is null. it gives the hunt the stage's hints and the paths of its moving
 * pieces. it starts the fog from stg05_fog.bin, then turns Ninja's fog off. it
 * gives player 0 a smoke task, and player 1 one in a two-player game */
void pumpkin_hill_init(void);
/* end the stage. it frees the hints, the messages and the set file, and
 * releases the textures. it stops the set manager, the object lights, the fog,
 * the particles and the land. it leaves Ninja's fog off. it takes the stage's
 * motions out of the motion table. it clears each of player_hooks' drop_rings,
 * unk_04, add_item_icon, create_piece, show_piece_sign, explode, throw_skull
 * and flash that still holds the stage's call. it leaves stage_hooks set, and
 * destroys none of the stage's tasks */
void pumpkin_hill_exit(void);
/* the stage task's step. where no updraft task lives, it creates one,
 * ObjSetManExec, and runs that task's first step at once. the updraft lifts
 * the players in slots 0 and 1 out of the pit under the stage. task needs a
 * work */
void pumpkin_hill_step(struct task *task);
/* the stage's object lists, for a one-player and a two-player game */
extern struct object_list pumpkin_hill_objects;
extern struct object_list pumpkin_hill_2p_objects;

#endif

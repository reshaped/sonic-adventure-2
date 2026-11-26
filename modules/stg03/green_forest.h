/* Green Forest, the stage of STG03. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG03_GREEN_FOREST_H
#define STG03_GREEN_FOREST_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

/* The land shares its texture slots with the ground and water archives. */
extern struct land_table green_forest_land;
extern const char green_forest_land_filenames[545];
extern NJS_TEXLIST green_forest_land_texlist;
extern NJS_TEXLIST green_forest_ground_texlist;
extern NJS_TEXLIST green_forest_water_texlist;

/* start the stage. among the rest, it starts the land, the object list and the
 * background task, BgExec. it fills each of player_hooks' drop_rings and splash
 * that is null, and sets stage_hooks. it gives each player in slots 0 and 1 a
 * HosiExec task of stars. it gives none while either player's stars from an
 * earlier start still live */
void green_forest_init(void);
/* end the stage. it destroys the stars, and releases the textures, the set
 * file, the messages and the hints. it stops the set manager, the particles
 * and the land. it takes the stage's motions out of the motion table. it
 * clears each of player_hooks' drop_rings and splash that still holds the
 * stage's call */
void green_forest_exit(void);
/* the stage task's step, which does nothing */
void green_forest_step(struct task *task);
/* the stage's object list, which green_forest_init gives to set_start */
extern struct object_kind green_forest_kinds[88];
extern struct object_list green_forest_objects;
extern NJS_TEXLIST **green_forest_object_texture_groups[];
extern struct texture_pvm green_forest_object_pvms[];

#endif

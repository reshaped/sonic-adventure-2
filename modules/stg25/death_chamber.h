/* Death Chamber, stg25: the stage's init, exit and step, and its background.
 * the background picks the land blocks camera 0 draws. it plays the sounds
 * of the turning center and the pools. it sets the fog, and draws the
 * scenery and the water */
#ifndef STG25_DEATH_CHAMBER_H
#define STG25_DEATH_CHAMBER_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table death_chamber_land;
extern const char death_chamber_land_filenames[691];
extern NJS_TEXLIST death_chamber_land_texlist;
extern NJS_TEXLIST death_chamber_water_texlist;

struct task;

/* the stage's land blocks, as game_state.land_block_mask holds them. the
 * background gives camera 0 the blocks of the place where the current camera
 * stands. west is toward -x, and south toward -z */
#define WEST_FAR_BLOCKS 0x2
#define EAST_FAR_BLOCKS 0x4
#define SOUTH_EAST_FAR_BLOCKS 0x8
#define CENTER_NORTH_BLOCKS 0x20
#define REST_BLOCKS 0x800
#define CENTER_SOUTH_BLOCKS 0x1000
#define WEST_NEAR_BLOCKS 0x2000
#define EAST_NEAR_BLOCKS 0x4000
#define SOUTH_EAST_NEAR_BLOCKS 0x8000
#define SOUTH_WEST_BLOCKS 0x10000
#define HALL_BLOCKS 0x200000
#define NORTH_BLOCKS 0x400000
#define FAR_EAST_BLOCKS ((Sint32)0x80000000)
#define CENTER_BLOCKS (CENTER_NORTH_BLOCKS | CENTER_SOUTH_BLOCKS)

/* the overlay's init and exit, which stg25/head.c's stg25_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * death_chamber_init loads the stage's land, its objects and their textures,
 * and compiles their models. it reads the camera, light and fog files. it loads
 * the hints, the messages and the sounds. it starts the music, the paths and
 * the kill area. it also starts the particles, the object lights and the
 * water's specks. it starts the land blocks' task, the background's, the
 * footsteps, the smoke and stg25/floor_tiles.h's task. it sets the emerald
 * pieces' look, their paths and stage_hooks. it fills each null call of
 * player_hooks with the stage's own. it registers the stage's motions in the
 * motion table. it clears the switches, and sets game_state.block_grid_loaded.
 *
 * death_chamber_exit frees the set file, the hints and the messages, and
 * releases the textures. it stops the set manager, the land and the fog. it
 * also stops the particles, the object lights and the water's specks. it puts
 * back the emerald pieces' look, and takes away their paths. it takes the
 * stage's motions out of the motion table. it nulls each call of player_hooks
 * that still holds the stage's own. it leaves stage_hooks and
 * game_state.block_grid_loaded as init left them. it also leaves the sounds,
 * the music, the paths, the kill area and the other tasks running */
void death_chamber_init(void);
void death_chamber_exit(void);
/* the step of the stage's task. it starts the fog with no setting, where the
 * fog doesn't run */
void death_chamber_step(struct task *task);

/* the flames' textures for the props of stg25/flame_prop.h, which the disc
 * holds. each color of stg25/chamber_color.h has 16 frames */
extern NJS_TEXLIST chamber_flame_blue_texlist;
extern NJS_TEXLIST chamber_flame_green_texlist;
extern NJS_TEXLIST chamber_flame_red_texlist;

/* the stage's object list, which death_chamber_init gives to set_start */
extern struct object_list death_chamber_objects;

/* the map's three pictures, dz_dc_map01 to dz_dc_map03, which the disc
 * holds. the overlay of stg25/map.h shows them in turn */
extern NJS_TEXLIST death_chamber_map_texlist;

#endif

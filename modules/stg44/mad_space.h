/* Mad Space, stg44: the stage's init, exit and step, and its background */
#ifndef STG44_MAD_SPACE_H
#define STG44_MAD_SPACE_H

#include <ninja.h>
#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table mad_space_land;
extern const char mad_space_land_filenames[605];
extern NJS_TEXLIST mad_space_land_texlist;

/* the stage's object list, which mad_space_init gives to set_start */
extern struct object_list mad_space_objects;
/* the overlay's init, which stg44/head.c's stg44_head gives overlay_run_init.
 * it starts the stage's land, objects, particles, paths and kill areas. it
 * starts the background's task, BgExec, and the object builds. it loads the
 * stage's textures, set file, camera files and light file. it starts the fog,
 * which it reads from both fog files, and the music. it loads the sounds and
 * the enemy bombs. it sets the hunt's hints and the emerald paths. it reads the
 * hunt's, the messenger's and the hint boxes' files. it compiles the land's,
 * the objects', the background's and the beacons' models into
 * STAGE_MODEL_BUILD_AREA of system/areas.h. it sets the lights, stage_hooks and
 * the debris tuning. it adds the stage's motions to the motion table. it fills
 * each of player_hooks' drop_rings, add_item_icon, unk_04, create_piece and
 * show_piece_sign that is null. it leaves Ninja's fog on */
void mad_space_init(void);
/* the overlay's exit, which stg44_head gives overlay_run_exit. it frees the set
 * file and the hunt's, the messenger's and the hint boxes' files. it releases
 * the textures and clears the emerald paths. it stops the land, the set
 * manager, the particles, the object builds and the fog. it leaves Ninja's fog
 * off. it takes the stage's motions out of the motion table. it clears each
 * player_hooks entry that mad_space_init fills and that still holds the
 * stage's own */
void mad_space_exit(void);
/* the stage task's step. it makes the carriage manager, ObjSetManExec, where
 * none lives. while player 0's y is below 500, it plays the wind at camera 0's
 * x and z. it sets world_gravity, world_gravity_ang_x and
 * world_gravity_ang_z to each player's way down in turn. a player's way down
 * is the fields' at the player's pos, or -y where no field gives one. where
 * both play, player 1's is the one left. a player's pad shakes where the
 * player's way down lies far from world_gravity as the step began. task needs
 * a work, and camera 0 must exist */
void mad_space_step(struct task *task);

#endif

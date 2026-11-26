/* White Jungle, the stage of STG04. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG04_WHITE_JUNGLE_H
#define STG04_WHITE_JUNGLE_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

/* White Jungle's SET entries index these kinds. Its object models obtain
 * their textures from the three object archives and the land's list. */
#define WHITE_JUNGLE_KINDS 83
extern struct object_kind white_jungle_kinds[WHITE_JUNGLE_KINDS];
extern struct texture_pvm white_jungle_object_pvms[];
extern NJS_TEXLIST **white_jungle_object_texture_groups[];

extern struct land_table white_jungle_land;
extern const char white_jungle_land_filenames[347];
extern NJS_TEXLIST white_jungle_terrain_textures;

/* start the stage. among the rest, it starts the land, the object list and the
 * background task, BgExec. it also starts the fog and the object lights. it
 * fills each of player_hooks' drop_rings and splash that is null, and sets
 * stage_hooks. it gives each player in slots 0 and 1 a HosiExec task of stars.
 * it gives none while either player's stars from an earlier start still live.
 * it leaves Ninja's fog off */
void white_jungle_init(void);
/* end the stage. it destroys the stars, and releases the textures, the set
 * file, the messages and the hints. it stops the object lights, the fog, the
 * set manager, the particles and the land. it takes the stage's motions out of
 * the motion table. it clears each of player_hooks' drop_rings and splash that
 * still holds the stage's call. it leaves Ninja's fog off */
void white_jungle_exit(void);
/* the stage task's step, which keeps the stage's ambient sound playing, keyed
 * by task. task needs a work */
void white_jungle_step(struct task *task);
/* the stage's object list, which white_jungle_init gives to set_start */
extern struct object_list white_jungle_objects;

#endif

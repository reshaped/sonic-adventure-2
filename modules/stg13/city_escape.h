/* City Escape, the stage of STG13. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG13_CITY_ESCAPE_H
#define STG13_CITY_ESCAPE_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table city_escape_land;
extern NJS_TEXLIST city_escape_land_texlist;

#define CITY_ESCAPE_KINDS 102
extern struct object_kind city_escape_kinds[CITY_ESCAPE_KINDS];
extern NJS_TEXLIST **city_escape_object_texture_groups[];
extern struct texture_pvm city_escape_object_pvms[];

/* the texture lists of the background's five pairs of models, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *background_build_texlists[];

/* start the stage. it starts the land, the set manager, the particles, the
 * paths, the kill areas, the object lights and the background task, BgExec.
 * it compiles the land's, the background's and the objects' models into
 * STAGE_MODEL_BUILD_AREA of system/areas.h. it starts the fog, and leaves
 * Ninja's fog on. it adds the stage's motions to the motion table. it fills
 * each of player_hooks' drop_rings and add_item_icon that is null, and sets
 * stage_hooks */
void city_escape_init(void);
/* end the stage. it frees the hints, the messages and the set file, and
 * releases the textures. it stops the land, the set manager, the particles
 * and the object lights. it stops the fog, and leaves Ninja's fog off. it
 * takes the stage's motions out of the motion table. it clears each of
 * player_hooks' drop_rings and add_item_icon that still holds the stage's
 * call */
void city_escape_exit(void);
/* the stage task's step. it plays the opening voice at 0:00 and 10 frames on
 * the stage's clock. it does so only in MISSION_FIRST outside a demo. a player
 * in slot 0 or 1 falls out of the stage below a height of -22195. one that has
 * missed doesn't. players fall only while kill_areas_task lives and has no
 * child. player_fall_out makes the miss's task that child, so no player falls
 * while a miss runs. task needs a work */
void city_escape_step(struct task *task);
/* the stage's object list, which city_escape_init gives to set_start */
extern struct object_list city_escape_objects;

#endif

/* Security Hall, the stage of STG08: its init, exit and step, its art and its
 * object kinds */
#ifndef STG08_SECURITY_HALL_H
#define STG08_SECURITY_HALL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "system/texture.h"

struct task;

extern NJS_TEXLIST *security_empty_textures[];
/* the hunt's pieces: one model, in a texture list for each piece */
extern NJS_CNK_MODEL security_piece_model;
extern NJS_TEXLIST security_piece_1_texlist;
extern NJS_TEXLIST security_piece_2_texlist;
extern NJS_TEXLIST security_piece_3_texlist;
extern struct texture_pvm security_pvms[];
extern NJS_TEXLIST **security_texture_groups[];

#define SECURITY_HALL_KINDS 85
extern struct object_kind security_hall_kinds[SECURITY_HALL_KINDS];

/* start the stage, as stg08/head.c's stg08_head gives overlay_run_init.
 * among the rest, it starts the land, the set manager and the particles. it
 * also starts the background task, BgExec. it compiles the land's and the
 * objects' models into STAGE_MODEL_BUILD_AREA of system/areas.h. it gives the
 * hunt the stage's hints and the paths of its moving pieces. it fills each of
 * player_hooks' drop_rings, add_item_icon, create_piece and show_piece_sign
 * that is null. it sets stage_hooks. it reads security_fog and
 * security_alt_fog from their files, and leaves Ninja's fog off */
void security_hall_init(void);
/* end the stage, as stg08_head gives overlay_run_exit. it frees the hint
 * file, the hints, the messages and the set file, and releases the textures.
 * it stops the set manager, the fog, the particles and the land. it resets
 * the hunt pieces' look and paths. it leaves Ninja's fog off. it takes the
 * stage's motions out of the motion table. it clears each of those
 * player_hooks that still holds the stage's call. it leaves stage_hooks set,
 * and doesn't destroy BgExec */
void security_hall_exit(void);
/* the stage task's step. it starts the fog where it isn't running. while the
 * work's mode is 0, it opens the safe area whose switch a player turns on. in
 * MISSION_FIRST, while no mini event runs, it plays the stage's voices by the
 * clock. a player in slot 0 or 1 falls out of the stage below a height of
 * -1100. one that has missed doesn't. players fall only while kill_areas_task
 * lives and has no child, so no player falls while a miss runs. task needs a
 * work */
void security_hall_step(struct task *task);
/* the stage's object list, which security_hall_init gives to set_start */
extern struct object_list security_hall_objects;

/* SAFE_AREAS counts the safe areas, A, B and C, numbered from 0.
 * SAFE_AREA_SWITCH is the id of an area's switch. the stage's step opens an
 * area once a player turns its switch on. it keeps only the open area's switch
 * on */
#define SAFE_AREAS 3
#define SAFE_AREA_SWITCH(area) ((area) + 1)
/* the safe area that is open */
extern Sint32 security_open_area;
/* a safe's or a panel's areas, from its placement's ang.x. the high nibble,
 * mod SAFE_AREAS, is its area, which picks its models. the low nibble, mod
 * SAFE_AREAS, counts the areas from that one on to SAFE_TEXTURE's. the models
 * take that area's textures */
#define SAFE_AREA_BITS 0xf0
#define SAFE_AREA_SHIFT 4
#define SAFE_AREAS_ON_BITS 0x0f
#define SAFE_AREA(work)                                                        \
    ((((work)->ang.x & SAFE_AREA_BITS) >> SAFE_AREA_SHIFT) % SAFE_AREAS)
#define SAFE_AREAS_ON(work) (((work)->ang.x & SAFE_AREAS_ON_BITS) % SAFE_AREAS)
#define SAFE_TEXTURE(work)                                                     \
    ((SAFE_AREAS_ON(work) + SAFE_AREA(work)) % SAFE_AREAS)

/* a vector, or its three Floats as an array. some of the objects' sums walk
 * the array a Float at a time */
union vector_floats {
    NJS_VECTOR vector;
    Float component[3];
};

#endif

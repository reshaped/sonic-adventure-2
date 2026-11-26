/* King Boom Boo's stage, the program of BOSS_BIGBOGY.PRS. the overlay's head
 * names its init, its exit and its task's step */
#ifndef BOSS_BIGBOGY_STAGE_H
#define BOSS_BIGBOGY_STAGE_H
#include <ninja.h>

#include "object/set.h"
#include "sound/lists.h"

struct task;

/* the textures bigbogy_stage_init loads: the fire's, the boss's model's and
 * the title's */
extern NJS_TEXLIST bigbogy_effect_texlist, bigbogy_model_texlist;
extern NJS_TEXLIST bigbogy_title_texlist;

/* the stage's object list, which bigbogy_stage_init gives to set_start */
extern struct object_list bigbogy_object_list;
extern const struct sound_list bigbogy_sounds;
/* the arena's center, on its floor */
extern const NJS_POINT3 bigbogy_arena_center;
/* the radius of the boss's circle about the center */
extern const Float bigbogy_circle_radius;
/* the stage holds player 0 within BIGBOGY_ARENA_RADIUS of the center in x and
 * z */
#define BIGBOGY_ARENA_RADIUS 260.0F
/* the lights the boss's body draws in. it flashes from BIGBOGY_BODY_LIGHT to
 * BIGBOGY_FLASH_LIGHT as he recoils on the ground */
#define BIGBOGY_BODY_LIGHT 3
#define BIGBOGY_FLASH_LIGHT 4

/* start the stage, and compile the land's models. it gives
 * player_hooks.drop_rings ring_scatter and player_hooks.unk_04 dirt_create,
 * each where it is null. it sets stage_hooks */
void bigbogy_stage_init(void);
/* end the stage. it releases the textures, the set file and the messages. it
 * stops the particles, the set manager and the land. it clears each player
 * hook that still holds what bigbogy_stage_init gave it */
void bigbogy_stage_exit(void);
/* the stage task's step. the first step creates the boss on his circle, at
 * the center less bigbogy_circle_radius in x. later steps hold player 0
 * within BIGBOGY_ARENA_RADIUS */
void bigbogy_stage_step(struct task *task);

#endif

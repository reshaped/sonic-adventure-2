/* Final Chase's meteors: FIREBALL, METEO_MOVE, METEO_MOVE_F and METEO_SET in
 * its object list, and those the background throws */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "shared/meteor_art.h"
#include "stg40/meteor.h"

#define ROCKS 4
#define ROCK(work) ((Uint32)(work)->scl.x % ROCKS)
/* a meteor's sphere takes ROCK_SHAPE_SCALE of its rock's radius, and rocks 0
 * and 3 take ROCK_SHAPE_MARGIN more */
#define ROCK_SHAPE_SCALE 0.7F
#define ROCK_SHAPE_MARGIN 10.0F

/* a fireball's timer starts once a player comes within scl.x plus
 * BREAK_NEAR. the fireball breaks once the timer passes BREAK_WAIT */
#define BREAK_WAIT 60
#define BREAK_NEAR 30.0F
#define BREAK_SOUND 12
#define BREAK_SOUND_VOLUME 60
/* the break throws PIECES pieces, the first SHARDS of them shards. each
 * piece's speed in y gains PIECE_LIFT after the speed's turn */
#define PIECES 20
#define SHARDS 5
#define PIECE_FLOOR (-100000.0F)
#define PIECE_LIFT 0.5F
/* the chance that a step sheds a spark: SPARK_CHANCE for a fireball, and
 * FALL_SPARK_CHANCE for a falling meteor */
#define SPARK_CHANCE 0.2F
#define FALL_SPARK_CHANCE 0.8F

/* while its timer runs, the fireball blinks from BLINK_COLOR toward
 * BLINK_RED. the blink turns BLINK_RATE Angle units a step of the timer */
#define BLINK_RATE 0x500
#define BLINK_COLOR 0xffffffff
#define BLINK_RED 0xffff2020

#include "stg40/meteor/text.inc"

static NJS_TEXNAME meteor_0_textures[1] = {
#include "stg40/meteor/meteor_0_textures.inc"
};

NJS_TEXLIST meteor_0_texlist = {
#include "stg40/meteor/meteor_0_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_0.h"

NJS_CNK_OBJECT meteor_0_object = {
#include "stg40/meteor/meteor_0_object.inc"
};

static NJS_TEXNAME meteor_1_textures[1] = {
#include "stg40/meteor/meteor_1_textures.inc"
};

NJS_TEXLIST meteor_1_texlist = {
#include "stg40/meteor/meteor_1_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_1.h"

NJS_CNK_OBJECT meteor_1_object = {
#include "stg40/meteor/meteor_1_object.inc"
};

static NJS_TEXNAME meteor_2_textures[2] = {
#include "stg40/meteor/meteor_2_textures.inc"
};

NJS_TEXLIST meteor_2_texlist = {
#include "stg40/meteor/meteor_2_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_2.h"

NJS_CNK_OBJECT meteor_2_object = {
#include "stg40/meteor/meteor_2_object.inc"
};

static NJS_TEXNAME meteor_3_textures[2] = {
#include "stg40/meteor/meteor_3_textures.inc"
};

NJS_TEXLIST meteor_3_texlist = {
#include "stg40/meteor/meteor_3_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_3.h"

NJS_CNK_OBJECT meteor_3_object = {
#include "stg40/meteor/meteor_3_object.inc"
};

static NJS_TEXNAME meteor_fall_textures[1] = {
#include "stg40/meteor/meteor_fall_textures.inc"
};

NJS_TEXLIST meteor_fall_texlist = {
#include "stg40/meteor/meteor_fall_texlist.inc"
};

#include "shared/meteor_shapes/meteor_falling.h"

NJS_CNK_OBJECT meteor_fall_object[1] = {
#include "stg40/meteor/meteor_fall_object.inc"
};

static NJS_TEXNAME final_chase_fireball_textures[1] = {
#include "stg40/meteor/final_chase_fireball_textures.inc"
};

static NJS_TEXLIST final_chase_fireball_texlist = {
#include "stg40/meteor/final_chase_fireball_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fireball.h"

static NJS_CNK_OBJECT final_chase_fireball_object[1] = {
#include "stg40/meteor/final_chase_fireball_object.inc"
};

static NJS_TEXNAME fireball_piece_textures[2] = {
#include "stg40/meteor/fireball_piece_textures.inc"
};

static NJS_TEXLIST fireball_piece_texlist = {
#include "stg40/meteor/fireball_piece_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fragment.h"

static NJS_CNK_OBJECT fireball_piece_object[1] = {
#include "stg40/meteor/fireball_piece_object.inc"
};

static NJS_TEXNAME fireball_shard_textures[2] = {
#include "stg40/meteor/fireball_shard_textures.inc"
};

static NJS_TEXLIST fireball_shard_texlist = {
#include "stg40/meteor/fireball_shard_texlist.inc"
};

#include "shared/meteor_shapes/meteor_shard.h"

static NJS_CNK_OBJECT fireball_shard_object[1] = {
#include "stg40/meteor/fireball_shard_object.inc"
};

/* the rocks' lists come first, in rock order: meteor_draw indexes the array
 * by rock */
NJS_TEXLIST *meteor_texlists[9] = {
#include "stg40/meteor/meteor_texlists.inc"
};
static NJS_CNK_OBJECT *meteor_objects[ROCKS] = {
#include "stg40/meteor/meteor_objects.inc"
};
/* the fireball's sphere, which hurts players. meteor_set_init gives it to its
 * meteor too, at the rock's radius */
/* clang-format off */
static struct collision_shape fireball_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |
        SHAPE_HURTS_PLAYERS,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 32.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Uint32 dat_8c5ad8e8 = 0xff0070ff;
/* a fireball's sparks: their color, and their size, fireball_spark_size and
 * up to fireball_spark_size_range more */
static Uint32 fireball_spark_color = 0xff3020ff;
static Float fireball_spark_size = 20.0F;
static Float fireball_spark_size_range = 30.0F;
/* a fireball's and a falling meteor's spin about x and about y, in degrees a
 * step */
static Float spin_x = 0.36F;
static Float spin_y = 0.45000002F;
/* a falling meteor's sparks are fall_spark_color through the first half of
 * the meteor's life. they blend to fall_spark_end_color over the second
 * half */
static Float fall_spark_size = 38.0F;
static Float fall_spark_size_range = 70.0F;
static Uint32 fall_spark_color = 0xffff7000;
static Uint32 fall_spark_end_color = 0xffff2030;
/* a moving meteor's sphere, whose radius its init sets by the rock */
/* clang-format off */
static struct collision_shape meteor_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
    0, {0.0F, 0.0F, 0.0F}, 35.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* a moving meteor's spin about x and about y, in degrees a step. its scl.y
 * and scl.z add to them */
static Float moving_spin_x = 0.36F;
static Float moving_spin_y = 0.45000002F;

#define FIREBALL_INIT fireball_init
#define FIREBALL_TEXLIST final_chase_fireball_texlist
#define FIREBALL_OBJECT final_chase_fireball_object
#define FIREBALL_SHARD_TEXLIST fireball_shard_texlist
#define FIREBALL_SHARD_OBJECT fireball_shard_object
#define FIREBALL_PIECE_TEXLIST fireball_piece_texlist
#define FIREBALL_PIECE_OBJECT fireball_piece_object
#include "shared/breaking_fireball.h" /* IWYU pragma: keep -- the code it compiles here */

#define ROCK_SPIN_X moving_spin_x
#define ROCK_SPIN_Y moving_spin_y
#define STILL_METEOR_SHAPE fireball_shape
#include "shared/falling_meteor.h" /* IWYU pragma: keep -- the code it compiles here */
#include "shared/meteor_rocks.h" /* IWYU pragma: keep -- the code it compiles here */

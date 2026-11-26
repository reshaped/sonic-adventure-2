/* the shot's art, then MUJINGUN's texture list, its model and the model's
 * scroll. no code reads barrel_object */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/cannon_parts.h"
#include "stg36/cannon.h"
#include "stg36/cannon_art/text.inc"

/* MUJINGUN loads two palettes for the same shell and its four sparks. */

static NJS_TEXNAME cannon_shot_bright_names[2] = {
#include "stg36/cannon_art/cannon_shot_bright_names.inc"
};

NJS_TEXLIST cannon_shot_bright = {
#include "stg36/cannon_art/cannon_shot_bright.inc"
};
static NJS_TEXNAME cannon_shot_dim_names[2] = {
#include "stg36/cannon_art/cannon_shot_dim_names.inc"
};

NJS_TEXLIST cannon_shot_dim = {
#include "stg36/cannon_art/cannon_shot_dim.inc"
};

static Sint16 shot_spark_down_polygons[29] = {
#include "stg36/cannon_art/shot_spark_down_polygons.inc"
};

static Sint32 shot_spark_down_vertices[19] = {
#include "stg36/cannon_art/shot_spark_down_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_down_model = {
#include "stg36/cannon_art/shot_spark_down_model.inc"
};

static NJS_CNK_OBJECT shot_spark_down = {
#include "stg36/cannon_art/shot_spark_down.inc"
};

static Sint16 shot_spark_back_polygons[29] = {
#include "stg36/cannon_art/shot_spark_back_polygons.inc"
};

static Sint32 shot_spark_back_vertices[19] = {
#include "stg36/cannon_art/shot_spark_back_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_back_model = {
#include "stg36/cannon_art/shot_spark_back_model.inc"
};

static NJS_CNK_OBJECT shot_spark_back = {
#include "stg36/cannon_art/shot_spark_back.inc"
};

static Sint16 shot_spark_front_polygons[29] = {
#include "stg36/cannon_art/shot_spark_front_polygons.inc"
};

static Sint32 shot_spark_front_vertices[19] = {
#include "stg36/cannon_art/shot_spark_front_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_front_model = {
#include "stg36/cannon_art/shot_spark_front_model.inc"
};

static NJS_CNK_OBJECT shot_spark_front = {
#include "stg36/cannon_art/shot_spark_front.inc"
};

static Sint16 shot_spark_up_polygons[29] = {
#include "stg36/cannon_art/shot_spark_up_polygons.inc"
};

static Sint32 shot_spark_up_vertices[19] = {
#include "stg36/cannon_art/shot_spark_up_vertices.inc"
};

static NJS_CNK_MODEL shot_spark_up_model = {
#include "stg36/cannon_art/shot_spark_up_model.inc"
};

static NJS_CNK_OBJECT shot_spark_up = {
#include "stg36/cannon_art/shot_spark_up.inc"
};

static Sint16 cannon_shot_model_polygons[57] = {
#include "stg36/cannon_art/cannon_shot_model_polygons.inc"
};

static Sint32 cannon_shot_model_vertices[27] = {
#include "stg36/cannon_art/cannon_shot_model_vertices.inc"
};

static NJS_CNK_MODEL shot_center_model = {
#include "stg36/cannon_art/shot_center_model.inc"
};

NJS_CNK_OBJECT cannon_shot_model = {
#include "stg36/cannon_art/cannon_shot_model.inc"
};
static NJS_TEXNAME barrel_names[5] = {
#include "stg36/cannon_art/barrel_names.inc"
};

NJS_TEXLIST cannon_texlist = {
#include "stg36/cannon_art/cannon_texlist.inc"
};

static Sint16 barrel_polygons[481] = {
#include "stg36/cannon_art/barrel_polygons.inc"
};

static Sint32 barrel_vertices[359] = {
#include "stg36/cannon_art/barrel_vertices.inc"
};

NJS_CNK_MODEL cannon_barrel = {
#include "stg36/cannon_art/cannon_barrel.inc"
};

static NJS_CNK_OBJECT barrel_object = {
#include "stg36/cannon_art/barrel_object.inc"
};

/* the barrel's uvs, as texture_anim.h lays them out: one entry of 18 vertices,
 * in two keys. each u of the second key is 32 past the first key's */
static Sint16 barrel_uv_keys[76] = {
#include "stg36/cannon_art/barrel_uv_keys.inc"
};

struct texture_anim cannon_barrel_anim = {
#include "stg36/cannon_art/cannon_barrel_anim.inc"
};

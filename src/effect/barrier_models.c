/* The barrier and invincibility drawers select their own textures and draw
 * these four models directly. Each model also has a root object and a
 * one-name texture list in this member. */
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/barrier.h"

#include "effect/barrier_models/text.inc"

static NJS_TEXNAME barrier_spark_textures[1] = {
#include "effect/barrier_models/barrier_spark_textures.inc"
};

static NJS_TEXLIST barrier_spark_texlist = {
#include "effect/barrier_models/barrier_spark_texlist.inc"
};

static Sint16 barrier_spark_polygons[165] = {
#include "effect/barrier_models/barrier_spark_polygons.inc"
};

static Sint32 barrier_spark_vertices[103] = {
#include "effect/barrier_models/barrier_spark_vertices.inc"
};

NJS_CNK_MODEL barrier_spark_model = {
#include "effect/barrier_models/barrier_spark_model.inc"
};

static NJS_CNK_OBJECT barrier_spark_object = {
#include "effect/barrier_models/barrier_spark_object.inc"
};

static NJS_TEXNAME magnetic_spark_textures[1] = {
#include "effect/barrier_models/magnetic_spark_textures.inc"
};

static NJS_TEXLIST magnetic_spark_texlist = {
#include "effect/barrier_models/magnetic_spark_texlist.inc"
};

static Sint16 magnetic_spark_polygons[125] = {
#include "effect/barrier_models/magnetic_spark_polygons.inc"
};

static Sint32 magnetic_spark_vertices[105] = {
#include "effect/barrier_models/magnetic_spark_vertices.inc"
};

NJS_CNK_MODEL magnetic_spark_model = {
#include "effect/barrier_models/magnetic_spark_model.inc"
};

static NJS_CNK_OBJECT magnetic_spark_object = {
#include "effect/barrier_models/magnetic_spark_object.inc"
};

static NJS_TEXNAME invincibility_ball_textures[1] = {
#include "effect/barrier_models/invincibility_ball_textures.inc"
};

static NJS_TEXLIST invincibility_ball_texlist = {
#include "effect/barrier_models/invincibility_ball_texlist.inc"
};

static Sint16 invincibility_ball_polygons[161] = {
#include "effect/barrier_models/invincibility_ball_polygons.inc"
};

static Sint32 invincibility_ball_vertices[351] = {
#include "effect/barrier_models/invincibility_ball_vertices.inc"
};

NJS_CNK_MODEL invincibility_ball_model = {
#include "effect/barrier_models/invincibility_ball_model.inc"
};

static NJS_CNK_OBJECT invincibility_ball_object = {
#include "effect/barrier_models/invincibility_ball_object.inc"
};

static NJS_TEXNAME invincibility_spark_textures[1] = {
#include "effect/barrier_models/invincibility_spark_textures.inc"
};

static NJS_TEXLIST invincibility_spark_texlist = {
#include "effect/barrier_models/invincibility_spark_texlist.inc"
};

static Sint16 invincibility_spark_polygons[23] = {
#include "effect/barrier_models/invincibility_spark_polygons.inc"
};

static Sint32 invincibility_spark_vertices[19] = {
#include "effect/barrier_models/invincibility_spark_vertices.inc"
};

NJS_CNK_MODEL invincibility_spark_model = {
#include "effect/barrier_models/invincibility_spark_model.inc"
};

static NJS_CNK_OBJECT invincibility_spark_object = {
#include "effect/barrier_models/invincibility_spark_object.inc"
};

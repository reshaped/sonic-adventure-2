/* STG23.PRS's texture lists, chunk models, Basic models and texture names,
 * from the disc. its C lies between swing.c and scenery_textures.c, and its
 * D lies between lift.c and swing.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg23/platforms.h"

#include "stg23/platforms/text.inc"

static NJS_TEXNAME textures[1] = {
#include "stg23/platforms/textures.inc"
};
NJS_TEXLIST base_swing_texlist = {
#include "stg23/platforms/base_swing_texlist.inc"
};
static Sint16 swing_polygons[99] = {
#include "stg23/platforms/swing_polygons.inc"
};
static Sint32 swing_vertices[147] = {
#include "stg23/platforms/swing_vertices.inc"
};
NJS_CNK_MODEL swing_model = {
#include "stg23/platforms/swing_model.inc"
};
static NJS_CNK_OBJECT swing_object = {
#include "stg23/platforms/swing_object.inc"
};
static NJS_MATERIAL swing_collision_materials[1] = {
#include "stg23/platforms/swing_collision_materials.inc"
};
static Sint16 swing_collision_faces[21] = {
#include "stg23/platforms/swing_collision_faces.inc"
};
static NJS_MESHSET swing_collision_meshsets[1] = {
#include "stg23/platforms/swing_collision_meshsets.inc"
};
static NJS_POINT3 swing_collision_points[8] = {
#include "stg23/platforms/swing_collision_points.inc"
};
static NJS_POINT3 swing_collision_normals[8] = {
#include "stg23/platforms/swing_collision_normals.inc"
};
static NJS_MODEL swing_collision_model = {
#include "stg23/platforms/swing_collision_model.inc"
};
NJS_OBJECT swing_collision = {
#include "stg23/platforms/swing_collision.inc"
};

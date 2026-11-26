/* STG43.PRS's texture lists, chunk models, Basic models, floats, texture
 * list pointers, collision shapes, chunk object pointers, pointer slot,
 * words and texture names, from the disc. its C lies between drill.c and
 * land_model_change.c, and its D lies between
 * player_motions_upgrade_sand_ocean.c and land_draw.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "stg43/drill_assets.h"

#include "stg43/drill_assets/text.inc"

static NJS_TEXNAME platform_textures[14] = {
#include "stg43/drill_assets/platform_textures.inc"
};
NJS_TEXLIST platform_texlist = {
#include "stg43/drill_assets/platform_texlist.inc"
};
static Sint16 platform_left_clamp_polygons[347] = {
#include "stg43/drill_assets/platform_left_clamp_polygons.inc"
};
static Sint32 platform_left_clamp_vertices[331] = {
#include "stg43/drill_assets/platform_left_clamp_vertices.inc"
};
static NJS_CNK_MODEL platform_left_clamp_model = {
#include "stg43/drill_assets/platform_left_clamp_model.inc"
};
NJS_CNK_OBJECT platform_left_clamp = {
#include "stg43/drill_assets/platform_left_clamp.inc"
};
static Sint16 platform_light_polygons[379] = {
#include "stg43/drill_assets/platform_light_polygons.inc"
};
static Sint32 platform_light_vertices[219] = {
#include "stg43/drill_assets/platform_light_vertices.inc"
};
static NJS_CNK_MODEL platform_light_model = {
#include "stg43/drill_assets/platform_light_model.inc"
};
NJS_CNK_OBJECT platform_light[1] = {
#include "stg43/drill_assets/platform_light.inc"
};
static Sint16 platform_right_clamp_polygons[347] = {
#include "stg43/drill_assets/platform_right_clamp_polygons.inc"
};
static Sint32 platform_right_clamp_vertices[331] = {
#include "stg43/drill_assets/platform_right_clamp_vertices.inc"
};
static NJS_CNK_MODEL platform_right_clamp_model = {
#include "stg43/drill_assets/platform_right_clamp_model.inc"
};
NJS_CNK_OBJECT platform_right_clamp = {
#include "stg43/drill_assets/platform_right_clamp.inc"
};
static Sint16 platform_body_polygons[941] = {
#include "stg43/drill_assets/platform_body_polygons.inc"
};
static Sint32 platform_body_vertices[755] = {
#include "stg43/drill_assets/platform_body_vertices.inc"
};
static NJS_CNK_MODEL platform_body_model = {
#include "stg43/drill_assets/platform_body_model.inc"
};
NJS_CNK_OBJECT platform_body = {
#include "stg43/drill_assets/platform_body.inc"
};
static NJS_MATERIAL platform_collision_material[1] = {
#include "stg43/drill_assets/platform_collision_material.inc"
};
static Sint16 platform_collision_faces_0[5] = {
#include "stg43/drill_assets/platform_collision_faces_0.inc"
};
static NJS_MESHSET platform_collision_meshes[1] = {
#include "stg43/drill_assets/platform_collision_meshes.inc"
};
static NJS_POINT3 platform_collision_points[4] = {
#include "stg43/drill_assets/platform_collision_points.inc"
};
static NJS_POINT3 platform_collision_normals[4] = {
#include "stg43/drill_assets/platform_collision_normals.inc"
};
static NJS_MODEL platform_collision_model = {
#include "stg43/drill_assets/platform_collision_model.inc"
};
NJS_OBJECT platform_collision = {
#include "stg43/drill_assets/platform_collision.inc"
};
static NJS_TEXNAME drill_body_textures[2] = {
#include "stg43/drill_assets/drill_body_textures.inc"
};
NJS_TEXLIST drill_body_texlist = {
#include "stg43/drill_assets/drill_body_texlist.inc"
};
static Sint16 drill_body_polygons[447] = {
#include "stg43/drill_assets/drill_body_polygons.inc"
};
static Sint32 drill_body_vertices[355] = {
#include "stg43/drill_assets/drill_body_vertices.inc"
};
static NJS_CNK_MODEL drill_body_model = {
#include "stg43/drill_assets/drill_body_model.inc"
};
NJS_CNK_OBJECT drill_body[1] = {
#include "stg43/drill_assets/drill_body.inc"
};
static NJS_TEXNAME drill_head_textures[4] = {
#include "stg43/drill_assets/drill_head_textures.inc"
};
NJS_TEXLIST drill_head_texlist = {
#include "stg43/drill_assets/drill_head_texlist.inc"
};
static Sint16 drill_tooth_5_polygons[403] = {
#include "stg43/drill_assets/drill_tooth_5_polygons.inc"
};
static Sint32 drill_tooth_5_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_5_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_5_model = {
#include "stg43/drill_assets/drill_tooth_5_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_5 = {
#include "stg43/drill_assets/drill_tooth_5.inc"
};
static Sint16 drill_tooth_4_polygons[403] = {
#include "stg43/drill_assets/drill_tooth_4_polygons.inc"
};
static Sint32 drill_tooth_4_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_4_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_4_model = {
#include "stg43/drill_assets/drill_tooth_4_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_4 = {
#include "stg43/drill_assets/drill_tooth_4.inc"
};
static Sint16 drill_tooth_3_polygons[401] = {
#include "stg43/drill_assets/drill_tooth_3_polygons.inc"
};
static Sint32 drill_tooth_3_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_3_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_3_model = {
#include "stg43/drill_assets/drill_tooth_3_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_3 = {
#include "stg43/drill_assets/drill_tooth_3.inc"
};
static Sint16 drill_tooth_2_polygons[401] = {
#include "stg43/drill_assets/drill_tooth_2_polygons.inc"
};
static Sint32 drill_tooth_2_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_2_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_2_model = {
#include "stg43/drill_assets/drill_tooth_2_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_2 = {
#include "stg43/drill_assets/drill_tooth_2.inc"
};
static Sint16 drill_tooth_1_polygons[401] = {
#include "stg43/drill_assets/drill_tooth_1_polygons.inc"
};
static Sint32 drill_tooth_1_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_1_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_1_model = {
#include "stg43/drill_assets/drill_tooth_1_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_1 = {
#include "stg43/drill_assets/drill_tooth_1.inc"
};
static Sint16 drill_head_polygons[609] = {
#include "stg43/drill_assets/drill_head_polygons.inc"
};
static Sint32 drill_head_vertices[321] = {
#include "stg43/drill_assets/drill_head_vertices.inc"
};
static NJS_CNK_MODEL drill_head_model = {
#include "stg43/drill_assets/drill_head_model.inc"
};
NJS_CNK_OBJECT drill_head[1] = {
#include "stg43/drill_assets/drill_head.inc"
};
static Sint16 drill_tooth_0_polygons[403] = {
#include "stg43/drill_assets/drill_tooth_0_polygons.inc"
};
static Sint32 drill_tooth_0_vertices[165] = {
#include "stg43/drill_assets/drill_tooth_0_vertices.inc"
};
static NJS_CNK_MODEL drill_tooth_0_model = {
#include "stg43/drill_assets/drill_tooth_0_model.inc"
};
static NJS_CNK_OBJECT drill_tooth_0 = {
#include "stg43/drill_assets/drill_tooth_0.inc"
};
static NJS_CNK_OBJECT drill_root = {
#include "stg43/drill_assets/drill_root.inc"
};
Float platform_slope_gain = {
#include "stg43/drill_assets/platform_slope_gain.inc"
};
Float platform_drag = {
#include "stg43/drill_assets/platform_drag.inc"
};
Float platform_drive = {
#include "stg43/drill_assets/platform_drive.inc"
};
Float drill_drag = {
#include "stg43/drill_assets/drill_drag.inc"
};
Float drill_drive = {
#include "stg43/drill_assets/drill_drive.inc"
};
NJS_POINT3 rider_seat = {
#include "stg43/drill_assets/rider_seat.inc"
};
NJS_TEXLIST *cosmic_drill_texlists[4] = {
#include "stg43/drill_assets/cosmic_drill_texlists.inc"
};
static struct collision_shape unused_shape[1] = {
#include "stg43/drill_assets/unused_shape.inc"
};
struct collision_shape tooth_shape[1] = {
#include "stg43/drill_assets/tooth_shape.inc"
};
NJS_CNK_OBJECT *drill_teeth[DRILL_TEETH] = {
#include "stg43/drill_assets/drill_teeth.inc"
};
struct task *drill_triggers = {
#include "stg43/drill_assets/drill_triggers.inc"
};
Uint32 drill_light_on = {
#include "stg43/drill_assets/drill_light_on.inc"
};
Uint32 drill_light_off = {
#include "stg43/drill_assets/drill_light_off.inc"
};
NJS_VECTOR launch_velocity = {
#include "stg43/drill_assets/launch_velocity.inc"
};
NJS_POINT3 launch_target = {
#include "stg43/drill_assets/launch_target.inc"
};
struct collision_shape drill_shape = {
#include "stg43/drill_assets/drill_shape.inc"
};
Float drill_near_drag = {
#include "stg43/drill_assets/drill_near_drag.inc"
};

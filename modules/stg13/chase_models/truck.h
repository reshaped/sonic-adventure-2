/* City Escape pursuit path, truck parts, lamps and scrolling surfaces. */
#ifndef STG13_CHASE_MODELS_TRUCK_H
#define STG13_CHASE_MODELS_TRUCK_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stage/path.h"

static NJS_POINT3 truck_path_points[88] = {
#include "stg13/chase_models/truck/truck_path_points.inc"
};

static struct path truck_path = {
#include "stg13/chase_models/truck/truck_path.inc"
};

static NJS_TEXNAME truck_texture_names[11] = {
#include "stg13/chase_models/truck/truck_texture_names.inc"
};

static NJS_TEXLIST truck_texlist = {
#include "stg13/chase_models/truck/truck_texlist.inc"
};

static NJS_TEXNAME truck_unlit_texture_names[11] = {
#include "stg13/chase_models/truck/truck_unlit_texture_names.inc"
};

static NJS_TEXLIST truck_unlit_texlist = {
#include "stg13/chase_models/truck/truck_unlit_texlist.inc"
};

static Sint16 truck_front_wheel_2_polygons[111] = {
#include "stg13/chase_models/truck/truck_front_wheel_2_polygons.inc"
};

static Sint32 truck_front_wheel_2_vertices[171] = {
#include "stg13/chase_models/truck/truck_front_wheel_2_vertices.inc"
};

static NJS_CNK_MODEL truck_front_wheel_2_model = {
#include "stg13/chase_models/truck/truck_front_wheel_2_model.inc"
};

static NJS_CNK_OBJECT truck_front_wheel_2 = {
#include "stg13/chase_models/truck/truck_front_wheel_2.inc"
};

static Sint16 truck_front_wheel_1_polygons[111] = {
#include "stg13/chase_models/truck/truck_front_wheel_1_polygons.inc"
};

static Sint32 truck_front_wheel_1_vertices[171] = {
#include "stg13/chase_models/truck/truck_front_wheel_1_vertices.inc"
};

static NJS_CNK_MODEL truck_front_wheel_1_model = {
#include "stg13/chase_models/truck/truck_front_wheel_1_model.inc"
};

static NJS_CNK_OBJECT truck_front_wheel_1 = {
#include "stg13/chase_models/truck/truck_front_wheel_1.inc"
};

static Sint16 truck_front_polygons[801] = {
#include "stg13/chase_models/truck/truck_front_polygons.inc"
};

static Sint32 truck_front_vertices[519] = {
#include "stg13/chase_models/truck/truck_front_vertices.inc"
};

static NJS_CNK_MODEL truck_front_model = {
#include "stg13/chase_models/truck/truck_front_model.inc"
};

static NJS_CNK_OBJECT truck_front_object = {
#include "stg13/chase_models/truck/truck_front_object.inc"
};

static Sint16 truck_rear_wheel_2_polygons[85] = {
#include "stg13/chase_models/truck/truck_rear_wheel_2_polygons.inc"
};

static Sint32 truck_rear_wheel_2_vertices[123] = {
#include "stg13/chase_models/truck/truck_rear_wheel_2_vertices.inc"
};

static NJS_CNK_MODEL truck_rear_wheel_2_model = {
#include "stg13/chase_models/truck/truck_rear_wheel_2_model.inc"
};

static NJS_CNK_OBJECT truck_rear_wheel_2 = {
#include "stg13/chase_models/truck/truck_rear_wheel_2.inc"
};

static Sint16 truck_back_polygons[165] = {
#include "stg13/chase_models/truck/truck_back_polygons.inc"
};

static Sint32 truck_back_vertices[219] = {
#include "stg13/chase_models/truck/truck_back_vertices.inc"
};

static NJS_CNK_MODEL truck_back_model = {
#include "stg13/chase_models/truck/truck_back_model.inc"
};

static NJS_CNK_OBJECT truck_back_object = {
#include "stg13/chase_models/truck/truck_back_object.inc"
};

static Sint16 truck_trailer_polygons[149] = {
#include "stg13/chase_models/truck/truck_trailer_polygons.inc"
};

static Sint32 truck_trailer_vertices[147] = {
#include "stg13/chase_models/truck/truck_trailer_vertices.inc"
};

static NJS_CNK_MODEL truck_trailer_model = {
#include "stg13/chase_models/truck/truck_trailer_model.inc"
};

static NJS_CNK_OBJECT truck_trailer_object = {
#include "stg13/chase_models/truck/truck_trailer_object.inc"
};

static Sint16 truck_rear_wheel_1_polygons[85] = {
#include "stg13/chase_models/truck/truck_rear_wheel_1_polygons.inc"
};

static Sint32 truck_rear_wheel_1_vertices[123] = {
#include "stg13/chase_models/truck/truck_rear_wheel_1_vertices.inc"
};

static NJS_CNK_MODEL truck_rear_wheel_1_model = {
#include "stg13/chase_models/truck/truck_rear_wheel_1_model.inc"
};

static NJS_CNK_OBJECT truck_rear_wheel_1 = {
#include "stg13/chase_models/truck/truck_rear_wheel_1.inc"
};

static Sint16 truck_polygons[939] = {
#include "stg13/chase_models/truck/truck_polygons.inc"
};

static Sint32 truck_vertices[735] = {
#include "stg13/chase_models/truck/truck_vertices.inc"
};

static NJS_CNK_MODEL truck_model = {
#include "stg13/chase_models/truck/truck_model.inc"
};

static NJS_CNK_OBJECT truck_object = {
#include "stg13/chase_models/truck/truck_object.inc"
};

static NJS_TEXNAME truck_roof_texture_names[2] = {
#include "stg13/chase_models/truck/truck_roof_texture_names.inc"
};

static NJS_TEXLIST truck_roof_texlist = {
#include "stg13/chase_models/truck/truck_roof_texlist.inc"
};

static Sint16 truck_roof_polygons[237] = {
#include "stg13/chase_models/truck/truck_roof_polygons.inc"
};

static Sint32 truck_roof_vertices[75] = {
#include "stg13/chase_models/truck/truck_roof_vertices.inc"
};

static NJS_CNK_MODEL truck_roof_model = {
#include "stg13/chase_models/truck/truck_roof_model.inc"
};

static NJS_CNK_OBJECT truck_roof_object = {
#include "stg13/chase_models/truck/truck_roof_object.inc"
};

/* Strip offsets are measured in halfwords from the polygon list. */
static Sint16 truck_roof_uvs[78] = {
#include "stg13/chase_models/truck/truck_roof_uvs.inc"
};

static struct texture_anim truck_roof_anim = {
#include "stg13/chase_models/truck/truck_roof_anim.inc"
};

static NJS_TEXNAME truck_scroll_texture_names_1[2] = {
#include "stg13/chase_models/truck/truck_scroll_texture_names_1.inc"
};

static NJS_TEXLIST truck_scroll_texlist_1 = {
#include "stg13/chase_models/truck/truck_scroll_texlist_1.inc"
};

static Sint16 truck_scroll_1_polygons[59] = {
#include "stg13/chase_models/truck/truck_scroll_1_polygons.inc"
};

static Sint32 truck_scroll_1_vertices[39] = {
#include "stg13/chase_models/truck/truck_scroll_1_vertices.inc"
};

static NJS_CNK_MODEL truck_scroll_1_model = {
#include "stg13/chase_models/truck/truck_scroll_1_model.inc"
};

static NJS_CNK_OBJECT truck_scroll_object_1[1] = {
#include "stg13/chase_models/truck/truck_scroll_object_1.inc"
};

/* Strip offsets are measured in halfwords from the polygon list. */
static Sint16 truck_scroll_uvs_1[16] = {
#include "stg13/chase_models/truck/truck_scroll_uvs_1.inc"
};

static struct texture_anim truck_scroll_anim_1 = {
#include "stg13/chase_models/truck/truck_scroll_anim_1.inc"
};

static NJS_TEXNAME truck_scroll_texture_names_2[2] = {
#include "stg13/chase_models/truck/truck_scroll_texture_names_2.inc"
};

static NJS_TEXLIST truck_scroll_texlist_2 = {
#include "stg13/chase_models/truck/truck_scroll_texlist_2.inc"
};

static Sint16 truck_scroll_2_polygons[47] = {
#include "stg13/chase_models/truck/truck_scroll_2_polygons.inc"
};

static Sint32 truck_scroll_2_vertices[27] = {
#include "stg13/chase_models/truck/truck_scroll_2_vertices.inc"
};

static NJS_CNK_MODEL truck_scroll_2_model = {
#include "stg13/chase_models/truck/truck_scroll_2_model.inc"
};

static NJS_CNK_OBJECT truck_scroll_object_2[1] = {
#include "stg13/chase_models/truck/truck_scroll_object_2.inc"
};

/* Strip offsets are measured in halfwords from the polygon list. */
static Sint16 truck_scroll_uvs_2[12] = {
#include "stg13/chase_models/truck/truck_scroll_uvs_2.inc"
};

static struct texture_anim truck_scroll_anim_2 = {
#include "stg13/chase_models/truck/truck_scroll_anim_2.inc"
};

static NJS_TEXNAME truck_scroll_texture_names_3[2] = {
#include "stg13/chase_models/truck/truck_scroll_texture_names_3.inc"
};

static NJS_TEXLIST truck_scroll_texlist_3 = {
#include "stg13/chase_models/truck/truck_scroll_texlist_3.inc"
};

static Sint16 truck_scroll_3_polygons[47] = {
#include "stg13/chase_models/truck/truck_scroll_3_polygons.inc"
};

static Sint32 truck_scroll_3_vertices[27] = {
#include "stg13/chase_models/truck/truck_scroll_3_vertices.inc"
};

static NJS_CNK_MODEL truck_scroll_3_model = {
#include "stg13/chase_models/truck/truck_scroll_3_model.inc"
};

static NJS_CNK_OBJECT truck_scroll_object_3[1] = {
#include "stg13/chase_models/truck/truck_scroll_object_3.inc"
};

/* Strip offsets are measured in halfwords from the polygon list. */
static Sint16 truck_scroll_uvs_3[12] = {
#include "stg13/chase_models/truck/truck_scroll_uvs_3.inc"
};

static struct texture_anim truck_scroll_anim_3 = {
#include "stg13/chase_models/truck/truck_scroll_anim_3.inc"
};

static NJS_TEXNAME lamp_texture_names[2] = {
#include "stg13/chase_models/truck/lamp_texture_names.inc"
};

static NJS_TEXLIST lamp_texlist = {
#include "stg13/chase_models/truck/lamp_texlist.inc"
};

#endif

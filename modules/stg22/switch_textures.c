#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg22/fixture_models.h"
#include "stg22/gravity_switch.h"
#include "stg22/switch_textures/text.inc"

/* the gravity switch's texture list and its models */

/* the set editor's labels for ang.x, ang.y and ang.z, which no code reads */
static const char pitch_caption[12] = "<- ANG X";
static const char yaw_caption[12] = "<- ANG Y";
static const char roll_caption[12] = "<- ANG Z";

static NJS_TEXNAME gadget_gravity_switch_texlist_names[10] = {
#include "stg22/switch_textures/gadget_gravity_switch_texlist_names.inc"
};
NJS_TEXLIST gadget_gravity_switch_texlist = {
#include "stg22/switch_textures/gadget_gravity_switch_texlist.inc"
};

/* Switch arrow, scrolling face and the solid chute. */

static Sint16 gadget_switch_arrow_node_1_polygons[115] = {
#include "stg22/switch_textures/gadget_switch_arrow_node_1_polygons.inc"
};

static Sint32 gadget_switch_arrow_node_1_vertices[123] = {
#include "stg22/switch_textures/gadget_switch_arrow_node_1_vertices.inc"
};

static NJS_CNK_MODEL gadget_switch_arrow_node_1_mesh = {
#include "stg22/switch_textures/gadget_switch_arrow_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gadget_switch_arrow_node_1 = {
#include "stg22/switch_textures/gadget_switch_arrow_node_1.inc"
};

static Sint16 gadget_switch_arrow_polygons[25] = {
#include "stg22/switch_textures/gadget_switch_arrow_polygons.inc"
};

static Sint32 gadget_switch_arrow_vertices[19] = {
#include "stg22/switch_textures/gadget_switch_arrow_vertices.inc"
};

static NJS_CNK_MODEL gadget_switch_arrow_mesh = {
#include "stg22/switch_textures/gadget_switch_arrow_mesh.inc"
};

NJS_CNK_OBJECT gadget_switch_arrow[1] = {
#include "stg22/switch_textures/gadget_switch_arrow.inc"
};

static Sint16 gadget_switch_body_polygons[945] = {
#include "stg22/switch_textures/gadget_switch_body_polygons.inc"
};

static Sint32 gadget_switch_body_vertices[787] = {
#include "stg22/switch_textures/gadget_switch_body_vertices.inc"
};

static NJS_CNK_MODEL gadget_switch_body_mesh = {
#include "stg22/switch_textures/gadget_switch_body_mesh.inc"
};

NJS_CNK_OBJECT gadget_switch_body = {
#include "stg22/switch_textures/gadget_switch_body.inc"
};

static Sint16 gadget_switch_uvs[12] = {
#include "stg22/switch_textures/gadget_switch_uvs.inc"
};

struct texture_anim gadget_switch_scroll = {
#include "stg22/switch_textures/gadget_switch_scroll.inc"
};

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL gadget_switch_solid_materials[1] = {
#include "stg22/switch_textures/gadget_switch_solid_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 gadget_switch_solid_indices_0[12] = {
#include "stg22/switch_textures/gadget_switch_solid_indices_0.inc"
};

static NJS_MESHSET gadget_switch_solid_meshsets[1] = {
#include "stg22/switch_textures/gadget_switch_solid_meshsets.inc"
};

static NJS_POINT3 gadget_switch_solid_points[6] = {
#include "stg22/switch_textures/gadget_switch_solid_points.inc"
};

static NJS_VECTOR gadget_switch_solid_normals[6] = {
#include "stg22/switch_textures/gadget_switch_solid_normals.inc"
};

static NJS_MODEL gadget_switch_solid_model = {
#include "stg22/switch_textures/gadget_switch_solid_model.inc"
};

NJS_OBJECT gadget_switch_solid = {
#include "stg22/switch_textures/gadget_switch_solid.inc"
};

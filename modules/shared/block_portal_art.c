/* the core block's portal, its glow and its modifier, and the basic models of
 * its hulls. core_block_land_node is the block's hull, which the block and
 * the lift collide by. no code reads the portal's rim and plane */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_block.h"

enum {
    BLOCK_TEX_MIU128_CG001,
    BLOCK_TEX_MIU128_CG022,
    BLOCK_TEX_MIU256_CC002,
    BLOCK_TEX_TX_CCPOISON001
};

static Sint16 glow_sibling_polygons[25] = {
#include "shared/block_portal_art/glow_sibling_polygons.inc"
};

static Sint32 glow_sibling_vertices[19] = {
#include "shared/block_portal_art/glow_sibling_vertices.inc"
};

static NJS_CNK_MODEL glow_sibling_mesh = {
#include "shared/block_portal_art/glow_sibling_mesh.inc"
};

static NJS_CNK_OBJECT glow_sibling_object = {
#include "shared/block_portal_art/glow_sibling_object.inc"
};

static Sint16 glow_polygons[25] = {
#include "shared/block_portal_art/glow_polygons.inc"
};

static Sint32 glow_vertices[19] = {
#include "shared/block_portal_art/glow_vertices.inc"
};

NJS_CNK_MODEL core_block_glow[1] = {
#include "shared/block_portal_art/core_block_glow.inc"
};

static NJS_CNK_OBJECT glow_object = {
#include "shared/block_portal_art/glow_object.inc"
};

static Sint16 portal_polygons[325] = {
#include "shared/block_portal_art/portal_polygons.inc"
};

static Sint32 portal_vertices[267] = {
#include "shared/block_portal_art/portal_vertices.inc"
};

NJS_CNK_MODEL core_block_portal = {
#include "shared/block_portal_art/core_block_portal.inc"
};

static NJS_CNK_OBJECT portal_object = {
#include "shared/block_portal_art/portal_object.inc"
};

static NJS_CNK_OBJECT portal_root = {
#include "shared/block_portal_art/portal_root.inc"
};

static Sint16 modifier_polygons[41] = {
#include "shared/block_portal_art/modifier_polygons.inc"
};

struct modifier_vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[8];
    Sint32 end;
};
static struct modifier_vertex_list modifier_vertices = {
#include "shared/block_portal_art/modifier_vertices.inc"
};

NJS_CNK_MODEL core_block_modifier = {
#include "shared/block_portal_art/core_block_modifier.inc"
};

static NJS_CNK_OBJECT modifier_object = {
#include "shared/block_portal_art/modifier_object.inc"
};

static NJS_MATERIAL block_material[1] = {
#include "shared/block_portal_art/block_material.inc"
};

static Sint16 block_strips[21] = {
#include "shared/block_portal_art/block_strips.inc"
};

static NJS_MESHSET block_faces[1] = {
#include "shared/block_portal_art/block_faces.inc"
};

static NJS_POINT3 block_points[8] = {
#include "shared/block_portal_art/block_points.inc"
};

static NJS_VECTOR block_normals[8] = {
#include "shared/block_portal_art/block_normals.inc"
};

static NJS_MODEL block_hull = {
#include "shared/block_portal_art/block_hull.inc"
};

NJS_OBJECT core_block_land_node = {
#include "shared/block_portal_art/core_block_land_node.inc"
};

static NJS_MATERIAL portal_rim_material[1] = {
#include "shared/block_portal_art/portal_rim_material.inc"
};

static Sint16 portal_rim_strips[33] = {
#include "shared/block_portal_art/portal_rim_strips.inc"
};

static NJS_MESHSET portal_rim_faces[1] = {
#include "shared/block_portal_art/portal_rim_faces.inc"
};

static NJS_POINT3 portal_rim_points[16] = {
#include "shared/block_portal_art/portal_rim_points.inc"
};

static NJS_VECTOR portal_rim_normals[16] = {
#include "shared/block_portal_art/portal_rim_normals.inc"
};

static NJS_MODEL portal_rim_hull = {
#include "shared/block_portal_art/portal_rim_hull.inc"
};

static NJS_OBJECT portal_rim_collision = {
#include "shared/block_portal_art/portal_rim_collision.inc"
};

static NJS_MATERIAL portal_plane_material[1] = {
#include "shared/block_portal_art/portal_plane_material.inc"
};

static Sint16 portal_plane_strips[5] = {
#include "shared/block_portal_art/portal_plane_strips.inc"
};

static NJS_MESHSET portal_plane_faces[1] = {
#include "shared/block_portal_art/portal_plane_faces.inc"
};

static NJS_POINT3 portal_plane_points[4] = {
#include "shared/block_portal_art/portal_plane_points.inc"
};

static NJS_VECTOR portal_plane_normals[4] = {
#include "shared/block_portal_art/portal_plane_normals.inc"
};

static NJS_MODEL portal_plane_hull = {
#include "shared/block_portal_art/portal_plane_hull.inc"
};

static NJS_OBJECT portal_plane_collision = {
#include "shared/block_portal_art/portal_plane_collision.inc"
};

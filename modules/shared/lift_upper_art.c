/* the upper portal of BLOCK2, the vertical block of Cannon's Core, and the
 * portal's glow. the four collision hulls of the portals' rims and planes are
 * data that no code reads */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_block.h"

enum {
    BLOCK_TEX_MIU128_CG001,
    BLOCK_TEX_MIU128_CG022,
    BLOCK_TEX_MIU256_CC002,
    BLOCK_TEX_TX_CCPOISON001
};

static Sint16 upper_portal_sibling_polygons[25] = {
#include "shared/lift_upper_art/upper_portal_sibling_polygons.inc"
};

static Sint32 upper_portal_sibling_vertices[19] = {
#include "shared/lift_upper_art/upper_portal_sibling_vertices.inc"
};

static NJS_CNK_MODEL upper_portal_sibling_mesh = {
#include "shared/lift_upper_art/upper_portal_sibling_mesh.inc"
};

static NJS_CNK_OBJECT upper_portal_sibling_object = {
#include "shared/lift_upper_art/upper_portal_sibling_object.inc"
};

static Sint16 upper_portal_polygons[325] = {
#include "shared/lift_upper_art/upper_portal_polygons.inc"
};

static Sint32 upper_portal_vertices[195] = {
#include "shared/lift_upper_art/upper_portal_vertices.inc"
};

NJS_CNK_MODEL core_lift_upper_portal = {
#include "shared/lift_upper_art/core_lift_upper_portal.inc"
};

static NJS_CNK_OBJECT upper_portal_object = {
#include "shared/lift_upper_art/upper_portal_object.inc"
};

static Sint16 upper_glow_polygons[25] = {
#include "shared/lift_upper_art/upper_glow_polygons.inc"
};

static Sint32 upper_glow_vertices[19] = {
#include "shared/lift_upper_art/upper_glow_vertices.inc"
};

NJS_CNK_MODEL core_lift_upper_glow = {
#include "shared/lift_upper_art/core_lift_upper_glow.inc"
};

static NJS_CNK_OBJECT upper_glow_object = {
#include "shared/lift_upper_art/upper_glow_object.inc"
};

static NJS_CNK_OBJECT upper_portal_root = {
#include "shared/lift_upper_art/upper_portal_root.inc"
};

static NJS_MATERIAL lower_rim_material[1] = {
#include "shared/lift_upper_art/lower_rim_material.inc"
};

static Sint16 lower_rim_strips[33] = {
#include "shared/lift_upper_art/lower_rim_strips.inc"
};

static NJS_MESHSET lower_rim_faces[1] = {
#include "shared/lift_upper_art/lower_rim_faces.inc"
};

static NJS_POINT3 lower_rim_points[16] = {
#include "shared/lift_upper_art/lower_rim_points.inc"
};

static NJS_VECTOR lower_rim_normals[16] = {
#include "shared/lift_upper_art/lower_rim_normals.inc"
};

static NJS_MODEL lower_rim_hull = {
#include "shared/lift_upper_art/lower_rim_hull.inc"
};

static NJS_OBJECT lower_rim_collision = {
#include "shared/lift_upper_art/lower_rim_collision.inc"
};

static NJS_MATERIAL upper_rim_material[1] = {
#include "shared/lift_upper_art/upper_rim_material.inc"
};

static Sint16 upper_rim_strips[33] = {
#include "shared/lift_upper_art/upper_rim_strips.inc"
};

static NJS_MESHSET upper_rim_faces[1] = {
#include "shared/lift_upper_art/upper_rim_faces.inc"
};

static NJS_POINT3 upper_rim_points[16] = {
#include "shared/lift_upper_art/upper_rim_points.inc"
};

static NJS_VECTOR upper_rim_normals[16] = {
#include "shared/lift_upper_art/upper_rim_normals.inc"
};

static NJS_MODEL upper_rim_hull = {
#include "shared/lift_upper_art/upper_rim_hull.inc"
};

static NJS_OBJECT upper_rim_collision = {
#include "shared/lift_upper_art/upper_rim_collision.inc"
};

static NJS_MATERIAL lower_plane_material[1] = {
#include "shared/lift_upper_art/lower_plane_material.inc"
};

static Sint16 lower_plane_strips[5] = {
#include "shared/lift_upper_art/lower_plane_strips.inc"
};

static NJS_MESHSET lower_plane_faces[1] = {
#include "shared/lift_upper_art/lower_plane_faces.inc"
};

static NJS_POINT3 lower_plane_points[4] = {
#include "shared/lift_upper_art/lower_plane_points.inc"
};

static NJS_VECTOR lower_plane_normals[4] = {
#include "shared/lift_upper_art/lower_plane_normals.inc"
};

static NJS_MODEL lower_plane_hull = {
#include "shared/lift_upper_art/lower_plane_hull.inc"
};

static NJS_OBJECT lower_plane_collision = {
#include "shared/lift_upper_art/lower_plane_collision.inc"
};

static NJS_MATERIAL upper_plane_material[1] = {
#include "shared/lift_upper_art/upper_plane_material.inc"
};

static Sint16 upper_plane_strips[5] = {
#include "shared/lift_upper_art/upper_plane_strips.inc"
};

static NJS_MESHSET upper_plane_faces[1] = {
#include "shared/lift_upper_art/upper_plane_faces.inc"
};

static NJS_POINT3 upper_plane_points[4] = {
#include "shared/lift_upper_art/upper_plane_points.inc"
};

static NJS_VECTOR upper_plane_normals[4] = {
#include "shared/lift_upper_art/upper_plane_normals.inc"
};

static NJS_MODEL upper_plane_hull = {
#include "shared/lift_upper_art/upper_plane_hull.inc"
};

static NJS_OBJECT upper_plane_collision = {
#include "shared/lift_upper_art/upper_plane_collision.inc"
};

/* the model of Cannon's Core's solid object, and its collision tree. the tree
 * holds the body's hull, then the rails' sides and tops. no code reads
 * solid_object or solid_collision_root */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_solid.h"

enum {
    BLOCK_TEX_ET64_3,
    BLOCK_TEX_ET64_4,
    BLOCK_TEX_MIU128_CG022,
    BLOCK_TEX_MIU512_CG001,
    BLOCK_TEX_MIU512_CG003,
    BLOCK_TEX_MIU64_CG007,
    BLOCK_TEX_MIU64_CG010
};

static Sint16 solid_polygons[479] = {
#include "shared/core_solid_art/solid_polygons.inc"
};

static Sint32 solid_vertices[339] = {
#include "shared/core_solid_art/solid_vertices.inc"
};

NJS_CNK_MODEL core_solid_model = {
#include "shared/core_solid_art/core_solid_model.inc"
};

static NJS_CNK_OBJECT solid_object = {
#include "shared/core_solid_art/solid_object.inc"
};

static NJS_MATERIAL rail_tops_material[1] = {
#include "shared/core_solid_art/rail_tops_material.inc"
};

static Sint16 rail_tops_strips[10] = {
#include "shared/core_solid_art/rail_tops_strips.inc"
};

static NJS_MESHSET rail_tops_faces[1] = {
#include "shared/core_solid_art/rail_tops_faces.inc"
};

static NJS_POINT3 rail_tops_points[8] = {
#include "shared/core_solid_art/rail_tops_points.inc"
};

static NJS_VECTOR rail_tops_normals[8] = {
#include "shared/core_solid_art/rail_tops_normals.inc"
};

static NJS_MODEL rail_tops_hull = {
#include "shared/core_solid_art/rail_tops_hull.inc"
};

static NJS_OBJECT rail_tops_collision = {
#include "shared/core_solid_art/rail_tops_collision.inc"
};

static NJS_MATERIAL rail_sides_material[1] = {
#include "shared/core_solid_art/rail_sides_material.inc"
};

static Sint16 rail_sides_strips[18] = {
#include "shared/core_solid_art/rail_sides_strips.inc"
};

static NJS_MESHSET rail_sides_faces[1] = {
#include "shared/core_solid_art/rail_sides_faces.inc"
};

static NJS_POINT3 rail_sides_points[16] = {
#include "shared/core_solid_art/rail_sides_points.inc"
};

static NJS_VECTOR rail_sides_normals[16] = {
#include "shared/core_solid_art/rail_sides_normals.inc"
};

static NJS_MODEL rail_sides_hull = {
#include "shared/core_solid_art/rail_sides_hull.inc"
};

static NJS_OBJECT rail_sides_collision = {
#include "shared/core_solid_art/rail_sides_collision.inc"
};

static NJS_MATERIAL solid_material[1] = {
#include "shared/core_solid_art/solid_material.inc"
};

static Sint16 solid_strips[19] = {
#include "shared/core_solid_art/solid_strips.inc"
};

static NJS_MESHSET solid_faces[1] = {
#include "shared/core_solid_art/solid_faces.inc"
};

static NJS_POINT3 solid_points[8] = {
#include "shared/core_solid_art/solid_points.inc"
};

static NJS_VECTOR solid_normals[8] = {
#include "shared/core_solid_art/solid_normals.inc"
};

static NJS_MODEL solid_hull = {
#include "shared/core_solid_art/solid_hull.inc"
};

NJS_OBJECT core_solid_collision = {
#include "shared/core_solid_art/core_solid_collision.inc"
};

static NJS_OBJECT solid_collision_root = {
#include "shared/core_solid_art/solid_collision_root.inc"
};

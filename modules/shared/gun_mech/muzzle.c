/* the gunfire's three muzzle flashes. boss_gunfire.c draws a for frames 0 and
 * 1, b for 2, and c for 3 and 4 */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 muzzle_object_a_polygons[35] = {
#include "shared/gun_mech/muzzle/muzzle_object_a_polygons.inc"
};

static Sint32 muzzle_object_a_vertices[75] = {
#include "shared/gun_mech/muzzle/muzzle_object_a_vertices.inc"
};

static NJS_CNK_MODEL muzzle_object_a_mesh = {
#include "shared/gun_mech/muzzle/muzzle_object_a_mesh.inc"
};

NJS_CNK_OBJECT muzzle_object_a = {
#include "shared/gun_mech/muzzle/muzzle_object_a.inc"
};

static Sint16 muzzle_object_b_polygons[35] = {
#include "shared/gun_mech/muzzle/muzzle_object_b_polygons.inc"
};

static Sint32 muzzle_object_b_vertices[75] = {
#include "shared/gun_mech/muzzle/muzzle_object_b_vertices.inc"
};

static NJS_CNK_MODEL muzzle_object_b_mesh = {
#include "shared/gun_mech/muzzle/muzzle_object_b_mesh.inc"
};

NJS_CNK_OBJECT muzzle_object_b = {
#include "shared/gun_mech/muzzle/muzzle_object_b.inc"
};

static Sint16 muzzle_object_c_polygons[35] = {
#include "shared/gun_mech/muzzle/muzzle_object_c_polygons.inc"
};

static Sint32 muzzle_object_c_vertices[75] = {
#include "shared/gun_mech/muzzle/muzzle_object_c_vertices.inc"
};

static NJS_CNK_MODEL muzzle_object_c_mesh = {
#include "shared/gun_mech/muzzle/muzzle_object_c_mesh.inc"
};

NJS_CNK_OBJECT muzzle_object_c = {
#include "shared/gun_mech/muzzle/muzzle_object_c.inc"
};

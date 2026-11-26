/* the laser's spot on the ground, which mech_boss_laser.c draws. it is an outer
 * model with an inner one as its child */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 spot_inner_polygons[43] = {
#include "shared/gun_mech/laser_spot/spot_inner_polygons.inc"
};

static Sint32 spot_inner_vertices[171] = {
#include "shared/gun_mech/laser_spot/spot_inner_vertices.inc"
};

static NJS_CNK_MODEL spot_inner_mesh = {
#include "shared/gun_mech/laser_spot/spot_inner_mesh.inc"
};

static NJS_CNK_OBJECT spot_inner = {
#include "shared/gun_mech/laser_spot/spot_inner.inc"
};

static Sint16 laser_polygons[75] = {
#include "shared/gun_mech/laser_spot/laser_polygons.inc"
};

static Sint32 laser_vertices[291] = {
#include "shared/gun_mech/laser_spot/laser_vertices.inc"
};

static NJS_CNK_MODEL laser_mesh = {
#include "shared/gun_mech/laser_spot/laser_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_laser_object = {
#include "shared/gun_mech/laser_spot/mech_boss_laser_object.inc"
};

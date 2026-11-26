/* the model that mech_boss_missile.c draws for a missile in flight */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 missile_polygons[183] = {
#include "shared/gun_mech/missile/missile_polygons.inc"
};

static Sint32 missile_vertices[207] = {
#include "shared/gun_mech/missile/missile_vertices.inc"
};

static NJS_CNK_MODEL missile_mesh = {
#include "shared/gun_mech/missile/missile_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_missile_object = {
#include "shared/gun_mech/missile/mech_boss_missile_object.inc"
};

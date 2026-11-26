/* the pulse of colored vertices that mech_boss_laser_beam.c draws along the
 * laser's beam */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 laser_pulse_polygons[55] = {
#include "shared/gun_mech/laser_pulse/laser_pulse_polygons.inc"
};

static Sint32 laser_pulse_vertices[107] = {
#include "shared/gun_mech/laser_pulse/laser_pulse_vertices.inc"
};

static NJS_CNK_MODEL laser_pulse_mesh = {
#include "shared/gun_mech/laser_pulse/laser_pulse_mesh.inc"
};

NJS_CNK_OBJECT laser_pulse_object = {
#include "shared/gun_mech/laser_pulse/laser_pulse_object.inc"
};

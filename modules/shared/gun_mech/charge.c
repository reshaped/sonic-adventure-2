/* the model of the beam's charge, whose texture mech_boss_beam.c animates as it
 * draws. no code reads the node over it */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

enum effect_texture { TEX_SHOT_EFF02 = 4 };

static Sint16 charge_polygons[657] = {
#include "shared/gun_mech/charge/charge_polygons.inc"
};

static Sint32 charge_vertices[435] = {
#include "shared/gun_mech/charge/charge_vertices.inc"
};

NJS_CNK_MODEL mech_boss_beam_charge_model = {
#include "shared/gun_mech/charge/mech_boss_beam_charge_model.inc"
};

static NJS_CNK_OBJECT charge = {
#include "shared/gun_mech/charge/charge.inc"
};

/* the beam's impact: the bomb's model, and the pillar and torus it starts at
 * its full size. no code reads the bomb's node */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

enum effect_texture { TEX_SHOT_EFF01 = 3, TEX_SHOT_EFF02 = 4 };

static Sint16 bomb_polygons[657] = {
#include "shared/gun_mech/beam/bomb_polygons.inc"
};

static Sint32 bomb_vertices[435] = {
#include "shared/gun_mech/beam/bomb_vertices.inc"
};

NJS_CNK_MODEL mech_boss_beam_bomb_model = {
#include "shared/gun_mech/beam/mech_boss_beam_bomb_model.inc"
};

static NJS_CNK_OBJECT bomb = {
#include "shared/gun_mech/beam/bomb.inc"
};

static Sint16 beam_pillar_polygons[79] = {
#include "shared/gun_mech/beam/beam_pillar_polygons.inc"
};

static Sint32 beam_pillar_vertices[123] = {
#include "shared/gun_mech/beam/beam_pillar_vertices.inc"
};

static NJS_CNK_MODEL beam_pillar_mesh = {
#include "shared/gun_mech/beam/beam_pillar_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_beam_pillar_object = {
#include "shared/gun_mech/beam/mech_boss_beam_pillar_object.inc"
};

static Sint16 beam_torus_polygons[79] = {
#include "shared/gun_mech/beam/beam_torus_polygons.inc"
};

static Sint32 beam_torus_vertices[123] = {
#include "shared/gun_mech/beam/beam_torus_vertices.inc"
};

static NJS_CNK_MODEL beam_torus_mesh = {
#include "shared/gun_mech/beam/beam_torus_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_beam_torus_object = {
#include "shared/gun_mech/beam/mech_boss_beam_torus_object.inc"
};

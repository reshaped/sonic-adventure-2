/* a whole foothold's height, and the footholds' models */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/footholds.h"
#include "shared/golem_foothold_art.h"
#include "boss_golem/foothold_art/text.inc"

Float golem_floor_y = 200.0F;

#include "shared/golem_footholds/a_textures.h"

NJS_TEXLIST golem_foothold_a_texlist = {
#include "boss_golem/foothold_art/golem_foothold_a_texlist.inc"
};

#include "shared/golem_footholds/a_mesh.h"

NJS_CNK_MODEL golem_foothold_a_model = {
#include "boss_golem/foothold_art/golem_foothold_a_model.inc"
};

NJS_CNK_OBJECT golem_foothold_a_object = {
#include "boss_golem/foothold_art/golem_foothold_a_object.inc"
};

#include "shared/golem_footholds/b_textures.h"

NJS_TEXLIST golem_foothold_b_texlist = {
#include "boss_golem/foothold_art/golem_foothold_b_texlist.inc"
};

#include "shared/golem_footholds/b_mesh.h"

NJS_CNK_MODEL golem_foothold_b_model = {
#include "boss_golem/foothold_art/golem_foothold_b_model.inc"
};

NJS_CNK_OBJECT golem_foothold_b_object = {
#include "boss_golem/foothold_art/golem_foothold_b_object.inc"
};

#include "shared/golem_footholds/sensor_mesh.h"

NJS_OBJECT golem_foothold_sensor_object = {
#include "boss_golem/foothold_art/golem_foothold_sensor_object.inc"
};

#include "shared/golem_footholds/side_mesh.h"

NJS_OBJECT golem_foothold_side_object = {
#include "boss_golem/foothold_art/golem_foothold_side_object.inc"
};

#include "shared/golem_footholds/pieces_textures.h"

NJS_TEXLIST golem_foothold_pieces_texlist = {
#include "boss_golem/foothold_art/golem_foothold_pieces_texlist.inc"
};

#include "shared/golem_footholds/pieces_mesh.h"

NJS_CNK_OBJECT golem_foothold_pieces[1] = {
#include "boss_golem/foothold_art/golem_foothold_pieces.inc"
};

#include "shared/golem_footholds/sink_motion.h"

NJS_MOTION golem_foothold_sink_motion[1] = {
#include "boss_golem/foothold_art/golem_foothold_sink_motion.inc"
};

NJS_TEXLIST *golem_foothold_texlists[4] = {
#include "boss_golem/foothold_art/golem_foothold_texlists.inc"
};

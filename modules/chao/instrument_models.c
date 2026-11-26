/* CHAO.PRS's chunk models, from the disc. its D lies between
 * animal_actions.c and instruments.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/instrument_models.h"

static Sint16 chao_maraca_4_polygons[223] = {
#include "chao/instrument_models/chao_maraca_4_polygons.inc"
};
static Sint32 chao_maraca_4_vertices[147] = {
#include "chao/instrument_models/chao_maraca_4_vertices.inc"
};
static NJS_CNK_MODEL chao_maraca_4_model = {
#include "chao/instrument_models/chao_maraca_4_model.inc"
};
NJS_CNK_OBJECT chao_maraca_4 = {
#include "chao/instrument_models/chao_maraca_4.inc"
};
NJS_CNK_OBJECT chao_maraca_4_root = {
#include "chao/instrument_models/chao_maraca_4_root.inc"
};
static Sint16 chao_maraca_11_polygons[223] = {
#include "chao/instrument_models/chao_maraca_11_polygons.inc"
};
static Sint32 chao_maraca_11_vertices[147] = {
#include "chao/instrument_models/chao_maraca_11_vertices.inc"
};
static NJS_CNK_MODEL chao_maraca_11_model = {
#include "chao/instrument_models/chao_maraca_11_model.inc"
};
NJS_CNK_OBJECT chao_maraca_11 = {
#include "chao/instrument_models/chao_maraca_11.inc"
};
NJS_CNK_OBJECT chao_maraca_11_root = {
#include "chao/instrument_models/chao_maraca_11_root.inc"
};
static Sint16 chao_bell_polygons[139] = {
#include "chao/instrument_models/chao_bell_polygons.inc"
};
static Sint32 chao_bell_vertices[297] = {
#include "chao/instrument_models/chao_bell_vertices.inc"
};
static NJS_CNK_MODEL chao_bell_model = {
#include "chao/instrument_models/chao_bell_model.inc"
};
NJS_CNK_OBJECT chao_bell = {
#include "chao/instrument_models/chao_bell.inc"
};
static Sint16 chao_drum_polygons[239] = {
#include "chao/instrument_models/chao_drum_polygons.inc"
};
static Sint32 chao_drum_vertices[243] = {
#include "chao/instrument_models/chao_drum_vertices.inc"
};
static NJS_CNK_MODEL chao_drum_model = {
#include "chao/instrument_models/chao_drum_model.inc"
};
NJS_CNK_OBJECT chao_drum = {
#include "chao/instrument_models/chao_drum.inc"
};
NJS_CNK_OBJECT chao_drum_root = {
#include "chao/instrument_models/chao_drum_root.inc"
};
static Sint16 chao_drumstick_4_polygons[145] = {
#include "chao/instrument_models/chao_drumstick_4_polygons.inc"
};
static Sint32 chao_drumstick_4_vertices[117] = {
#include "chao/instrument_models/chao_drumstick_4_vertices.inc"
};
static NJS_CNK_MODEL chao_drumstick_4_model = {
#include "chao/instrument_models/chao_drumstick_4_model.inc"
};
NJS_CNK_OBJECT chao_drumstick_4 = {
#include "chao/instrument_models/chao_drumstick_4.inc"
};
NJS_CNK_OBJECT chao_drumstick_4_root = {
#include "chao/instrument_models/chao_drumstick_4_root.inc"
};
static Sint16 chao_drumstick_11_polygons[137] = {
#include "chao/instrument_models/chao_drumstick_11_polygons.inc"
};
static Sint32 chao_drumstick_11_vertices[117] = {
#include "chao/instrument_models/chao_drumstick_11_vertices.inc"
};
static NJS_CNK_MODEL chao_drumstick_11_model = {
#include "chao/instrument_models/chao_drumstick_11_model.inc"
};
NJS_CNK_OBJECT chao_drumstick_11 = {
#include "chao/instrument_models/chao_drumstick_11.inc"
};
NJS_CNK_OBJECT chao_drumstick_11_root = {
#include "chao/instrument_models/chao_drumstick_11_root.inc"
};
static Sint16 chao_flute_polygons[467] = {
#include "chao/instrument_models/chao_flute_polygons.inc"
};
static Sint32 chao_flute_vertices[333] = {
#include "chao/instrument_models/chao_flute_vertices.inc"
};
static NJS_CNK_MODEL chao_flute_model = {
#include "chao/instrument_models/chao_flute_model.inc"
};
NJS_CNK_OBJECT chao_flute = {
#include "chao/instrument_models/chao_flute.inc"
};
static Sint16 chao_castanets_node_1_polygons[167] = {
#include "chao/instrument_models/chao_castanets_node_1_polygons.inc"
};
static Sint32 chao_castanets_node_1_vertices[111] = {
#include "chao/instrument_models/chao_castanets_node_1_vertices.inc"
};
static NJS_CNK_MODEL chao_castanets_node_1_model = {
#include "chao/instrument_models/chao_castanets_node_1_model.inc"
};
static NJS_CNK_OBJECT chao_castanets_node_1 = {
#include "chao/instrument_models/chao_castanets_node_1.inc"
};
static Sint16 chao_castanets_polygons[153] = {
#include "chao/instrument_models/chao_castanets_polygons.inc"
};
static Sint32 chao_castanets_vertices[111] = {
#include "chao/instrument_models/chao_castanets_vertices.inc"
};
static NJS_CNK_MODEL chao_castanets_model = {
#include "chao/instrument_models/chao_castanets_model.inc"
};
NJS_CNK_OBJECT chao_castanets = {
#include "chao/instrument_models/chao_castanets.inc"
};
static Sint16 chao_trumpet_polygons[251] = {
#include "chao/instrument_models/chao_trumpet_polygons.inc"
};
static Sint32 chao_trumpet_vertices[381] = {
#include "chao/instrument_models/chao_trumpet_vertices.inc"
};
static NJS_CNK_MODEL chao_trumpet_model = {
#include "chao/instrument_models/chao_trumpet_model.inc"
};
NJS_CNK_OBJECT chao_trumpet = {
#include "chao/instrument_models/chao_trumpet.inc"
};
static Sint16 chao_cymbal_4_polygons[199] = {
#include "chao/instrument_models/chao_cymbal_4_polygons.inc"
};
static Sint32 chao_cymbal_4_vertices[273] = {
#include "chao/instrument_models/chao_cymbal_4_vertices.inc"
};
static NJS_CNK_MODEL chao_cymbal_4_model = {
#include "chao/instrument_models/chao_cymbal_4_model.inc"
};
NJS_CNK_OBJECT chao_cymbal_4 = {
#include "chao/instrument_models/chao_cymbal_4.inc"
};
NJS_CNK_OBJECT chao_cymbal_4_root = {
#include "chao/instrument_models/chao_cymbal_4_root.inc"
};
static Sint16 chao_cymbal_11_polygons[203] = {
#include "chao/instrument_models/chao_cymbal_11_polygons.inc"
};
static Sint32 chao_cymbal_11_vertices[273] = {
#include "chao/instrument_models/chao_cymbal_11_vertices.inc"
};
static NJS_CNK_MODEL chao_cymbal_11_model = {
#include "chao/instrument_models/chao_cymbal_11_model.inc"
};
NJS_CNK_OBJECT chao_cymbal_11 = {
#include "chao/instrument_models/chao_cymbal_11.inc"
};
NJS_CNK_OBJECT chao_cymbal_11_root = {
#include "chao/instrument_models/chao_cymbal_11_root.inc"
};
static Sint16 chao_tambourine_node_5_polygons[27] = {
#include "chao/instrument_models/chao_tambourine_node_5_polygons.inc"
};
static Sint32 chao_tambourine_node_5_vertices[27] = {
#include "chao/instrument_models/chao_tambourine_node_5_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_node_5_model = {
#include "chao/instrument_models/chao_tambourine_node_5_model.inc"
};
static NJS_CNK_OBJECT chao_tambourine_node_5 = {
#include "chao/instrument_models/chao_tambourine_node_5.inc"
};
static Sint16 chao_tambourine_node_4_polygons[27] = {
#include "chao/instrument_models/chao_tambourine_node_4_polygons.inc"
};
static Sint32 chao_tambourine_node_4_vertices[27] = {
#include "chao/instrument_models/chao_tambourine_node_4_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_node_4_model = {
#include "chao/instrument_models/chao_tambourine_node_4_model.inc"
};
static NJS_CNK_OBJECT chao_tambourine_node_4 = {
#include "chao/instrument_models/chao_tambourine_node_4.inc"
};
static Sint16 chao_tambourine_node_3_polygons[27] = {
#include "chao/instrument_models/chao_tambourine_node_3_polygons.inc"
};
static Sint32 chao_tambourine_node_3_vertices[27] = {
#include "chao/instrument_models/chao_tambourine_node_3_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_node_3_model = {
#include "chao/instrument_models/chao_tambourine_node_3_model.inc"
};
static NJS_CNK_OBJECT chao_tambourine_node_3 = {
#include "chao/instrument_models/chao_tambourine_node_3.inc"
};
static Sint16 chao_tambourine_node_2_polygons[27] = {
#include "chao/instrument_models/chao_tambourine_node_2_polygons.inc"
};
static Sint32 chao_tambourine_node_2_vertices[27] = {
#include "chao/instrument_models/chao_tambourine_node_2_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_node_2_model = {
#include "chao/instrument_models/chao_tambourine_node_2_model.inc"
};
static NJS_CNK_OBJECT chao_tambourine_node_2 = {
#include "chao/instrument_models/chao_tambourine_node_2.inc"
};
static Sint16 chao_tambourine_node_1_polygons[27] = {
#include "chao/instrument_models/chao_tambourine_node_1_polygons.inc"
};
static Sint32 chao_tambourine_node_1_vertices[27] = {
#include "chao/instrument_models/chao_tambourine_node_1_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_node_1_model = {
#include "chao/instrument_models/chao_tambourine_node_1_model.inc"
};
static NJS_CNK_OBJECT chao_tambourine_node_1 = {
#include "chao/instrument_models/chao_tambourine_node_1.inc"
};
static Sint16 chao_tambourine_polygons[231] = {
#include "chao/instrument_models/chao_tambourine_polygons.inc"
};
static Sint32 chao_tambourine_vertices[225] = {
#include "chao/instrument_models/chao_tambourine_vertices.inc"
};
static NJS_CNK_MODEL chao_tambourine_model = {
#include "chao/instrument_models/chao_tambourine_model.inc"
};
NJS_CNK_OBJECT chao_tambourine = {
#include "chao/instrument_models/chao_tambourine.inc"
};
NJS_CNK_OBJECT chao_tambourine_root = {
#include "chao/instrument_models/chao_tambourine_root.inc"
};

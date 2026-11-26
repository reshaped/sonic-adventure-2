/* CHAO.PRS's chunk models, from the disc. its D lies between radio.c and
 * television.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/garden_decor.h"
#include "chao/toy_art.h"

static Sint16 garden_tv_screen_polygons[51] = {
#include "chao/television_models/garden_tv_screen_polygons.inc"
};
static Sint32 garden_tv_screen_vertices[63] = {
#include "chao/television_models/garden_tv_screen_vertices.inc"
};
static NJS_CNK_MODEL garden_tv_screen_model = {
#include "chao/television_models/garden_tv_screen_model.inc"
};
NJS_CNK_OBJECT garden_tv_screen[1] = {
#include "chao/television_models/garden_tv_screen.inc"
};
static Sint16 garden_tv_model_polygons[671] = {
#include "chao/television_models/garden_tv_model_polygons.inc"
};
static Sint32 garden_tv_model_vertices[579] = {
#include "chao/television_models/garden_tv_model_vertices.inc"
};
static NJS_CNK_MODEL garden_tv_model_model = {
#include "chao/television_models/garden_tv_model_model.inc"
};
NJS_CNK_OBJECT garden_tv_model = {
#include "chao/television_models/garden_tv_model.inc"
};
NJS_CNK_OBJECT garden_tv_root = {
#include "chao/television_models/garden_tv_root.inc"
};
static Sint16 garden_toy_duck_node_1_polygons[597] = {
#include "chao/television_models/garden_toy_duck_node_1_polygons.inc"
};
static Sint32 garden_toy_duck_node_1_vertices[399] = {
#include "chao/television_models/garden_toy_duck_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_duck_node_1_model = {
#include "chao/television_models/garden_toy_duck_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_toy_duck_node_1 = {
#include "chao/television_models/garden_toy_duck_node_1.inc"
};
NJS_CNK_OBJECT garden_toy_duck = {
#include "chao/television_models/garden_toy_duck.inc"
};
static Sint16 garden_toy_board_node_2_polygons[285] = {
#include "chao/television_models/garden_toy_board_node_2_polygons.inc"
};
static Sint32 garden_toy_board_node_2_vertices[183] = {
#include "chao/television_models/garden_toy_board_node_2_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_board_node_2_model = {
#include "chao/television_models/garden_toy_board_node_2_model.inc"
};
static NJS_CNK_OBJECT garden_toy_board_node_2 = {
#include "chao/television_models/garden_toy_board_node_2.inc"
};
static Sint16 garden_toy_board_node_1_polygons[283] = {
#include "chao/television_models/garden_toy_board_node_1_polygons.inc"
};
static Sint32 garden_toy_board_node_1_vertices[147] = {
#include "chao/television_models/garden_toy_board_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_board_node_1_model = {
#include "chao/television_models/garden_toy_board_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_toy_board_node_1 = {
#include "chao/television_models/garden_toy_board_node_1.inc"
};
NJS_CNK_OBJECT garden_toy_board = {
#include "chao/television_models/garden_toy_board.inc"
};
static Sint16 garden_toy_pipe_stand_node_2_polygons[95] = {
#include "chao/television_models/garden_toy_pipe_stand_node_2_polygons.inc"
};
static Sint32 garden_toy_pipe_stand_node_2_vertices[255] = {
#include "chao/television_models/garden_toy_pipe_stand_node_2_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_pipe_stand_node_2_model = {
#include "chao/television_models/garden_toy_pipe_stand_node_2_model.inc"
};
static NJS_CNK_OBJECT garden_toy_pipe_stand_node_2 = {
#include "chao/television_models/garden_toy_pipe_stand_node_2.inc"
};
static Sint16 garden_toy_pipe_stand_node_1_polygons[371] = {
#include "chao/television_models/garden_toy_pipe_stand_node_1_polygons.inc"
};
static Sint32 garden_toy_pipe_stand_node_1_vertices[231] = {
#include "chao/television_models/garden_toy_pipe_stand_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_pipe_stand_node_1_model = {
#include "chao/television_models/garden_toy_pipe_stand_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_toy_pipe_stand_node_1 = {
#include "chao/television_models/garden_toy_pipe_stand_node_1.inc"
};
NJS_CNK_OBJECT garden_toy_pipe_stand = {
#include "chao/television_models/garden_toy_pipe_stand.inc"
};
static Sint16 garden_toy_piano_node_1_polygons[615] = {
#include "chao/television_models/garden_toy_piano_node_1_polygons.inc"
};
static Sint32 garden_toy_piano_node_1_vertices[621] = {
#include "chao/television_models/garden_toy_piano_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_piano_node_1_model = {
#include "chao/television_models/garden_toy_piano_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_toy_piano_node_1 = {
#include "chao/television_models/garden_toy_piano_node_1.inc"
};
NJS_CNK_OBJECT garden_toy_piano = {
#include "chao/television_models/garden_toy_piano.inc"
};
static Sint16 garden_toy_ring_node_1_polygons[467] = {
#include "chao/television_models/garden_toy_ring_node_1_polygons.inc"
};
static Sint32 garden_toy_ring_node_1_vertices[291] = {
#include "chao/television_models/garden_toy_ring_node_1_vertices.inc"
};
static NJS_CNK_MODEL garden_toy_ring_node_1_model = {
#include "chao/television_models/garden_toy_ring_node_1_model.inc"
};
static NJS_CNK_OBJECT garden_toy_ring_node_1 = {
#include "chao/television_models/garden_toy_ring_node_1.inc"
};
NJS_CNK_OBJECT garden_toy_ring = {
#include "chao/television_models/garden_toy_ring.inc"
};

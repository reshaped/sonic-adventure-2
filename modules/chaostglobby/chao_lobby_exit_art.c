/* CHAOSTGLOBBY.PRS's texture lists and chunk models, from the disc. its D
 * lies between lobby.c and exit.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"

static NJS_TEXNAME lobby_texture_names[67] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_texture_names.inc"
};
NJS_TEXLIST lobby_texlist = {
#include "chaostglobby/chao_lobby_exit_art/lobby_texlist.inc"
};
static Sint16 lobby_exit_base_polygons[3453] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_base_polygons.inc"
};
static Sint32 lobby_exit_base_vertices[3813] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_base_vertices.inc"
};
static NJS_CNK_MODEL lobby_exit_base_model = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_base_model.inc"
};
NJS_CNK_OBJECT lobby_exit_base = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_base.inc"
};
static Sint16 lobby_exit_frame_polygons[2097] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_frame_polygons.inc"
};
static Sint32 lobby_exit_frame_vertices[3867] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_frame_vertices.inc"
};
static NJS_CNK_MODEL lobby_exit_frame_model = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_frame_model.inc"
};
NJS_CNK_OBJECT lobby_exit_frame = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_frame.inc"
};
static Sint16 lobby_exit_disc_polygons[123] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_disc_polygons.inc"
};
static Sint32 lobby_exit_disc_vertices[147] = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_disc_vertices.inc"
};
static NJS_CNK_MODEL lobby_exit_disc_model = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_disc_model.inc"
};
NJS_CNK_OBJECT lobby_exit_disc = {
#include "chaostglobby/chao_lobby_exit_art/lobby_exit_disc.inc"
};

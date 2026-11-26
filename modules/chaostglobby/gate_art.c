/* CHAOSTGLOBBY.PRS's chunk models, from the disc. its D lies between exit.c
 * and dark.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostglobby/gate_art.h"

static Sint16 lobby_dark_arch_polygons[10619] = {
#include "chaostglobby/gate_art/lobby_dark_arch_polygons.inc"
};
static Sint32 lobby_dark_arch_vertices[8947] = {
#include "chaostglobby/gate_art/lobby_dark_arch_vertices.inc"
};
static NJS_CNK_MODEL lobby_dark_arch_model = {
#include "chaostglobby/gate_art/lobby_dark_arch_model.inc"
};
NJS_CNK_OBJECT lobby_dark_arch = {
#include "chaostglobby/gate_art/lobby_dark_arch.inc"
};
static Sint16 lobby_dark_threshold_polygons[155] = {
#include "chaostglobby/gate_art/lobby_dark_threshold_polygons.inc"
};
static Sint32 lobby_dark_threshold_vertices[103] = {
#include "chaostglobby/gate_art/lobby_dark_threshold_vertices.inc"
};
static NJS_CNK_MODEL lobby_dark_threshold_model = {
#include "chaostglobby/gate_art/lobby_dark_threshold_model.inc"
};
NJS_CNK_OBJECT lobby_dark_threshold = {
#include "chaostglobby/gate_art/lobby_dark_threshold.inc"
};

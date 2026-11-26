/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between grind_rail.c and rail_marker.c, and its D lies between
 * shutter.c and grind_rail.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_grind_rail_art/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg22/gadget_grind_rail_art/textures.inc"
};
NJS_TEXLIST gadget_grind_rail_texlist = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_texlist.inc"
};
static Sint16 gadget_grind_rail_node_2_polygons[43] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_2_polygons.inc"
};
static Sint32 gadget_grind_rail_node_2_vertices[35] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_2_vertices.inc"
};
static NJS_CNK_MODEL gadget_grind_rail_node_2_mesh = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_2_mesh.inc"
};
static NJS_CNK_OBJECT gadget_grind_rail_node_2 = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_2.inc"
};
static Sint16 gadget_grind_rail_node_1_polygons[43] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_1_polygons.inc"
};
static Sint32 gadget_grind_rail_node_1_vertices[35] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_1_vertices.inc"
};
static NJS_CNK_MODEL gadget_grind_rail_node_1_mesh = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_1_mesh.inc"
};
static NJS_CNK_OBJECT gadget_grind_rail_node_1 = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_node_1.inc"
};
static Sint16 gadget_grind_rail_polygons[147] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_polygons.inc"
};
static Sint32 gadget_grind_rail_vertices[83] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_vertices.inc"
};
static NJS_CNK_MODEL gadget_grind_rail_mesh = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail_mesh.inc"
};
NJS_CNK_OBJECT gadget_grind_rail[1] = {
#include "stg22/gadget_grind_rail_art/gadget_grind_rail.inc"
};

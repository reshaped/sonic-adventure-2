/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between sign_textures.c and ditch.c, and its D lies between
 * sign_textures.c and sign.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_sign_panel_art/text.inc"

static NJS_TEXNAME panel_name[1] = {
#include "stg22/gadget_sign_panel_art/panel_name.inc"
};
NJS_TEXLIST gadget_sign_panel_texlist = {
#include "stg22/gadget_sign_panel_art/gadget_sign_panel_texlist.inc"
};
static Sint16 gadget_sign_e_node_4_polygons[25] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_4_polygons.inc"
};
static Sint32 gadget_sign_e_node_4_vertices[19] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_4_vertices.inc"
};
static NJS_CNK_MODEL gadget_sign_e_node_4_mesh = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_4_mesh.inc"
};
static NJS_CNK_OBJECT gadget_sign_e_node_4 = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_4.inc"
};
static Sint16 gadget_sign_e_node_3_polygons[25] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_3_polygons.inc"
};
static Sint32 gadget_sign_e_node_3_vertices[19] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_3_vertices.inc"
};
static NJS_CNK_MODEL gadget_sign_e_node_3_mesh = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_3_mesh.inc"
};
static NJS_CNK_OBJECT gadget_sign_e_node_3 = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_3.inc"
};
static Sint16 gadget_sign_e_node_2_polygons[25] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_2_polygons.inc"
};
static Sint32 gadget_sign_e_node_2_vertices[19] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_2_vertices.inc"
};
static NJS_CNK_MODEL gadget_sign_e_node_2_mesh = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_2_mesh.inc"
};
static NJS_CNK_OBJECT gadget_sign_e_node_2 = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_2.inc"
};
static Sint16 gadget_sign_e_node_1_polygons[25] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_1_polygons.inc"
};
static Sint32 gadget_sign_e_node_1_vertices[19] = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_1_vertices.inc"
};
static NJS_CNK_MODEL gadget_sign_e_node_1_mesh = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_1_mesh.inc"
};
static NJS_CNK_OBJECT gadget_sign_e_node_1 = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e_node_1.inc"
};
NJS_CNK_OBJECT gadget_sign_e = {
#include "stg22/gadget_sign_panel_art/gadget_sign_e.inc"
};

/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between arrow.c and rolling_drum.c, and its D lies between
 * pickup_object_2.c and arrow.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_arrow_art/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg22/gadget_arrow_art/textures.inc"
};
NJS_TEXLIST gadget_arrow_texlist = {
#include "stg22/gadget_arrow_art/gadget_arrow_texlist.inc"
};
static Sint16 gadget_arrow_object_node_1_polygons[25] = {
#include "stg22/gadget_arrow_art/gadget_arrow_object_node_1_polygons.inc"
};
static Sint32 gadget_arrow_object_node_1_vertices[19] = {
#include "stg22/gadget_arrow_art/gadget_arrow_object_node_1_vertices.inc"
};
static NJS_CNK_MODEL gadget_arrow_object_node_1_mesh = {
#include "stg22/gadget_arrow_art/gadget_arrow_object_node_1_mesh.inc"
};
static NJS_CNK_OBJECT gadget_arrow_object_node_1 = {
#include "stg22/gadget_arrow_art/gadget_arrow_object_node_1.inc"
};
static Sint16 gadget_arrow_polygons[419] = {
#include "stg22/gadget_arrow_art/gadget_arrow_polygons.inc"
};
static Sint32 gadget_arrow_vertices[371] = {
#include "stg22/gadget_arrow_art/gadget_arrow_vertices.inc"
};
static NJS_CNK_MODEL gadget_arrow_mesh = {
#include "stg22/gadget_arrow_art/gadget_arrow_mesh.inc"
};
NJS_CNK_OBJECT gadget_arrow_object = {
#include "stg22/gadget_arrow_art/gadget_arrow_object.inc"
};

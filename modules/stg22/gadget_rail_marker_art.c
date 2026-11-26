/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between rail_marker.c and poison.c, and its D lies between
 * grind_rail.c and rail_marker.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_rail_marker_art/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg22/gadget_rail_marker_art/textures.inc"
};
NJS_TEXLIST gadget_marker_texlist = {
#include "stg22/gadget_rail_marker_art/gadget_marker_texlist.inc"
};
static Sint16 gadget_marker_polygons[271] = {
#include "stg22/gadget_rail_marker_art/gadget_marker_polygons.inc"
};
static Sint32 gadget_marker_vertices[243] = {
#include "stg22/gadget_rail_marker_art/gadget_marker_vertices.inc"
};
static NJS_CNK_MODEL gadget_marker_mesh = {
#include "stg22/gadget_rail_marker_art/gadget_marker_mesh.inc"
};
NJS_CNK_OBJECT gadget_marker_object = {
#include "stg22/gadget_rail_marker_art/gadget_marker_object.inc"
};

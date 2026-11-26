/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between capsule.c and colony_laser.c, and its D lies between
 * machine.c and capsule.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_capsule_art/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg22/gadget_capsule_art/textures.inc"
};
NJS_TEXLIST gadget_capsule_texlist = {
#include "stg22/gadget_capsule_art/gadget_capsule_texlist.inc"
};
static Sint16 gadget_capsule_polygons[193] = {
#include "stg22/gadget_capsule_art/gadget_capsule_polygons.inc"
};
static Sint32 gadget_capsule_vertices[179] = {
#include "stg22/gadget_capsule_art/gadget_capsule_vertices.inc"
};
static NJS_CNK_MODEL gadget_capsule_mesh = {
#include "stg22/gadget_capsule_art/gadget_capsule_mesh.inc"
};
NJS_CNK_OBJECT gadget_capsule_object[1] = {
#include "stg22/gadget_capsule_art/gadget_capsule_object.inc"
};

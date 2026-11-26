/* STG22.PRS's texture lists, chunk models and texture names, from the disc.
 * its C lies between machine.c and capsule.c, and its D lies between ditch.c
 * and machine.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

#include "stg22/gadget_machine_art/text.inc"

static NJS_TEXNAME textures[5] = {
#include "stg22/gadget_machine_art/textures.inc"
};
NJS_TEXLIST gadget_machine_texlist = {
#include "stg22/gadget_machine_art/gadget_machine_texlist.inc"
};
static Sint16 gadget_machine_object_node_6_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_6_polygons.inc"
};
static Sint32 gadget_machine_object_node_6_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_6_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_6_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_6_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_6 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_6.inc"
};
static Sint16 gadget_machine_object_node_5_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_5_polygons.inc"
};
static Sint32 gadget_machine_object_node_5_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_5_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_5_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_5_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_5 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_5.inc"
};
static Sint16 gadget_machine_object_node_4_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_4_polygons.inc"
};
static Sint32 gadget_machine_object_node_4_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_4_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_4_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_4_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_4 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_4.inc"
};
static Sint16 gadget_machine_object_node_3_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_3_polygons.inc"
};
static Sint32 gadget_machine_object_node_3_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_3_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_3_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_3_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_3 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_3.inc"
};
static Sint16 gadget_machine_object_node_2_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_2_polygons.inc"
};
static Sint32 gadget_machine_object_node_2_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_2_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_2_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_2_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_2 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_2.inc"
};
static Sint16 gadget_machine_object_node_1_polygons[237] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_1_polygons.inc"
};
static Sint32 gadget_machine_object_node_1_vertices[211] = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_1_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_object_node_1_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_1_mesh.inc"
};
static NJS_CNK_OBJECT gadget_machine_object_node_1 = {
#include "stg22/gadget_machine_art/gadget_machine_object_node_1.inc"
};
static Sint16 gadget_machine_polygons[247] = {
#include "stg22/gadget_machine_art/gadget_machine_polygons.inc"
};
static Sint32 gadget_machine_vertices[291] = {
#include "stg22/gadget_machine_art/gadget_machine_vertices.inc"
};
static NJS_CNK_MODEL gadget_machine_mesh = {
#include "stg22/gadget_machine_art/gadget_machine_mesh.inc"
};
NJS_CNK_OBJECT gadget_machine_object = {
#include "stg22/gadget_machine_art/gadget_machine_object.inc"
};

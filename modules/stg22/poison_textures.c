#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/poison.h"
#include "stg22/poison_materials.h"

const struct poison_material_names gadget_poison_materials = {
#include "stg22/poison_textures/gadget_poison_materials.inc"
};
static const char animation[13][16] = {
#include "stg22/poison_textures/animation.inc"
};

static NJS_TEXNAME textures[17] = {
#include "stg22/poison_textures/textures.inc"
};
NJS_TEXLIST gadget_poison_texlist = {
#include "stg22/poison_textures/gadget_poison_texlist.inc"
};

/* Poison tank body and glass shell. */

static Sint16 gadget_poison_tank_node_1_polygons[391] = {
#include "stg22/poison_textures/gadget_poison_tank_node_1_polygons.inc"
};

static Sint32 gadget_poison_tank_node_1_vertices[279] = {
#include "stg22/poison_textures/gadget_poison_tank_node_1_vertices.inc"
};

static NJS_CNK_MODEL gadget_poison_tank_node_1_mesh = {
#include "stg22/poison_textures/gadget_poison_tank_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gadget_poison_tank_node_1 = {
#include "stg22/poison_textures/gadget_poison_tank_node_1.inc"
};

static Sint16 gadget_poison_tank_polygons[25] = {
#include "stg22/poison_textures/gadget_poison_tank_polygons.inc"
};

static Sint32 gadget_poison_tank_vertices[19] = {
#include "stg22/poison_textures/gadget_poison_tank_vertices.inc"
};

static NJS_CNK_MODEL gadget_poison_tank_mesh = {
#include "stg22/poison_textures/gadget_poison_tank_mesh.inc"
};

NJS_CNK_OBJECT gadget_poison_tank[1] = {
#include "stg22/poison_textures/gadget_poison_tank.inc"
};

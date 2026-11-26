#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/poison_materials.h"
#include "stg22/prop_textures.h"

static NJS_TEXNAME textures[3] = {
#include "stg22/poison_body_tex/textures.inc"
};
NJS_TEXLIST gadget_poison_body_texlist = {
#include "stg22/poison_body_tex/gadget_poison_body_texlist.inc"
};

/* Debris from the poison tank body. */

static Sint16 gadget_poison_body_fragment_polygons[365] = {
#include "stg22/poison_body_tex/gadget_poison_body_fragment_polygons.inc"
};

static Sint32 gadget_poison_body_fragment_vertices[243] = {
#include "stg22/poison_body_tex/gadget_poison_body_fragment_vertices.inc"
};

NJS_CNK_MODEL gadget_poison_body_fragment = {
#include "stg22/poison_body_tex/gadget_poison_body_fragment.inc"
};

static NJS_CNK_OBJECT gadget_poison_body_fragment_object = {
#include "stg22/poison_body_tex/gadget_poison_body_fragment_object.inc"
};

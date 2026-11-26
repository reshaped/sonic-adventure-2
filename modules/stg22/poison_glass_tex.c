#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/poison_materials.h"
#include "stg22/prop_textures.h"

static NJS_TEXNAME textures[1] = {
#include "stg22/poison_glass_tex/textures.inc"
};
NJS_TEXLIST gadget_poison_glass_texlist = {
#include "stg22/poison_glass_tex/gadget_poison_glass_texlist.inc"
};

/* The glass shard from a broken poison tank. */

static Sint16 gadget_poison_glass_fragment_polygons[35] = {
#include "stg22/poison_glass_tex/gadget_poison_glass_fragment_polygons.inc"
};

static Sint32 gadget_poison_glass_fragment_vertices[31] = {
#include "stg22/poison_glass_tex/gadget_poison_glass_fragment_vertices.inc"
};

NJS_CNK_MODEL gadget_poison_glass_fragment = {
#include "stg22/poison_glass_tex/gadget_poison_glass_fragment.inc"
};

static NJS_CNK_OBJECT gadget_poison_glass_fragment_object = {
#include "stg22/poison_glass_tex/gadget_poison_glass_fragment_object.inc"
};

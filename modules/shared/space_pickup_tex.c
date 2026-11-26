#include <ninja.h>
#include <sg_xpt.h>

#include "shared/space_pickup_tex/text.inc"

#include "shared/space_pickup.h"

static NJS_TEXNAME first[1] = {
#include "shared/space_pickup_tex/first.inc"
};
NJS_TEXLIST space_pickup_first_texlist = {
#include "shared/space_pickup_tex/space_pickup_first_texlist.inc"
};
static NJS_TEXNAME second[1] = {
#include "shared/space_pickup_tex/second.inc"
};
NJS_TEXLIST space_pickup_second_texlist = {
#include "shared/space_pickup_tex/space_pickup_second_texlist.inc"
};
static NJS_TEXNAME third[1] = {
#include "shared/space_pickup_tex/third.inc"
};
NJS_TEXLIST space_pickup_third_texlist = {
#include "shared/space_pickup_tex/space_pickup_third_texlist.inc"
};

static Sint16 pickup_polygons[77] = {
#include "shared/space_pickup_tex/pickup_polygons.inc"
};

static Sint32 pickup_vertices[59] = {
#include "shared/space_pickup_tex/pickup_vertices.inc"
};

NJS_CNK_MODEL space_pickup_model = {
#include "shared/space_pickup_tex/space_pickup_model.inc"
};

static NJS_CNK_OBJECT pickup_object = {
#include "shared/space_pickup_tex/pickup_object.inc"
};

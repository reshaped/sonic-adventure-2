#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fan_textures/text.inc"
#include "stg08/fixture_art.h"
#include "stg08/prop_textures.h"

static NJS_TEXNAME textures[4] = {
#include "stg08/fan_textures/textures.inc"
};
NJS_TEXLIST security_fan_texlist = {
#include "stg08/fan_textures/security_fan_texlist.inc"
};

static Sint16 security_fan_object_polygons[377] = {
#include "stg08/fan_textures/security_fan_object_polygons.inc"
};

static Sint32 security_fan_object_vertices[247] = {
#include "stg08/fan_textures/security_fan_object_vertices.inc"
};

static NJS_CNK_MODEL security_fan_object_model = {
#include "stg08/fan_textures/security_fan_object_model.inc"
};

NJS_CNK_OBJECT security_fan_object[1] = {
#include "stg08/fan_textures/security_fan_object.inc"
};

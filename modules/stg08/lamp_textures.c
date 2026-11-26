#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"
#include "stg08/lamp.h"
#include "stg08/lamp_textures/text.inc"

static NJS_TEXNAME textures[2] = {
#include "stg08/lamp_textures/textures.inc"
};
NJS_TEXLIST security_lamp_texlist = {
#include "stg08/lamp_textures/security_lamp_texlist.inc"
};

static Sint16 security_lamp_lens_polygons[25] = {
#include "stg08/lamp_textures/security_lamp_lens_polygons.inc"
};

static Sint32 security_lamp_lens_vertices[19] = {
#include "stg08/lamp_textures/security_lamp_lens_vertices.inc"
};

static NJS_CNK_MODEL security_lamp_lens_model = {
#include "stg08/lamp_textures/security_lamp_lens_model.inc"
};

static NJS_CNK_OBJECT security_lamp_lens = {
#include "stg08/lamp_textures/security_lamp_lens.inc"
};

static Sint16 security_lamp_object_polygons[87] = {
#include "stg08/lamp_textures/security_lamp_object_polygons.inc"
};

static Sint32 security_lamp_object_vertices[79] = {
#include "stg08/lamp_textures/security_lamp_object_vertices.inc"
};

static NJS_CNK_MODEL security_lamp_housing_model = {
#include "stg08/lamp_textures/security_lamp_housing_model.inc"
};

NJS_CNK_OBJECT security_lamp_object[1] = {
#include "stg08/lamp_textures/security_lamp_object.inc"
};

#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"
#include "stg08/panel_c_textures/text.inc"
#include "stg08/prop_textures.h"

static NJS_TEXNAME textures[3] = {
#include "stg08/panel_c_textures/textures.inc"
};
NJS_TEXLIST security_panel_c_texlist = {
#include "stg08/panel_c_textures/security_panel_c_texlist.inc"
};

static Sint16 security_panel_c_object_polygons[257] = {
#include "stg08/panel_c_textures/security_panel_c_object_polygons.inc"
};

static Sint32 security_panel_c_object_vertices[243] = {
#include "stg08/panel_c_textures/security_panel_c_object_vertices.inc"
};

static NJS_CNK_MODEL security_panel_c_object_model = {
#include "stg08/panel_c_textures/security_panel_c_object_model.inc"
};

NJS_CNK_OBJECT security_panel_c_object = {
#include "stg08/panel_c_textures/security_panel_c_object.inc"
};

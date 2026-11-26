#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"
#include "stg08/prop_textures.h"

const char security_panel_face[16] = {
#include "stg08/panel_a_textures/security_panel_face.inc"
};
static const char area_a[12] = {
#include "stg08/panel_a_textures/area_a.inc"
};
const char security_panel_border[12] = {
#include "stg08/panel_a_textures/security_panel_border.inc"
};

static NJS_TEXNAME textures[3] = {
#include "stg08/panel_a_textures/textures.inc"
};
NJS_TEXLIST security_panel_a_texlist = {
#include "stg08/panel_a_textures/security_panel_a_texlist.inc"
};

static Sint16 security_panel_a_object_polygons[229] = {
#include "stg08/panel_a_textures/security_panel_a_object_polygons.inc"
};

static Sint32 security_panel_a_object_vertices[223] = {
#include "stg08/panel_a_textures/security_panel_a_object_vertices.inc"
};

static NJS_CNK_MODEL security_panel_a_object_model = {
#include "stg08/panel_a_textures/security_panel_a_object_model.inc"
};

NJS_CNK_OBJECT security_panel_a_object = {
#include "stg08/panel_a_textures/security_panel_a_object.inc"
};

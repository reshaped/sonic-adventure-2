#include <ninja.h>
#include <sg_xpt.h>

#include "stg08/fixture_art.h"
#include "stg08/money_texture/text.inc"
#include "stg08/paper.h"

static NJS_TEXNAME texture[1] = {
#include "stg08/money_texture/texture.inc"
};
NJS_TEXLIST security_money_texlist = {
#include "stg08/money_texture/security_money_texlist.inc"
};

static Sint16 security_money_object_polygons[29] = {
#include "stg08/money_texture/security_money_object_polygons.inc"
};

static Sint32 security_money_object_vertices[23] = {
#include "stg08/money_texture/security_money_object_vertices.inc"
};

static NJS_CNK_MODEL security_money_object_model = {
#include "stg08/money_texture/security_money_object_model.inc"
};

NJS_CNK_OBJECT security_money_object[1] = {
#include "stg08/money_texture/security_money_object.inc"
};

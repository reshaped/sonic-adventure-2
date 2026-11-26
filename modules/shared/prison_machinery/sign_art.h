#ifndef SHARED_PRISON_MACHINERY_SIGN_ART_H
#define SHARED_PRISON_MACHINERY_SIGN_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME sign_names[2] = {
#include "shared/prison_machinery/sign_art/sign_names.inc"
};

static NJS_TEXLIST sign_texlist = {
#include "shared/prison_machinery/sign_art/sign_texlist.inc"
};

static Sint16 sign_panel_polygons[49] = {
#include "shared/prison_machinery/sign_art/sign_panel_polygons.inc"
};

static Sint32 sign_panel_vertices[31] = {
#include "shared/prison_machinery/sign_art/sign_panel_vertices.inc"
};

static NJS_CNK_MODEL sign_panel_model = {
#include "shared/prison_machinery/sign_art/sign_panel_model.inc"
};

static NJS_CNK_OBJECT sign_panel = {
#include "shared/prison_machinery/sign_art/sign_panel.inc"
};

static Sint16 sign_polygons[103] = {
#include "shared/prison_machinery/sign_art/sign_polygons.inc"
};

static Sint32 sign_vertices[63] = {
#include "shared/prison_machinery/sign_art/sign_vertices.inc"
};

static NJS_CNK_MODEL sign_model = {
#include "shared/prison_machinery/sign_art/sign_model.inc"
};

static NJS_CNK_OBJECT sign_object = {
#include "shared/prison_machinery/sign_art/sign_object.inc"
};

#endif

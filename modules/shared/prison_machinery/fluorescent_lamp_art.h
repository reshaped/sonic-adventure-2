#ifndef SHARED_PRISON_MACHINERY_FLUORESCENT_LAMP_ART_H
#define SHARED_PRISON_MACHINERY_FLUORESCENT_LAMP_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME fluorescent_lamp_names[4] = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_names.inc"
};

static NJS_TEXLIST fluorescent_lamp_texlist = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_texlist.inc"
};

static Sint16 fluorescent_lamp_tube_polygons[81] = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_tube_polygons.inc"
};

static Sint32 fluorescent_lamp_tube_vertices[87] = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_tube_vertices.inc"
};

static NJS_CNK_MODEL fluorescent_lamp_tube_model = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_tube_model.inc"
};

static NJS_CNK_OBJECT fluorescent_lamp_tube = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_tube.inc"
};

static Sint16 fluorescent_lamp_polygons[355] = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_polygons.inc"
};

static Sint32 fluorescent_lamp_vertices[379] = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_vertices.inc"
};

static NJS_CNK_MODEL fluorescent_lamp_model = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_model.inc"
};

static NJS_CNK_OBJECT fluorescent_lamp_object = {
#include "shared/prison_machinery/fluorescent_lamp_art/fluorescent_lamp_object.inc"
};

#endif

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/colony_shutter_textures.h"
#include "shared/shutter_art.h"

static NJS_TEXNAME textures[6] = {
#include "shared/shutter_far_tex/textures.inc"
};

NJS_TEXLIST core_shutter_far_texlist = {
#include "shared/shutter_far_tex/core_shutter_far_texlist.inc"
};

static Sint16 core_shutter_far_closed_lamp_polygons[15] = {
#include "shared/shutter_far_tex/core_shutter_far_closed_lamp_polygons.inc"
};

static Sint32 core_shutter_far_closed_lamp_vertices[19] = {
#include "shared/shutter_far_tex/core_shutter_far_closed_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_far_closed_lamp_model = {
#include "shared/shutter_far_tex/core_shutter_far_closed_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_far_closed_lamp[1] = {
#include "shared/shutter_far_tex/core_shutter_far_closed_lamp.inc"
};

static Sint16 core_shutter_far_lamp_polygons[25] = {
#include "shared/shutter_far_tex/core_shutter_far_lamp_polygons.inc"
};

static Sint32 core_shutter_far_lamp_vertices[19] = {
#include "shared/shutter_far_tex/core_shutter_far_lamp_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_far_lamp_model = {
#include "shared/shutter_far_tex/core_shutter_far_lamp_model.inc"
};

NJS_CNK_OBJECT core_shutter_far_lamp[1] = {
#include "shared/shutter_far_tex/core_shutter_far_lamp.inc"
};

static Sint16 core_shutter_far_face_polygons[475] = {
#include "shared/shutter_far_tex/core_shutter_far_face_polygons.inc"
};

static Sint32 core_shutter_far_face_vertices[347] = {
#include "shared/shutter_far_tex/core_shutter_far_face_vertices.inc"
};

static NJS_CNK_MODEL core_shutter_far_face_model = {
#include "shared/shutter_far_tex/core_shutter_far_face_model.inc"
};

NJS_CNK_OBJECT core_shutter_far_face[1] = {
#include "shared/shutter_far_tex/core_shutter_far_face.inc"
};

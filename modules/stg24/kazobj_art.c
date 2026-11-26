/* Eternal Engine's KAZOBJ models, 0 to 3 and 5 to 7, each with its texture
 * list */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg24/scenery_art.h"
#include "shared/kazobj.h"

#include "stg24/kazobj_art/text.inc"

/* the set editor's labels for a KAZOBJ's kind and ang.y, which no code reads */
static const char editor_model[16] = "<- MODEL(0-  )";
static const char editor_angle_y[12] = "<- ANGLE Y";

static NJS_TEXNAME kazobj_texlist_0_names[1] = {
#include "stg24/kazobj_art/kazobj_texlist_0_names.inc"
};

NJS_TEXLIST kazobj_texlist_0 = {
#include "stg24/kazobj_art/kazobj_texlist_0.inc"
};

static Sint16 kazobj_model_0_polygons[57] = {
#include "stg24/kazobj_art/kazobj_model_0_polygons.inc"
};

static Sint32 kazobj_model_0_vertices[35] = {
#include "stg24/kazobj_art/kazobj_model_0_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_0_model = {
#include "stg24/kazobj_art/kazobj_model_0_model.inc"
};

NJS_CNK_OBJECT kazobj_model_0 = {
#include "stg24/kazobj_art/kazobj_model_0.inc"
};

static NJS_TEXNAME kazobj_texlist_1_names[1] = {
#include "stg24/kazobj_art/kazobj_texlist_1_names.inc"
};

NJS_TEXLIST kazobj_texlist_1 = {
#include "stg24/kazobj_art/kazobj_texlist_1.inc"
};

static Sint16 kazobj_model_1_polygons[57] = {
#include "stg24/kazobj_art/kazobj_model_1_polygons.inc"
};

static Sint32 kazobj_model_1_vertices[35] = {
#include "stg24/kazobj_art/kazobj_model_1_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_1_model = {
#include "stg24/kazobj_art/kazobj_model_1_model.inc"
};

NJS_CNK_OBJECT kazobj_model_1 = {
#include "stg24/kazobj_art/kazobj_model_1.inc"
};

static NJS_TEXNAME kazobj_texlist_2_names[1] = {
#include "stg24/kazobj_art/kazobj_texlist_2_names.inc"
};

NJS_TEXLIST kazobj_texlist_2 = {
#include "stg24/kazobj_art/kazobj_texlist_2.inc"
};

static Sint16 kazobj_model_2_polygons[99] = {
#include "stg24/kazobj_art/kazobj_model_2_polygons.inc"
};

static Sint32 kazobj_model_2_vertices[67] = {
#include "stg24/kazobj_art/kazobj_model_2_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_2_model = {
#include "stg24/kazobj_art/kazobj_model_2_model.inc"
};

NJS_CNK_OBJECT kazobj_model_2 = {
#include "stg24/kazobj_art/kazobj_model_2.inc"
};

static NJS_TEXNAME kazobj_texlist_3_names[1] = {
#include "stg24/kazobj_art/kazobj_texlist_3_names.inc"
};

NJS_TEXLIST kazobj_texlist_3 = {
#include "stg24/kazobj_art/kazobj_texlist_3.inc"
};

static Sint16 kazobj_model_3_polygons[113] = {
#include "stg24/kazobj_art/kazobj_model_3_polygons.inc"
};

static Sint32 kazobj_model_3_vertices[67] = {
#include "stg24/kazobj_art/kazobj_model_3_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_3_model = {
#include "stg24/kazobj_art/kazobj_model_3_model.inc"
};

NJS_CNK_OBJECT kazobj_model_3 = {
#include "stg24/kazobj_art/kazobj_model_3.inc"
};

static NJS_TEXNAME kazobj_texlist_5_names[1] = {
#include "stg24/kazobj_art/kazobj_texlist_5_names.inc"
};

NJS_TEXLIST kazobj_texlist_5 = {
#include "stg24/kazobj_art/kazobj_texlist_5.inc"
};

static Sint16 kazobj_model_5_polygons[117] = {
#include "stg24/kazobj_art/kazobj_model_5_polygons.inc"
};

static Sint32 kazobj_model_5_vertices[131] = {
#include "stg24/kazobj_art/kazobj_model_5_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_5_model = {
#include "stg24/kazobj_art/kazobj_model_5_model.inc"
};

NJS_CNK_OBJECT kazobj_model_5 = {
#include "stg24/kazobj_art/kazobj_model_5.inc"
};

static NJS_TEXNAME kazobj_texlist_6_names[3] = {
#include "stg24/kazobj_art/kazobj_texlist_6_names.inc"
};

NJS_TEXLIST kazobj_texlist_6 = {
#include "stg24/kazobj_art/kazobj_texlist_6.inc"
};

static Sint16 kazobj_model_6_polygons[89] = {
#include "stg24/kazobj_art/kazobj_model_6_polygons.inc"
};

static Sint32 kazobj_model_6_vertices[59] = {
#include "stg24/kazobj_art/kazobj_model_6_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_6_model = {
#include "stg24/kazobj_art/kazobj_model_6_model.inc"
};

NJS_CNK_OBJECT kazobj_model_6 = {
#include "stg24/kazobj_art/kazobj_model_6.inc"
};

static NJS_TEXNAME kazobj_texlist_7_names[4] = {
#include "stg24/kazobj_art/kazobj_texlist_7_names.inc"
};

NJS_TEXLIST kazobj_texlist_7 = {
#include "stg24/kazobj_art/kazobj_texlist_7.inc"
};

static Sint16 kazobj_model_7_polygons[169] = {
#include "stg24/kazobj_art/kazobj_model_7_polygons.inc"
};

static Sint32 kazobj_model_7_vertices[179] = {
#include "stg24/kazobj_art/kazobj_model_7_vertices.inc"
};

static NJS_CNK_MODEL kazobj_model_7_model = {
#include "stg24/kazobj_art/kazobj_model_7_model.inc"
};

NJS_CNK_OBJECT kazobj_model_7 = {
#include "stg24/kazobj_art/kazobj_model_7.inc"
};

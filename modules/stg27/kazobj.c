#include <ninja.h>
#include <sg_xpt.h>

#include "stg27/kazobj/text.inc"
#include "stg27/object_archives.h"
#include "shared/kazobj.h"

/* the set editor's labels for a model's placement, which no code reads */
static const char editor_model[16] = "<- MODEL(0-  )";
static const char editor_angle_y[12] = "<- ANGLE Y";

/* Kazobj 0 geometry. */

static NJS_TEXNAME kazobj_0_names[1] = {
#include "stg27/kazobj/kazobj_0_names.inc"
};

NJS_TEXLIST kazobj_texlist_0 = {
#include "stg27/kazobj/kazobj_texlist_0.inc"
};

static Sint16 kazobj_0_polygons[117] = {
#include "stg27/kazobj/kazobj_0_polygons.inc"
};

static Sint32 kazobj_0_vertices[131] = {
#include "stg27/kazobj/kazobj_0_vertices.inc"
};

static NJS_CNK_MODEL kazobj_0_model = {
#include "stg27/kazobj/kazobj_0_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_0 = {
#include "stg27/kazobj/kazobj_model_0.inc"
};

/* Kazobj 1 geometry. */

static NJS_TEXNAME kazobj_1_names[3] = {
#include "stg27/kazobj/kazobj_1_names.inc"
};

NJS_TEXLIST kazobj_texlist_1 = {
#include "stg27/kazobj/kazobj_texlist_1.inc"
};

static Sint16 kazobj_1_polygons[89] = {
#include "stg27/kazobj/kazobj_1_polygons.inc"
};

static Sint32 kazobj_1_vertices[59] = {
#include "stg27/kazobj/kazobj_1_vertices.inc"
};

static NJS_CNK_MODEL kazobj_1_model = {
#include "stg27/kazobj/kazobj_1_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_1 = {
#include "stg27/kazobj/kazobj_model_1.inc"
};

/* Kazobj 2 geometry. */

static NJS_TEXNAME kazobj_2_names[4] = {
#include "stg27/kazobj/kazobj_2_names.inc"
};

NJS_TEXLIST kazobj_texlist_2 = {
#include "stg27/kazobj/kazobj_texlist_2.inc"
};

static Sint16 kazobj_2_polygons[169] = {
#include "stg27/kazobj/kazobj_2_polygons.inc"
};

static Sint32 kazobj_2_vertices[179] = {
#include "stg27/kazobj/kazobj_2_vertices.inc"
};

static NJS_CNK_MODEL kazobj_2_model = {
#include "stg27/kazobj/kazobj_2_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_2 = {
#include "stg27/kazobj/kazobj_model_2.inc"
};

/* Kazobj 3 geometry. */

static NJS_TEXNAME kazobj_3_names[4] = {
#include "stg27/kazobj/kazobj_3_names.inc"
};

NJS_TEXLIST kazobj_texlist_3 = {
#include "stg27/kazobj/kazobj_texlist_3.inc"
};

static Sint16 kazobj_3_polygons[191] = {
#include "stg27/kazobj/kazobj_3_polygons.inc"
};

static Sint32 kazobj_3_vertices[151] = {
#include "stg27/kazobj/kazobj_3_vertices.inc"
};

static NJS_CNK_MODEL kazobj_3_model = {
#include "stg27/kazobj/kazobj_3_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_3 = {
#include "stg27/kazobj/kazobj_model_3.inc"
};

/* Kazobj 4 geometry. */

static NJS_TEXNAME kazobj_4_names[4] = {
#include "stg27/kazobj/kazobj_4_names.inc"
};

NJS_TEXLIST kazobj_texlist_4 = {
#include "stg27/kazobj/kazobj_texlist_4.inc"
};

static Sint16 kazobj_4_polygons[329] = {
#include "stg27/kazobj/kazobj_4_polygons.inc"
};

static Sint32 kazobj_4_vertices[291] = {
#include "stg27/kazobj/kazobj_4_vertices.inc"
};

static NJS_CNK_MODEL kazobj_4_model = {
#include "stg27/kazobj/kazobj_4_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_4 = {
#include "stg27/kazobj/kazobj_model_4.inc"
};

/* Kazobj 5 geometry. */

static NJS_TEXNAME kazobj_5_names[4] = {
#include "stg27/kazobj/kazobj_5_names.inc"
};

NJS_TEXLIST kazobj_texlist_5 = {
#include "stg27/kazobj/kazobj_texlist_5.inc"
};

static Sint16 kazobj_5_polygons[393] = {
#include "stg27/kazobj/kazobj_5_polygons.inc"
};

static Sint32 kazobj_5_vertices[403] = {
#include "stg27/kazobj/kazobj_5_vertices.inc"
};

static NJS_CNK_MODEL kazobj_5_model = {
#include "stg27/kazobj/kazobj_5_model.inc"
};

static NJS_CNK_OBJECT kazobj_model_5 = {
#include "stg27/kazobj/kazobj_model_5.inc"
};

NJS_TEXLIST *colony_kazobj_texlists[7] = {
#include "stg27/kazobj/colony_kazobj_texlists.inc"
};

/* Lost Colony's six kinds, the last three built and drawn with fog off */
#define COLONY_KAZOBJ
#include "shared/kazobj.c" /* NOLINT(bugprone-suspicious-include) */

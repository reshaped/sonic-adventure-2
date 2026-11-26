#include <ninja.h>
#include <sg_xpt.h>

#include "shared/machinery_assets.h"
#include "stg26/machinery.h"

static Sint16 machinery_curve_polygons[661] = {
#include "stg26/machinery/machinery_curve_polygons.inc"
};

static Sint32 machinery_curve_vertices[563] = {
#include "stg26/machinery/machinery_curve_vertices.inc"
};

NJS_CNK_MODEL machinery_curve_model = {
#include "stg26/machinery/machinery_curve_model.inc"
};

static NJS_CNK_OBJECT machinery_curve_object = {
#include "stg26/machinery/machinery_curve_object.inc"
};

static NJS_TEXNAME straight_textures[3] = {
#include "stg26/machinery/straight_textures.inc"
};
NJS_TEXLIST machinery_straight_texlist = {
#include "stg26/machinery/machinery_straight_texlist.inc"
};

static Sint16 machinery_straight_polygons[477] = {
#include "stg26/machinery/machinery_straight_polygons.inc"
};

static Sint32 machinery_straight_vertices[419] = {
#include "stg26/machinery/machinery_straight_vertices.inc"
};

NJS_CNK_MODEL machinery_straight_model = {
#include "stg26/machinery/machinery_straight_model.inc"
};

static NJS_CNK_OBJECT machinery_straight_object = {
#include "stg26/machinery/machinery_straight_object.inc"
};

static NJS_CNK_MODEL *models[MACHINERY_KINDS] = {
#include "stg26/machinery/models.inc"
};
NJS_TEXLIST *quarters_machinery_texlists[MACHINERY_KINDS + 1] = {
#include "stg26/machinery/quarters_machinery_texlists.inc"
};

#define MACHINERY_INIT quarters_machinery_init
#define MACHINERY_TEXLISTS quarters_machinery_texlists
#define MACHINERY_BUILD quarters_machinery_build
#include "shared/machinery_prop.h" /* IWYU pragma: keep -- the code it compiles here */

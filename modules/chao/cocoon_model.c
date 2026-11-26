/* CHAO.PRS's chunk models, from the disc. its D lies between illness.c and
 * animal_actions.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/cocoon.h"

static Sint16 cocoon_model_polygons[281] = {
#include "chao/cocoon_model/cocoon_model_polygons.inc"
};
static Sint32 cocoon_model_vertices[207] = {
#include "chao/cocoon_model/cocoon_model_vertices.inc"
};
static NJS_CNK_MODEL cocoon_mesh = {
#include "chao/cocoon_model/cocoon_mesh.inc"
};
NJS_CNK_OBJECT cocoon_model = {
#include "chao/cocoon_model/cocoon_model.inc"
};

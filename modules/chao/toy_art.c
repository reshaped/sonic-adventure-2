/* CHAO.PRS's chunk models, from the disc. its D lies between ground.c and
 * dat_0c569760.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/toy_art.h"

static Sint16 shadow_triangles[89] = {
#include "chao/toy_art/shadow_triangles.inc"
};
static Sint32 shadow_vertices[51] = {
#include "chao/toy_art/shadow_vertices.inc"
};
NJS_CNK_MODEL garden_shadow_model = {
#include "chao/toy_art/garden_shadow_model.inc"
};
NJS_CNK_OBJECT garden_shadow_object = {
#include "chao/toy_art/garden_shadow_object.inc"
};

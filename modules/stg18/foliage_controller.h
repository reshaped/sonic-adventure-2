/* the sasa_ data that stg18/sasa.c and stg18/flower.c each define for
 * stg18/foliage_impl.h: the motions, shapes, names and builds of the plant's
 * bright and dark kinds, and the plant's size and volume. SASA_HEIGHT and
 * SASA_RADIUS hold SASA's size unless the includer defines them first */
#ifndef STG18_FOLIAGE_CONTROLLER_H
#define STG18_FOLIAGE_CONTROLLER_H

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"

#ifndef SASA_HEIGHT
#define SASA_HEIGHT 14.0F
#endif
#ifndef SASA_RADIUS
#define SASA_RADIUS 4.0F
#endif

static NJS_MOTION *sasa_motions[2] = {
#include "stg18/foliage_controller/sasa_motions.inc"
};
static NJS_MOTION *sasa_shapes[2] = {
#include "stg18/foliage_controller/sasa_shapes.inc"
};
static char *sasa_kind_names[2] = {
#include "stg18/foliage_controller/sasa_kind_names.inc"
};
static Float sasa_height = SASA_HEIGHT;
static Float sasa_radius = SASA_RADIUS;
static Sint32 sasa_volume = 50;
static Bool sasa_built = FALSE;
static struct model_build sasa_builds[2] = {
#include "stg18/foliage_controller/sasa_builds.inc"
};

#endif

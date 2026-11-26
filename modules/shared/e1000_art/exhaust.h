/* the E-1000's exhaust, which e1000_draw draws at e1000_flight_node's matrix
 * while the E-1000 flies. the draw writes each u and v of the exhaust's strip,
 * to scroll its texture */
#ifndef SHARED_E1000_ART_EXHAUST_H
#define SHARED_E1000_ART_EXHAUST_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 e1000_exhaust_polygons[43] = {
#include "shared/e1000_art/exhaust/e1000_exhaust_polygons.inc"
};

static Sint32 e1000_exhaust_vertices[43] = {
#include "shared/e1000_art/exhaust/e1000_exhaust_vertices.inc"
};

static NJS_CNK_MODEL e1000_exhaust_model = {
#include "shared/e1000_art/exhaust/e1000_exhaust_model.inc"
};

/* the node over the exhaust's model, whose model alone e1000_draw reads */
/* @hack type: the node declared as an array of one, where the node alone would
 * do */
static NJS_CNK_OBJECT e1000_flight_object[1] = {
#include "shared/e1000_art/exhaust/e1000_flight_object.inc"
};

#endif

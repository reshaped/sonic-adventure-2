/* The warning sign frame uses the local sign face in texture slot 2. */
#ifndef SHARED_ROAD_FURNITURE_WARNING_SIGN_BODY_H
#define SHARED_ROAD_FURNITURE_WARNING_SIGN_BODY_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 warning_sign_body_polygons[235] = {
#include "shared/road_furniture/warning_sign_body/warning_sign_body_polygons.inc"
};

static Sint32 warning_sign_body_vertices[247] = {
#include "shared/road_furniture/warning_sign_body/warning_sign_body_vertices.inc"
};

static NJS_CNK_MODEL warning_sign_body_mesh = {
#include "shared/road_furniture/warning_sign_body/warning_sign_body_mesh.inc"
};

#endif

#ifndef SHARED_CAMERA_ART_VERTICAL_CAMERA_LIGHT_H
#define SHARED_CAMERA_ART_VERTICAL_CAMERA_LIGHT_H

/* The vertical camera projects this light from its head. */
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 vertical_camera_light_polygons[25] = {
#include "shared/camera_art/vertical_camera_light/vertical_camera_light_polygons.inc"
};

static Sint32 vertical_camera_light_vertices[19] = {
#include "shared/camera_art/vertical_camera_light/vertical_camera_light_vertices.inc"
};

static NJS_CNK_MODEL vertical_camera_light_model = {
#include "shared/camera_art/vertical_camera_light/vertical_camera_light_model.inc"
};

static NJS_CNK_OBJECT vertical_camera_light = {
#include "shared/camera_art/vertical_camera_light/vertical_camera_light.inc"
};

#endif

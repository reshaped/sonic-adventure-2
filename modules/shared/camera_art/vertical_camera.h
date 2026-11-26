#ifndef SHARED_CAMERA_ART_VERTICAL_CAMERA_H
#define SHARED_CAMERA_ART_VERTICAL_CAMERA_H

/* The camera base and its movable head. */
#include <ninja.h>
#include <sg_xpt.h>

static Sint32 vertical_camera_head_vertices[211] = {
#include "shared/camera_art/vertical_camera/vertical_camera_head_vertices.inc"
};

static NJS_CNK_MODEL vertical_camera_head_model = {
#include "shared/camera_art/vertical_camera/vertical_camera_head_model.inc"
};

static NJS_CNK_OBJECT vertical_camera_head = {
#include "shared/camera_art/vertical_camera/vertical_camera_head.inc"
};

static Sint16 vertical_camera_polygons[233] = {
#include "shared/camera_art/vertical_camera/vertical_camera_polygons.inc"
};

static Sint32 vertical_camera_vertices[195] = {
#include "shared/camera_art/vertical_camera/vertical_camera_vertices.inc"
};

static NJS_CNK_MODEL vertical_camera_model = {
#include "shared/camera_art/vertical_camera/vertical_camera_model.inc"
};

#endif

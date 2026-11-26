#ifndef SHARED_SECURITY_CAMERA_ART_H
#define SHARED_SECURITY_CAMERA_ART_H
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME security_camera_texlist_names[4] = {
#include "shared/security_camera_art/security_camera_texlist_names.inc"
};

static NJS_TEXLIST security_camera_texlist = {
#include "shared/security_camera_art/security_camera_texlist.inc"
};

static Sint16 security_camera_object_child_polygons[329] = {
#include "shared/security_camera_art/security_camera_object_child_polygons.inc"
};

static Sint32 security_camera_object_child_vertices[343] = {
#include "shared/security_camera_art/security_camera_object_child_vertices.inc"
};

static NJS_CNK_MODEL security_camera_object_child_model = {
#include "shared/security_camera_art/security_camera_object_child_model.inc"
};

static NJS_CNK_OBJECT security_camera_object_child = {
#include "shared/security_camera_art/security_camera_object_child.inc"
};

static Sint16 security_camera_polygons[103] = {
#include "shared/security_camera_art/security_camera_polygons.inc"
};

static Sint32 security_camera_vertices[83] = {
#include "shared/security_camera_art/security_camera_vertices.inc"
};

static NJS_CNK_MODEL security_camera_model = {
#include "shared/security_camera_art/security_camera_model.inc"
};

static NJS_CNK_OBJECT security_camera_object[1] = {
#include "shared/security_camera_art/security_camera_object.inc"
};

#endif

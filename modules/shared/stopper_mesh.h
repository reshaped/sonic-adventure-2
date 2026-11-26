/* The blinking lamp and body drawn by the stopper. */
#ifndef SHARED_STOPPER_MESH_H
#define SHARED_STOPPER_MESH_H

#include <ninja.h>

static Sint16 lamp_polygons[43] = {
#include "shared/stopper_mesh/lamp_polygons.inc"
};

static Sint32 lamp_vertices[39] = {
#include "shared/stopper_mesh/lamp_vertices.inc"
};

static NJS_CNK_MODEL lamp_mesh = {
#include "shared/stopper_mesh/lamp_mesh.inc"
};

static NJS_CNK_OBJECT lamp_object = {
#include "shared/stopper_mesh/lamp_object.inc"
};

static Sint16 body_polygons[107] = {
#include "shared/stopper_mesh/body_polygons.inc"
};

static Sint32 body_vertices[115] = {
#include "shared/stopper_mesh/body_vertices.inc"
};

static NJS_CNK_MODEL body_mesh = {
#include "shared/stopper_mesh/body_mesh.inc"
};

#endif

/* The solid collision mesh copied by each stopper. */
#ifndef SHARED_STOPPER_SOLID_H
#define SHARED_STOPPER_SOLID_H

#include <ninja.h>

static NJS_MATERIAL stopper_material[1] = {
#include "shared/stopper_solid/stopper_material.inc"
};

static Sint16 stopper_strips[25] = {
#include "shared/stopper_solid/stopper_strips.inc"
};

static NJS_MESHSET stopper_faces[1] = {
#include "shared/stopper_solid/stopper_faces.inc"
};

static NJS_POINT3 stopper_points[8] = {
#include "shared/stopper_solid/stopper_points.inc"
};

static NJS_VECTOR stopper_normals[8] = {
#include "shared/stopper_solid/stopper_normals.inc"
};

static NJS_MODEL stopper_hull = {
#include "shared/stopper_solid/stopper_hull.inc"
};

#endif

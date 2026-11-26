#ifndef SHARED_CORE_LASER_ART_H
#define SHARED_CORE_LASER_ART_H

/* Two emitters and their beam. */
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 core_laser_right_polygons[173] = {
#include "shared/core_laser_art/core_laser_right_polygons.inc"
};

static Sint32 core_laser_right_vertices[107] = {
#include "shared/core_laser_art/core_laser_right_vertices.inc"
};

static NJS_CNK_MODEL core_laser_right_model = {
#include "shared/core_laser_art/core_laser_right_model.inc"
};

static NJS_CNK_OBJECT core_laser_right[1] = {
#include "shared/core_laser_art/core_laser_right.inc"
};

static Sint16 core_laser_left_polygons[173] = {
#include "shared/core_laser_art/core_laser_left_polygons.inc"
};

static Sint32 core_laser_left_vertices[107] = {
#include "shared/core_laser_art/core_laser_left_vertices.inc"
};

static NJS_CNK_MODEL core_laser_left_model = {
#include "shared/core_laser_art/core_laser_left_model.inc"
};

static NJS_CNK_OBJECT core_laser_left[1] = {
#include "shared/core_laser_art/core_laser_left.inc"
};

static Sint16 core_laser_beam_polygons[23] = {
#include "shared/core_laser_art/core_laser_beam_polygons.inc"
};

static Sint32 core_laser_beam_vertices[43] = {
#include "shared/core_laser_art/core_laser_beam_vertices.inc"
};

static NJS_CNK_MODEL core_laser_beam_model = {
#include "shared/core_laser_art/core_laser_beam_model.inc"
};

static NJS_CNK_OBJECT core_laser_beam[1] = {
#include "shared/core_laser_art/core_laser_beam.inc"
};

#endif

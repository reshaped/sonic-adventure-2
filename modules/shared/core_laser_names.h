#ifndef SHARED_CORE_LASER_NAMES_H
#define SHARED_CORE_LASER_NAMES_H

#include <ninja.h>

/* Labels retained by the SET editor for the two modes and placement fields. */
static const char editor_wall[16] = {
#include "shared/core_laser_names/editor_wall.inc"
};
static const char editor_damage[16] = {
#include "shared/core_laser_names/editor_damage.inc"
};
static const char editor_rotation_y[12] = {
#include "shared/core_laser_names/editor_rotation_y.inc"
};
static const char editor_rotation_z[12] = {
#include "shared/core_laser_names/editor_rotation_z.inc"
};
static const char editor_size[8] = {
#include "shared/core_laser_names/editor_size.inc"
};
static const char editor_pitch[12] = {
#include "shared/core_laser_names/editor_pitch.inc"
};
static const char editor_count[8] = {
#include "shared/core_laser_names/editor_count.inc"
};

static NJS_TEXNAME core_laser_names[3] = {
#include "shared/core_laser_names/core_laser_names.inc"
};

#endif

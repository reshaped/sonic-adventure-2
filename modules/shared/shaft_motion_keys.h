#ifndef SHARED_SHAFT_MOTION_KEYS_H
#define SHARED_SHAFT_MOTION_KEYS_H
#include <ninja.h>

static NJS_MKEY_F shaft_object_child_position_keys[100] = {
#include "shared/shaft_motion_keys/shaft_object_child_position_keys.inc"
};

static NJS_MKEY_F shaft_object_child_sibling_position_keys[100] = {
#include "shared/shaft_motion_keys/shaft_object_child_sibling_position_keys.inc"
};

static NJS_MDATA3 shaft_tracks[4] = {
#include "shared/shaft_motion_keys/shaft_tracks.inc"
};

#endif

#ifndef SHARED_SPINNER_MOTION_KEYS_H
#define SHARED_SPINNER_MOTION_KEYS_H
#include <ninja.h>

static NJS_MKEY_A spinner_object_child_sibling_angle_keys[100] = {
#include "shared/spinner_motion_keys/spinner_object_child_sibling_angle_keys.inc"
};

static NJS_MDATA3 spinner_tracks[3] = {
#include "shared/spinner_motion_keys/spinner_tracks.inc"
};

#endif

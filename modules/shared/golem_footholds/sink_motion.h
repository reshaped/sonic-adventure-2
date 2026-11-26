#ifndef SHARED_GOLEM_FOOTHOLDS_SINK_MOTION_H
#define SHARED_GOLEM_FOOTHOLDS_SINK_MOTION_H

#include <ninja.h>

static NJS_MKEY_A sink_rotations[53] = {
#include "shared/golem_footholds/sink_motion/sink_rotations.inc"
};

static NJS_MDATA2 sink_tracks[1] = {
#include "shared/golem_footholds/sink_motion/sink_tracks.inc"
};

#endif

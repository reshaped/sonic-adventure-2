/* the starting grid of each of the ten tracks, by race_track: its origin and
 * its heading */
#ifndef CHAOSTGRACE_GRID_H
#define CHAOSTGRACE_GRID_H
#include <ninja.h>
#include "chao/race_record.h"

enum race_track {
    TRACK_BEG_SWIM,
    TRACK_BEG_FLY,
    TRACK_BEG_RUN,
    TRACK_BEG_POWER,
    TRACK_JWL_SWIM,
    TRACK_JWL_FLY,
    TRACK_JWL_RUN,
    TRACK_JWL_POWER,
    TRACK_JWL_IL,
    TRACK_JWL_LONG
};
struct race_grid {
    NJS_POINT3 pos;
    Angle yaw;
};
extern struct race_grid race_grids[RACE_RECORD_TRACKS];

#endif

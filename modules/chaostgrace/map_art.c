/* the course map's art, and the scale and offsets that place each racer's
 * marker on it. map_draw draws the track's map over map_all.pvr, the map of
 * every course */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race_record.h"
#include "chaostgrace/map.h"

/* the map's place on a full screen, then on each of four split screens.
 * race_map_layout_positions holds the same five. no code reads either */
static NJS_POINT2 map_layout_copy[5] = {
#include "chaostgrace/map_art/map_layout_copy.inc"
};

NJS_TEXLIST *race_map_texlist = NULL;
NJS_TEXANIM race_map_cells[2] = {
#include "chaostgrace/map_art/race_map_cells.inc"
};
Float race_map_scale = 0.112F;
Float race_map_origin_x = -260.0F;
Float race_map_origin_z = -660.0F;
Float race_map_offset_x = 5.0F;
Float race_map_offset_y = 8.0F;
NJS_POINT2 race_map_layout_positions[5] = {
#include "chaostgrace/map_art/race_map_layout_positions.inc"
};
char *race_map_tracks[RACE_RECORD_TRACKS] = {
    "map_swim_s.pvr", "map_fly_s.pvr", "map_run_s.pvr", "map_power_s.pvr",
    "map_swim.pvr",   "map_fly.pvr",   "map_run.pvr",   "map_power.pvr",
    "map_il.pvr",     "map_long.pvr"};
char *race_map_overview[] = {"map_all.pvr"};

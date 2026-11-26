/* the course map the race shows in a corner of the screen */
#ifndef CHAOSTGRACE_MAP_H
#define CHAOSTGRACE_MAP_H
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race_record.h"

/* the textures race_map_load allocates and loads, and their cells */
enum { RACE_MAP_TRACK, RACE_MAP_OVERVIEW, RACE_MAP_TEXTURES };
extern NJS_TEXLIST *race_map_texlist;
extern NJS_TEXANIM race_map_cells[RACE_MAP_TEXTURES];
/* a racer at world x and z has its marker (x - race_map_origin_x) *
 * race_map_scale + race_map_offset_x right of the map's corner, and
 * (z - race_map_origin_z) * race_map_scale + race_map_offset_y below it */
extern Float race_map_scale;
extern Float race_map_origin_x, race_map_origin_z;
extern Float race_map_offset_x, race_map_offset_y;
/* the map's place on a full screen, then on each of four split screens */
extern NJS_POINT2 race_map_layout_positions[5];
/* the maps' texture names: each track's, by race_track, and the overview */
extern char *race_map_tracks[RACE_RECORD_TRACKS];
extern char *race_map_overview[];

/* load the course's track map and the overview into race_map_texlist, unless
 * it is loaded */
void race_map_load(void);
/* make the map's task, if race_map_texlist is loaded. the map draws while the
 * race is RACE_READY or RACE_RUNNING. destroying its task frees
 * race_map_texlist */
void race_map_create(void);

#endif

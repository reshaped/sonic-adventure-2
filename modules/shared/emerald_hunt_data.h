/* the emerald hunt's data, which Aquatic Mine, Meteor Herd and Mad Space each
 * define for themselves. it holds the moving pieces' paths, the hints, and the
 * hint file loaded at the stage's start */
#ifndef SHARED_EMERALD_HUNT_DATA_H
#define SHARED_EMERALD_HUNT_DATA_H

#include "stage/hunt.h"
#include "stage/path.h"

#define HUNT_PIECE_ROUTES 8
extern struct path *hunt_piece_routes[HUNT_PIECE_ROUTES];
extern struct hunt_hint hunt_location_hints[];
extern void *hunt_stage_messages;
#endif

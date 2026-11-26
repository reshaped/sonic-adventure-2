/* STG25.PRS's paths, hunt hints, pointer slot and path list, from the disc.
 * its DATA lies between the section's start and room_scenery.c */
#include <ninja.h>

#include "stage/hunt.h"
#include "stage/path.h"
#include "stg25/hunt_routes.h"

#pragma section ATA
static NJS_POINT3 piece_route_0_points[121] = {
#include "stg25/hunt_routes/piece_route_0_points.inc"
};
static struct path piece_route_0 = {
#include "stg25/hunt_routes/piece_route_0.inc"
};
static NJS_POINT3 piece_route_1_points[51] = {
#include "stg25/hunt_routes/piece_route_1_points.inc"
};
static struct path piece_route_1 = {
#include "stg25/hunt_routes/piece_route_1.inc"
};
static NJS_POINT3 piece_route_2_points[41] = {
#include "stg25/hunt_routes/piece_route_2_points.inc"
};
static struct path piece_route_2 = {
#include "stg25/hunt_routes/piece_route_2.inc"
};
static NJS_POINT3 piece_route_3_points[161] = {
#include "stg25/hunt_routes/piece_route_3_points.inc"
};
static struct path piece_route_3 = {
#include "stg25/hunt_routes/piece_route_3.inc"
};
static NJS_POINT3 piece_route_4_points[161] = {
#include "stg25/hunt_routes/piece_route_4_points.inc"
};
static struct path piece_route_4 = {
#include "stg25/hunt_routes/piece_route_4.inc"
};
static NJS_POINT3 piece_route_5_points[41] = {
#include "stg25/hunt_routes/piece_route_5_points.inc"
};
static struct path piece_route_5 = {
#include "stg25/hunt_routes/piece_route_5.inc"
};
static NJS_POINT3 piece_route_6_points[51] = {
#include "stg25/hunt_routes/piece_route_6_points.inc"
};
static struct path piece_route_6 = {
#include "stg25/hunt_routes/piece_route_6.inc"
};
static NJS_POINT3 piece_route_7_points[81] = {
#include "stg25/hunt_routes/piece_route_7_points.inc"
};
static struct path piece_route_7 = {
#include "stg25/hunt_routes/piece_route_7.inc"
};
static NJS_POINT3 piece_route_8_points[65] = {
#include "stg25/hunt_routes/piece_route_8_points.inc"
};
static struct path piece_route_8 = {
#include "stg25/hunt_routes/piece_route_8.inc"
};
static NJS_POINT3 piece_route_9_points[129] = {
#include "stg25/hunt_routes/piece_route_9_points.inc"
};
static struct path piece_route_9 = {
#include "stg25/hunt_routes/piece_route_9.inc"
};
static NJS_POINT3 piece_route_10_points[41] = {
#include "stg25/hunt_routes/piece_route_10_points.inc"
};
static struct path piece_route_10 = {
#include "stg25/hunt_routes/piece_route_10.inc"
};
static NJS_POINT3 piece_route_11_points[151] = {
#include "stg25/hunt_routes/piece_route_11_points.inc"
};
static struct path piece_route_11 = {
#include "stg25/hunt_routes/piece_route_11.inc"
};
static NJS_POINT3 piece_route_12_points[51] = {
#include "stg25/hunt_routes/piece_route_12_points.inc"
};
static struct path piece_route_12 = {
#include "stg25/hunt_routes/piece_route_12.inc"
};
static NJS_POINT3 piece_route_13_points[101] = {
#include "stg25/hunt_routes/piece_route_13_points.inc"
};
static struct path piece_route_13 = {
#include "stg25/hunt_routes/piece_route_13.inc"
};
static NJS_POINT3 piece_route_14_points[201] = {
#include "stg25/hunt_routes/piece_route_14_points.inc"
};
static struct path piece_route_14 = {
#include "stg25/hunt_routes/piece_route_14.inc"
};
static NJS_POINT3 piece_route_15_points[241] = {
#include "stg25/hunt_routes/piece_route_15_points.inc"
};
static struct path piece_route_15 = {
#include "stg25/hunt_routes/piece_route_15.inc"
};
struct hunt_hint death_chamber_hints[149] = {
#include "stg25/hunt_routes/death_chamber_hints.inc"
};
static struct hunt_hint spare_hints[4] = {
#include "stg25/hunt_routes/spare_hints.inc"
};
void *death_chamber_hint_file = {
#include "stg25/hunt_routes/death_chamber_hint_file.inc"
};
struct path *death_chamber_paths[1] = {
#include "stg25/hunt_routes/death_chamber_paths.inc"
};
struct path *death_chamber_emerald_paths[17] = {
#include "stg25/hunt_routes/death_chamber_emerald_paths.inc"
};
#pragma section

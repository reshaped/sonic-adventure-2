/* STG26.PRS's hunt hints, pointer slot, paths and path list, from the disc.
 * its DATA lies between the section's start and room_scenery.c */
#include <ninja.h>

#include "stage/hunt.h"
#include "stage/path.h"
#include "stg26/hunt_routes.h"

#pragma section ATA
struct hunt_hint quarters_hints[149] = {
#include "stg26/hunt_routes/quarters_hints.inc"
};
static struct hunt_hint spare_hints[4] = {
#include "stg26/hunt_routes/spare_hints.inc"
};
void *quarters_hint_file = {
#include "stg26/hunt_routes/quarters_hint_file.inc"
};
static NJS_POINT3 piece_route_00_points[121] = {
#include "stg26/hunt_routes/piece_route_00_points.inc"
};
static struct path piece_route_00 = {
#include "stg26/hunt_routes/piece_route_00.inc"
};
static NJS_POINT3 piece_route_01_points[51] = {
#include "stg26/hunt_routes/piece_route_01_points.inc"
};
static struct path piece_route_01 = {
#include "stg26/hunt_routes/piece_route_01.inc"
};
static NJS_POINT3 piece_route_02_points[41] = {
#include "stg26/hunt_routes/piece_route_02_points.inc"
};
static struct path piece_route_02 = {
#include "stg26/hunt_routes/piece_route_02.inc"
};
static NJS_POINT3 piece_route_03_points[161] = {
#include "stg26/hunt_routes/piece_route_03_points.inc"
};
static struct path piece_route_03 = {
#include "stg26/hunt_routes/piece_route_03.inc"
};
static NJS_POINT3 piece_route_04_points[161] = {
#include "stg26/hunt_routes/piece_route_04_points.inc"
};
static struct path piece_route_04 = {
#include "stg26/hunt_routes/piece_route_04.inc"
};
static NJS_POINT3 piece_route_05_points[41] = {
#include "stg26/hunt_routes/piece_route_05_points.inc"
};
static struct path piece_route_05 = {
#include "stg26/hunt_routes/piece_route_05.inc"
};
static NJS_POINT3 piece_route_06_points[51] = {
#include "stg26/hunt_routes/piece_route_06_points.inc"
};
static struct path piece_route_06 = {
#include "stg26/hunt_routes/piece_route_06.inc"
};
static NJS_POINT3 piece_route_07_points[81] = {
#include "stg26/hunt_routes/piece_route_07_points.inc"
};
static struct path piece_route_07 = {
#include "stg26/hunt_routes/piece_route_07.inc"
};
static NJS_POINT3 piece_route_08_points[65] = {
#include "stg26/hunt_routes/piece_route_08_points.inc"
};
static struct path piece_route_08 = {
#include "stg26/hunt_routes/piece_route_08.inc"
};
static NJS_POINT3 piece_route_09_points[129] = {
#include "stg26/hunt_routes/piece_route_09_points.inc"
};
static struct path piece_route_09 = {
#include "stg26/hunt_routes/piece_route_09.inc"
};
static NJS_POINT3 piece_route_10_points[41] = {
#include "stg26/hunt_routes/piece_route_10_points.inc"
};
static struct path piece_route_10 = {
#include "stg26/hunt_routes/piece_route_10.inc"
};
static NJS_POINT3 piece_route_11_points[151] = {
#include "stg26/hunt_routes/piece_route_11_points.inc"
};
static struct path piece_route_11 = {
#include "stg26/hunt_routes/piece_route_11.inc"
};
static NJS_POINT3 piece_route_12_points[51] = {
#include "stg26/hunt_routes/piece_route_12_points.inc"
};
static struct path piece_route_12 = {
#include "stg26/hunt_routes/piece_route_12.inc"
};
static NJS_POINT3 piece_route_13_points[101] = {
#include "stg26/hunt_routes/piece_route_13_points.inc"
};
static struct path piece_route_13 = {
#include "stg26/hunt_routes/piece_route_13.inc"
};
static NJS_POINT3 piece_route_14_points[201] = {
#include "stg26/hunt_routes/piece_route_14_points.inc"
};
static struct path piece_route_14 = {
#include "stg26/hunt_routes/piece_route_14.inc"
};
static NJS_POINT3 piece_route_15_points[241] = {
#include "stg26/hunt_routes/piece_route_15_points.inc"
};
static struct path piece_route_15 = {
#include "stg26/hunt_routes/piece_route_15.inc"
};
struct path *quarters_paths[1] = {
#include "stg26/hunt_routes/quarters_paths.inc"
};
struct path *quarters_piece_paths[17] = {
#include "stg26/hunt_routes/quarters_piece_paths.inc"
};
#pragma section

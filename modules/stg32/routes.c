/* STG32.PRS's paths, path list, hunt hints and pointer slot, from the disc.
 * its D lies between the section's start and sky_pack.c */
#include <ninja.h>

#include "shared/emerald_hunt_data.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg32/routes.h"

static NJS_POINT3 emerald_route_1_points[121] = {
#include "stg32/routes/emerald_route_1_points.inc"
};
static struct path emerald_route_1 = {
#include "stg32/routes/emerald_route_1.inc"
};
static NJS_POINT3 emerald_route_2_points[51] = {
#include "stg32/routes/emerald_route_2_points.inc"
};
static struct path emerald_route_2 = {
#include "stg32/routes/emerald_route_2.inc"
};
static NJS_POINT3 emerald_route_3_points[41] = {
#include "stg32/routes/emerald_route_3_points.inc"
};
static struct path emerald_route_3 = {
#include "stg32/routes/emerald_route_3.inc"
};
static NJS_POINT3 emerald_route_4_points[161] = {
#include "stg32/routes/emerald_route_4_points.inc"
};
static struct path emerald_route_4 = {
#include "stg32/routes/emerald_route_4.inc"
};
static NJS_POINT3 emerald_route_5_points[161] = {
#include "stg32/routes/emerald_route_5_points.inc"
};
static struct path emerald_route_5 = {
#include "stg32/routes/emerald_route_5.inc"
};
static NJS_POINT3 emerald_route_6_points[41] = {
#include "stg32/routes/emerald_route_6_points.inc"
};
static struct path emerald_route_6 = {
#include "stg32/routes/emerald_route_6.inc"
};
static NJS_POINT3 emerald_route_7_points[51] = {
#include "stg32/routes/emerald_route_7_points.inc"
};
static struct path emerald_route_7 = {
#include "stg32/routes/emerald_route_7.inc"
};
static NJS_POINT3 emerald_route_8_points[81] = {
#include "stg32/routes/emerald_route_8_points.inc"
};
static struct path emerald_route_8 = {
#include "stg32/routes/emerald_route_8.inc"
};
struct path *hunt_piece_routes[HUNT_PIECE_ROUTES] = {
#include "stg32/routes/hunt_piece_routes.inc"
};
struct hunt_hint hunt_location_hints[149] = {
#include "stg32/routes/hunt_location_hints.inc"
};
static struct hunt_hint unused_path_hints[4] = {
#include "stg32/routes/unused_path_hints.inc"
};
void *hunt_stage_messages = {
#include "stg32/routes/hunt_stage_messages.inc"
};
static struct path_point grind_rail_1_points[77] = {
#include "stg32/routes/grind_rail_1_points.inc"
};
static struct path grind_rail_1 = {
#include "stg32/routes/grind_rail_1.inc"
};
static struct path_point grind_rail_2_points[33] = {
#include "stg32/routes/grind_rail_2_points.inc"
};
static struct path grind_rail_2 = {
#include "stg32/routes/grind_rail_2.inc"
};
static struct path_point grind_rail_3_points[201] = {
#include "stg32/routes/grind_rail_3_points.inc"
};
static struct path grind_rail_3 = {
#include "stg32/routes/grind_rail_3.inc"
};
static struct path_point grind_rail_4_points[73] = {
#include "stg32/routes/grind_rail_4_points.inc"
};
static struct path grind_rail_4 = {
#include "stg32/routes/grind_rail_4.inc"
};
struct path *meteor_herd_paths[2] = {
#include "stg32/routes/meteor_herd_paths.inc"
};

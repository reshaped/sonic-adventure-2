/* STG31.PRS's paths and path list, from the disc. its D lies between the
 * section's start and sky.c */
#include <ninja.h>

#include "stage/path.h"
#include "stage/path_task.h"
#include "stg31/path_ride.h"

static struct path_point join_path_points[23] = {
#include "stg31/paths/join_path_points.inc"
};
static struct path join_path = {
#include "stg31/paths/join_path.inc"
};
static NJS_POINT3 ride_z_points[25] = {
#include "stg31/paths/ride_z_points.inc"
};
struct path green_hill_ride_z = {
#include "stg31/paths/green_hill_ride_z.inc"
};
static NJS_POINT3 ride_x_upper_points[25] = {
#include "stg31/paths/ride_x_upper_points.inc"
};
struct path green_hill_ride_x_upper = {
#include "stg31/paths/green_hill_ride_x_upper.inc"
};
static NJS_POINT3 ride_x_lower_points[25] = {
#include "stg31/paths/ride_x_lower_points.inc"
};
struct path green_hill_ride_x_lower = {
#include "stg31/paths/green_hill_ride_x_lower.inc"
};
struct path *green_hill_paths[2] = {
#include "stg31/paths/green_hill_paths.inc"
};

/* STG43.PRS's paths and path list, from the disc. its D lies between the
 * section's start and sky_pack.c */
#include "stage/path.h"
#include "stg43/drill.h"
#include "stg43/paths.h"

static struct path_point cosmic_wall_first_path_points[94] = {
#include "stg43/paths/cosmic_wall_first_path_points.inc"
};
struct path cosmic_wall_first_path = {
#include "stg43/paths/cosmic_wall_first_path.inc"
};
static struct path_point cosmic_wall_second_path_points[99] = {
#include "stg43/paths/cosmic_wall_second_path_points.inc"
};
struct path cosmic_wall_second_path = {
#include "stg43/paths/cosmic_wall_second_path.inc"
};
struct path *cosmic_wall_paths[1] = {
#include "stg43/paths/cosmic_wall_paths.inc"
};

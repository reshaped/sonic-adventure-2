/* STG34.PRS's paths and path list, from the disc. its D lies between the
 * section's start and background_archives.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg34/routes.h"

static struct path_point upper_rail_points[14] = {
#include "stg34/routes/upper_rail_points.inc"
};
static struct path upper_rail = {
#include "stg34/routes/upper_rail.inc"
};
static struct path_point lower_rail_points[13] = {
#include "stg34/routes/lower_rail_points.inc"
};
static struct path lower_rail = {
#include "stg34/routes/lower_rail.inc"
};
static struct path_point upper_current_points[4] = {
#include "stg34/routes/upper_current_points.inc"
};
static struct path upper_current = {
#include "stg34/routes/upper_current.inc"
};
static struct path_point return_current_points[7] = {
#include "stg34/routes/return_current_points.inc"
};
static struct path return_current = {
#include "stg34/routes/return_current.inc"
};
static struct path_point descending_current_points[5] = {
#include "stg34/routes/descending_current_points.inc"
};
static struct path descending_current = {
#include "stg34/routes/descending_current.inc"
};
static struct path_point lower_current_points[117] = {
#include "stg34/routes/lower_current_points.inc"
};
static struct path lower_current = {
#include "stg34/routes/lower_current.inc"
};
struct path *core_sonic_paths[7] = {
#include "stg34/routes/core_sonic_paths.inc"
};

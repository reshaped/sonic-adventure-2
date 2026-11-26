/* STG37.PRS's paths and path list, from the disc. its D lies between the
 * section's start and background_archive.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg37/routes.h"

static struct path_point west_current_points[9] = {
#include "stg37/routes/west_current_points.inc"
};
static struct path west_current = {
#include "stg37/routes/west_current.inc"
};
static struct path_point east_current_points[9] = {
#include "stg37/routes/east_current_points.inc"
};
static struct path east_current = {
#include "stg37/routes/east_current.inc"
};
struct path *core_rouge_paths[3] = {
#include "stg37/routes/core_rouge_paths.inc"
};

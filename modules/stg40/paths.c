/* STG40.PRS's paths and path list, from the disc. its D lies between the
 * section's start and sky_pack.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg40/paths.h"

static struct path_point rail_11_points[85] = {
#include "stg40/paths/rail_11_points.inc"
};
static struct path rail_11 = {
#include "stg40/paths/rail_11.inc"
};
static struct path_point rail_10_points[140] = {
#include "stg40/paths/rail_10_points.inc"
};
static struct path rail_10 = {
#include "stg40/paths/rail_10.inc"
};
static struct path_point rail_09_points[4] = {
#include "stg40/paths/rail_09_points.inc"
};
static struct path rail_09 = {
#include "stg40/paths/rail_09.inc"
};
static struct path_point rail_08_points[24] = {
#include "stg40/paths/rail_08_points.inc"
};
static struct path rail_08 = {
#include "stg40/paths/rail_08.inc"
};
static struct path_point rail_07_points[64] = {
#include "stg40/paths/rail_07_points.inc"
};
static struct path rail_07 = {
#include "stg40/paths/rail_07.inc"
};
static struct path_point rail_06_points[20] = {
#include "stg40/paths/rail_06_points.inc"
};
static struct path rail_06 = {
#include "stg40/paths/rail_06.inc"
};
static struct path_point rail_05_points[41] = {
#include "stg40/paths/rail_05_points.inc"
};
static struct path rail_05 = {
#include "stg40/paths/rail_05.inc"
};
static struct path_point rail_04_points[26] = {
#include "stg40/paths/rail_04_points.inc"
};
static struct path rail_04 = {
#include "stg40/paths/rail_04.inc"
};
static struct path_point rail_03_points[5] = {
#include "stg40/paths/rail_03_points.inc"
};
static struct path rail_03 = {
#include "stg40/paths/rail_03.inc"
};
static struct path_point rail_02_points[21] = {
#include "stg40/paths/rail_02_points.inc"
};
static struct path rail_02 = {
#include "stg40/paths/rail_02.inc"
};
static struct path_point rail_01_points[34] = {
#include "stg40/paths/rail_01_points.inc"
};
static struct path rail_01 = {
#include "stg40/paths/rail_01.inc"
};
struct path *final_chase_paths[12] = {
#include "stg40/paths/final_chase_paths.inc"
};

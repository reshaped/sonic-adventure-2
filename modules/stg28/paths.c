/* STG28.PRS's paths and path list, from the disc. its DATA lies between the
 * section's start and cave_scenery.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg28/paths.h"

#pragma section ATA
static struct path_point join_1_points[42] = {
#include "stg28/paths/join_1_points.inc"
};
static struct path join_1 = {
#include "stg28/paths/join_1.inc"
};
static struct path_point join_2_points[42] = {
#include "stg28/paths/join_2_points.inc"
};
static struct path join_2 = {
#include "stg28/paths/join_2.inc"
};
static struct path_point join_3_points[42] = {
#include "stg28/paths/join_3_points.inc"
};
static struct path join_3 = {
#include "stg28/paths/join_3.inc"
};
static struct path_point join_4_points[45] = {
#include "stg28/paths/join_4_points.inc"
};
static struct path join_4 = {
#include "stg28/paths/join_4.inc"
};
static struct path_point join_5_points[45] = {
#include "stg28/paths/join_5_points.inc"
};
static struct path join_5 = {
#include "stg28/paths/join_5.inc"
};
static struct path_point rail_1_points[43] = {
#include "stg28/paths/rail_1_points.inc"
};
static struct path rail_1 = {
#include "stg28/paths/rail_1.inc"
};
static struct path_point rail_2_points[18] = {
#include "stg28/paths/rail_2_points.inc"
};
static struct path rail_2 = {
#include "stg28/paths/rail_2.inc"
};
static struct path_point rail_3_points[18] = {
#include "stg28/paths/rail_3_points.inc"
};
static struct path rail_3 = {
#include "stg28/paths/rail_3.inc"
};
static struct path_point rail_4_points[18] = {
#include "stg28/paths/rail_4_points.inc"
};
static struct path rail_4 = {
#include "stg28/paths/rail_4.inc"
};
static struct path_point rail_5_points[18] = {
#include "stg28/paths/rail_5_points.inc"
};
static struct path rail_5 = {
#include "stg28/paths/rail_5.inc"
};
static struct path_point rail_6_points[18] = {
#include "stg28/paths/rail_6_points.inc"
};
static struct path rail_6 = {
#include "stg28/paths/rail_6.inc"
};
static struct path_point rail_7_points[18] = {
#include "stg28/paths/rail_7_points.inc"
};
static struct path rail_7 = {
#include "stg28/paths/rail_7.inc"
};
static struct path_point rail_8_points[12] = {
#include "stg28/paths/rail_8_points.inc"
};
static struct path rail_8 = {
#include "stg28/paths/rail_8.inc"
};
struct path *pyramid_cave_paths[14] = {
#include "stg28/paths/pyramid_cave_paths.inc"
};
#pragma section

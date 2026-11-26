/* STG13.PRS's paths and path list, from the disc. its DATA lies between the
 * section's start and city_escape.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg13/paths.h"

#pragma section ATA
static struct path_point join_path_points[53] = {
#include "stg13/paths/join_path_points.inc"
};
static struct path join_path = {
#include "stg13/paths/join_path.inc"
};
static struct path_point rail_1_points[5] = {
#include "stg13/paths/rail_1_points.inc"
};
static struct path rail_1 = {
#include "stg13/paths/rail_1.inc"
};
static struct path_point rail_2_points[5] = {
#include "stg13/paths/rail_2_points.inc"
};
static struct path rail_2 = {
#include "stg13/paths/rail_2.inc"
};
static struct path_point rail_3_points[7] = {
#include "stg13/paths/rail_3_points.inc"
};
static struct path rail_3 = {
#include "stg13/paths/rail_3.inc"
};
static struct path_point rail_4_points[36] = {
#include "stg13/paths/rail_4_points.inc"
};
static struct path rail_4 = {
#include "stg13/paths/rail_4.inc"
};
static struct path_point rail_5_points[16] = {
#include "stg13/paths/rail_5_points.inc"
};
static struct path rail_5 = {
#include "stg13/paths/rail_5.inc"
};
static struct path_point rail_6_points[26] = {
#include "stg13/paths/rail_6_points.inc"
};
static struct path rail_6 = {
#include "stg13/paths/rail_6.inc"
};
static struct path_point rail_7_points[27] = {
#include "stg13/paths/rail_7_points.inc"
};
static struct path rail_7 = {
#include "stg13/paths/rail_7.inc"
};
static struct path_point rail_8_points[3] = {
#include "stg13/paths/rail_8_points.inc"
};
static struct path rail_8 = {
#include "stg13/paths/rail_8.inc"
};
static struct path_point rail_9_points[5] = {
#include "stg13/paths/rail_9_points.inc"
};
static struct path rail_9 = {
#include "stg13/paths/rail_9.inc"
};
static struct path_point rail_10_points[5] = {
#include "stg13/paths/rail_10_points.inc"
};
static struct path rail_10 = {
#include "stg13/paths/rail_10.inc"
};
static struct path_point rail_11_points[7] = {
#include "stg13/paths/rail_11_points.inc"
};
static struct path rail_11 = {
#include "stg13/paths/rail_11.inc"
};
static struct path_point rail_12_points[5] = {
#include "stg13/paths/rail_12_points.inc"
};
static struct path rail_12 = {
#include "stg13/paths/rail_12.inc"
};
struct path *city_escape_paths[14] = {
#include "stg13/paths/city_escape_paths.inc"
};
#pragma section

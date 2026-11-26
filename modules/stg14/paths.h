/* Radical Highway's joins, running path and rails, in stage traversal order. */
#ifndef STG14_PATHS_H
#define STG14_PATHS_H
#include "stage/path.h"
#include "stage/path_task.h"

static struct path_point join_1_points[52] = {
#include "stg14/paths/join_1_points.inc"
};

static struct path join_1 = {
#include "stg14/paths/join_1.inc"
};

static struct path_point join_2_points[52] = {
#include "stg14/paths/join_2_points.inc"
};

static struct path join_2 = {
#include "stg14/paths/join_2.inc"
};

static struct path_point join_3_points[52] = {
#include "stg14/paths/join_3_points.inc"
};

static struct path join_3 = {
#include "stg14/paths/join_3.inc"
};

static struct path_point join_4_points[52] = {
#include "stg14/paths/join_4_points.inc"
};

static struct path join_4 = {
#include "stg14/paths/join_4.inc"
};

static struct path_point join_5_points[54] = {
#include "stg14/paths/join_5_points.inc"
};

static struct path join_5 = {
#include "stg14/paths/join_5.inc"
};

static struct path_point join_6_points[98] = {
#include "stg14/paths/join_6_points.inc"
};

static struct path join_6 = {
#include "stg14/paths/join_6.inc"
};

static struct path_point join_7_points[54] = {
#include "stg14/paths/join_7_points.inc"
};

static struct path join_7 = {
#include "stg14/paths/join_7.inc"
};

static struct path_point run_1_points[139] = {
#include "stg14/paths/run_1_points.inc"
};

static struct path run_1 = {
#include "stg14/paths/run_1.inc"
};

static struct path_point rail_1_points[20] = {
#include "stg14/paths/rail_1_points.inc"
};

static struct path rail_1 = {
#include "stg14/paths/rail_1.inc"
};

static struct path_point rail_2_points[20] = {
#include "stg14/paths/rail_2_points.inc"
};

static struct path rail_2 = {
#include "stg14/paths/rail_2.inc"
};

static struct path_point rail_3_points[20] = {
#include "stg14/paths/rail_3_points.inc"
};

static struct path rail_3 = {
#include "stg14/paths/rail_3.inc"
};

static struct path_point rail_4_points[20] = {
#include "stg14/paths/rail_4_points.inc"
};

static struct path rail_4 = {
#include "stg14/paths/rail_4.inc"
};

static struct path_point rail_5_points[20] = {
#include "stg14/paths/rail_5_points.inc"
};

static struct path rail_5 = {
#include "stg14/paths/rail_5.inc"
};

static struct path_point rail_6_points[20] = {
#include "stg14/paths/rail_6_points.inc"
};

static struct path rail_6 = {
#include "stg14/paths/rail_6.inc"
};

static struct path_point rail_7_points[39] = {
#include "stg14/paths/rail_7_points.inc"
};

static struct path rail_7 = {
#include "stg14/paths/rail_7.inc"
};

static struct path_point rail_8_points[39] = {
#include "stg14/paths/rail_8_points.inc"
};

static struct path rail_8 = {
#include "stg14/paths/rail_8.inc"
};

static struct path *radical_highway_paths[17] = {
#include "stg14/paths/radical_highway_paths.inc"
};

#endif

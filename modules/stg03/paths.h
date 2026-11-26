/* Four paths attach a player near their curve. The remaining eight are rails.
 */
#ifndef STG03_PATHS_H
#define STG03_PATHS_H
#include "stage/path.h"
#include "stage/path_task.h"

static struct path_point join_path_1_points[60] = {
#include "stg03/paths/join_path_1_points.inc"
};

static struct path join_path_1 = {
#include "stg03/paths/join_path_1.inc"
};

static struct path_point join_path_2_points[60] = {
#include "stg03/paths/join_path_2_points.inc"
};

static struct path join_path_2 = {
#include "stg03/paths/join_path_2.inc"
};

static struct path_point join_path_3_points[60] = {
#include "stg03/paths/join_path_3_points.inc"
};

static struct path join_path_3 = {
#include "stg03/paths/join_path_3.inc"
};

static struct path_point join_path_4_points[61] = {
#include "stg03/paths/join_path_4_points.inc"
};

static struct path join_path_4 = {
#include "stg03/paths/join_path_4.inc"
};

static struct path_point rail_1_points[34] = {
#include "stg03/paths/rail_1_points.inc"
};

static struct path rail_1 = {
#include "stg03/paths/rail_1.inc"
};

static struct path_point rail_2_points[3] = {
#include "stg03/paths/rail_2_points.inc"
};

static struct path rail_2 = {
#include "stg03/paths/rail_2.inc"
};

static struct path_point rail_3_points[3] = {
#include "stg03/paths/rail_3_points.inc"
};

static struct path rail_3 = {
#include "stg03/paths/rail_3.inc"
};

static struct path_point rail_4_points[5] = {
#include "stg03/paths/rail_4_points.inc"
};

static struct path rail_4 = {
#include "stg03/paths/rail_4.inc"
};

static struct path_point rail_5_points[20] = {
#include "stg03/paths/rail_5_points.inc"
};

static struct path rail_5 = {
#include "stg03/paths/rail_5.inc"
};

static struct path_point rail_6_points[22] = {
#include "stg03/paths/rail_6_points.inc"
};

static struct path rail_6 = {
#include "stg03/paths/rail_6.inc"
};

static struct path_point rail_7_points[19] = {
#include "stg03/paths/rail_7_points.inc"
};

static struct path rail_7 = {
#include "stg03/paths/rail_7.inc"
};

static struct path_point rail_8_points[13] = {
#include "stg03/paths/rail_8_points.inc"
};

static struct path rail_8 = {
#include "stg03/paths/rail_8.inc"
};

static struct path *green_forest_paths[13] = {
#include "stg03/paths/green_forest_paths.inc"
};

#endif

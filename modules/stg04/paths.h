/* The three paths capture a player near their curve. */
#ifndef STG04_PATHS_H
#define STG04_PATHS_H
#include "stage/path.h"
#include "stage/path_task.h"

static struct path_point join_path_1_points[60] = {
#include "stg04/paths/join_path_1_points.inc"
};

static struct path join_path_1 = {
#include "stg04/paths/join_path_1.inc"
};

static struct path_point join_path_2_points[61] = {
#include "stg04/paths/join_path_2_points.inc"
};

static struct path join_path_2 = {
#include "stg04/paths/join_path_2.inc"
};

static struct path_point join_path_3_points[61] = {
#include "stg04/paths/join_path_3_points.inc"
};

static struct path join_path_3 = {
#include "stg04/paths/join_path_3.inc"
};

static struct path *white_jungle_paths[4] = {
#include "stg04/paths/white_jungle_paths.inc"
};

#endif

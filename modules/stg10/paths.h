/* Four approach paths and the four rails used after them. */
#ifndef STG10_PATHS_H
#define STG10_PATHS_H
#include "stage/path.h"
#include "stage/path_task.h"

static struct path_point approach_1_points[50] = {
#include "stg10/paths/approach_1_points.inc"
};

static struct path approach_1 = {
#include "stg10/paths/approach_1.inc"
};

static struct path_point approach_2_points[50] = {
#include "stg10/paths/approach_2_points.inc"
};

static struct path approach_2 = {
#include "stg10/paths/approach_2.inc"
};

static struct path_point approach_3_points[50] = {
#include "stg10/paths/approach_3_points.inc"
};

static struct path approach_3 = {
#include "stg10/paths/approach_3.inc"
};

static struct path_point approach_4_points[20] = {
#include "stg10/paths/approach_4_points.inc"
};

static struct path approach_4 = {
#include "stg10/paths/approach_4.inc"
};

static struct path_point rail_1_points[16] = {
#include "stg10/paths/rail_1_points.inc"
};

static struct path rail_1 = {
#include "stg10/paths/rail_1.inc"
};

static struct path_point rail_2_points[16] = {
#include "stg10/paths/rail_2_points.inc"
};

static struct path rail_2 = {
#include "stg10/paths/rail_2.inc"
};

static struct path_point rail_3_points[16] = {
#include "stg10/paths/rail_3_points.inc"
};

static struct path rail_3 = {
#include "stg10/paths/rail_3.inc"
};

static struct path_point rail_4_points[16] = {
#include "stg10/paths/rail_4_points.inc"
};

static struct path rail_4 = {
#include "stg10/paths/rail_4.inc"
};

static struct path *metal_harbor_paths[9] = {
#include "stg10/paths/metal_harbor_paths.inc"
};

#endif

/* Moving Emerald shards follow these five routes. */
#ifndef STG05_EMERALD_ROUTES_H
#define STG05_EMERALD_ROUTES_H

#include <ninja.h>

#include "stage/path.h"

static NJS_POINT3 pumpkin_hill_emerald_path_0_points[21] = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_0_points.inc"
};

static struct path pumpkin_hill_emerald_path_0 = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_0.inc"
};

static NJS_POINT3 pumpkin_hill_emerald_path_1_points[5] = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_1_points.inc"
};

static struct path pumpkin_hill_emerald_path_1 = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_1.inc"
};

static NJS_POINT3 pumpkin_hill_emerald_path_2_points[21] = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_2_points.inc"
};

static struct path pumpkin_hill_emerald_path_2 = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_2.inc"
};

static NJS_POINT3 pumpkin_hill_emerald_path_3_points[31] = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_3_points.inc"
};

static struct path pumpkin_hill_emerald_path_3 = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_3.inc"
};

static NJS_POINT3 pumpkin_hill_emerald_path_4_points[17] = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_4_points.inc"
};

static struct path pumpkin_hill_emerald_path_4 = {
#include "stg05/emerald_routes/pumpkin_hill_emerald_path_4.inc"
};

#endif

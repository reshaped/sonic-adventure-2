/* The eight Security Hall emerald routes use positions without slope or
 * segment-length fields. */
#ifndef STG08_PATHS_H
#define STG08_PATHS_H
#include <ninja.h>

#include "stage/path.h"

static NJS_POINT3 piece_path_1_points[121] = {
#include "stg08/paths/piece_path_1_points.inc"
};

static struct path piece_path_1 = {
#include "stg08/paths/piece_path_1.inc"
};

static NJS_POINT3 piece_path_2_points[51] = {
#include "stg08/paths/piece_path_2_points.inc"
};

static struct path piece_path_2 = {
#include "stg08/paths/piece_path_2.inc"
};

static NJS_POINT3 piece_path_3_points[41] = {
#include "stg08/paths/piece_path_3_points.inc"
};

static struct path piece_path_3 = {
#include "stg08/paths/piece_path_3.inc"
};

static NJS_POINT3 piece_path_4_points[161] = {
#include "stg08/paths/piece_path_4_points.inc"
};

static struct path piece_path_4 = {
#include "stg08/paths/piece_path_4.inc"
};

static NJS_POINT3 piece_path_5_points[161] = {
#include "stg08/paths/piece_path_5_points.inc"
};

static struct path piece_path_5 = {
#include "stg08/paths/piece_path_5.inc"
};

static NJS_POINT3 piece_path_6_points[41] = {
#include "stg08/paths/piece_path_6_points.inc"
};

static struct path piece_path_6 = {
#include "stg08/paths/piece_path_6.inc"
};

static NJS_POINT3 piece_path_7_points[51] = {
#include "stg08/paths/piece_path_7_points.inc"
};

static struct path piece_path_7 = {
#include "stg08/paths/piece_path_7.inc"
};

static NJS_POINT3 piece_path_8_points[81] = {
#include "stg08/paths/piece_path_8_points.inc"
};

static struct path piece_path_8 = {
#include "stg08/paths/piece_path_8.inc"
};

static struct path *security_piece_paths[8] = {
#include "stg08/paths/security_piece_paths.inc"
};

#endif

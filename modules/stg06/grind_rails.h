/* Sky Rail uses the normal gravity rail handler for all eighteen paths. */
#ifndef STG06_GRIND_RAILS_H
#define STG06_GRIND_RAILS_H

#include "stage/path.h"
#include "stage/path_task.h"

static struct path_point grind_rail_1_points[24] = {
#include "stg06/grind_rails/grind_rail_1_points.inc"
};

static struct path grind_rail_1 = {
#include "stg06/grind_rails/grind_rail_1.inc"
};

static struct path_point grind_rail_2_points[24] = {
#include "stg06/grind_rails/grind_rail_2_points.inc"
};

static struct path grind_rail_2 = {
#include "stg06/grind_rails/grind_rail_2.inc"
};

static struct path_point grind_rail_3_points[16] = {
#include "stg06/grind_rails/grind_rail_3_points.inc"
};

static struct path grind_rail_3 = {
#include "stg06/grind_rails/grind_rail_3.inc"
};

static struct path_point grind_rail_4_points[16] = {
#include "stg06/grind_rails/grind_rail_4_points.inc"
};

static struct path grind_rail_4 = {
#include "stg06/grind_rails/grind_rail_4.inc"
};

static struct path_point grind_rail_5_points[70] = {
#include "stg06/grind_rails/grind_rail_5_points.inc"
};

static struct path grind_rail_5 = {
#include "stg06/grind_rails/grind_rail_5.inc"
};

static struct path_point grind_rail_6_points[70] = {
#include "stg06/grind_rails/grind_rail_6_points.inc"
};

static struct path grind_rail_6 = {
#include "stg06/grind_rails/grind_rail_6.inc"
};

static struct path_point grind_rail_7_points[99] = {
#include "stg06/grind_rails/grind_rail_7_points.inc"
};

static struct path grind_rail_7 = {
#include "stg06/grind_rails/grind_rail_7.inc"
};

static struct path_point grind_rail_8_points[99] = {
#include "stg06/grind_rails/grind_rail_8_points.inc"
};

static struct path grind_rail_8 = {
#include "stg06/grind_rails/grind_rail_8.inc"
};

static struct path_point grind_rail_9_points[48] = {
#include "stg06/grind_rails/grind_rail_9_points.inc"
};

static struct path grind_rail_9 = {
#include "stg06/grind_rails/grind_rail_9.inc"
};

static struct path_point grind_rail_10_points[58] = {
#include "stg06/grind_rails/grind_rail_10_points.inc"
};

static struct path grind_rail_10 = {
#include "stg06/grind_rails/grind_rail_10.inc"
};

static struct path_point grind_rail_11_points[93] = {
#include "stg06/grind_rails/grind_rail_11_points.inc"
};

static struct path grind_rail_11 = {
#include "stg06/grind_rails/grind_rail_11.inc"
};

static struct path_point grind_rail_12_points[37] = {
#include "stg06/grind_rails/grind_rail_12_points.inc"
};

static struct path grind_rail_12 = {
#include "stg06/grind_rails/grind_rail_12.inc"
};

static struct path_point grind_rail_13_points[37] = {
#include "stg06/grind_rails/grind_rail_13_points.inc"
};

static struct path grind_rail_13 = {
#include "stg06/grind_rails/grind_rail_13.inc"
};

static struct path_point grind_rail_14_points[93] = {
#include "stg06/grind_rails/grind_rail_14_points.inc"
};

static struct path grind_rail_14 = {
#include "stg06/grind_rails/grind_rail_14.inc"
};

static struct path_point grind_rail_15_points[9] = {
#include "stg06/grind_rails/grind_rail_15_points.inc"
};

static struct path grind_rail_15 = {
#include "stg06/grind_rails/grind_rail_15.inc"
};

static struct path_point grind_rail_16_points[21] = {
#include "stg06/grind_rails/grind_rail_16_points.inc"
};

static struct path grind_rail_16 = {
#include "stg06/grind_rails/grind_rail_16.inc"
};

static struct path_point grind_rail_17_points[4] = {
#include "stg06/grind_rails/grind_rail_17_points.inc"
};

static struct path grind_rail_17 = {
#include "stg06/grind_rails/grind_rail_17.inc"
};

static struct path_point grind_rail_18_points[4] = {
#include "stg06/grind_rails/grind_rail_18_points.inc"
};

static struct path grind_rail_18 = {
#include "stg06/grind_rails/grind_rail_18.inc"
};

static struct path *sky_rail_paths[19] = {
#include "stg06/grind_rails/sky_rail_paths.inc"
};

#endif

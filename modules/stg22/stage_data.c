/* STG22.PRS's paths and path list, from the disc. its D lies between the
 * section's start and background_bank.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg22/stage_data.h"

static struct path_point inverted_rail_1_points[13] = {
#include "stg22/stage_data/inverted_rail_1_points.inc"
};
static struct path inverted_rail_1 = {
#include "stg22/stage_data/inverted_rail_1.inc"
};
static struct path_point inverted_rail_2_points[13] = {
#include "stg22/stage_data/inverted_rail_2_points.inc"
};
static struct path inverted_rail_2 = {
#include "stg22/stage_data/inverted_rail_2.inc"
};
static struct path_point inverted_rail_3_points[13] = {
#include "stg22/stage_data/inverted_rail_3_points.inc"
};
static struct path inverted_rail_3 = {
#include "stg22/stage_data/inverted_rail_3.inc"
};
static struct path_point inverted_rail_4_points[13] = {
#include "stg22/stage_data/inverted_rail_4_points.inc"
};
static struct path inverted_rail_4 = {
#include "stg22/stage_data/inverted_rail_4.inc"
};
static struct path_point inverted_rail_5_points[13] = {
#include "stg22/stage_data/inverted_rail_5_points.inc"
};
static struct path inverted_rail_5 = {
#include "stg22/stage_data/inverted_rail_5.inc"
};
static struct path_point inverted_rail_6_points[21] = {
#include "stg22/stage_data/inverted_rail_6_points.inc"
};
static struct path inverted_rail_6 = {
#include "stg22/stage_data/inverted_rail_6.inc"
};
static struct path_point upright_rail_points[12] = {
#include "stg22/stage_data/upright_rail_points.inc"
};
static struct path upright_rail = {
#include "stg22/stage_data/upright_rail.inc"
};
struct path *crazy_gadget_paths[8] = {
#include "stg22/stage_data/crazy_gadget_paths.inc"
};

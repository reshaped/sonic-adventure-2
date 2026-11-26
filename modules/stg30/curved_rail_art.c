/* STG30.PRS's paths, from the disc. its D lies between curve_tex.c and
 * curve_b_tex.c */
#include "stage/path.h"
#include "stg30/curved_rail_art.h"

static struct path_point points[13] = {
#include "stg30/curved_rail_art/points.inc"
};
struct path curved_rail_path = {
#include "stg30/curved_rail_art/curved_rail_path.inc"
};

/* CHAOSTGRACE.PRS's chunk models, from the disc. its D lies between
 * hunt_data.c and hunt_toy.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgrace/hunt_data.h"

static Sint16 hunt_fruit_scavenger_round_fruit_polygons[213] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_round_fruit_polygons.inc"
};
static Sint32 hunt_fruit_scavenger_round_fruit_vertices[171] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_round_fruit_vertices.inc"
};
static NJS_CNK_MODEL hunt_fruit_scavenger_round_fruit_model = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_round_fruit_model.inc"
};
NJS_CNK_OBJECT scavenger_round_fruit = {
#include "chaostgrace/chao_race_hunt_fruit/scavenger_round_fruit.inc"
};
static Sint16 hunt_fruit_scavenger_triangle_fruit_polygons[189] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_triangle_fruit_polygons.inc"
};
static Sint32 hunt_fruit_scavenger_triangle_fruit_vertices[147] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_triangle_fruit_vertices.inc"
};
static NJS_CNK_MODEL hunt_fruit_scavenger_triangle_fruit_model = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_triangle_fruit_model.inc"
};
NJS_CNK_OBJECT scavenger_triangle_fruit = {
#include "chaostgrace/chao_race_hunt_fruit/scavenger_triangle_fruit.inc"
};
static Sint16 hunt_fruit_scavenger_square_fruit_polygons[293] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_square_fruit_polygons.inc"
};
static Sint32 hunt_fruit_scavenger_square_fruit_vertices[195] = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_square_fruit_vertices.inc"
};
static NJS_CNK_MODEL hunt_fruit_scavenger_square_fruit_model = {
#include "chaostgrace/chao_race_hunt_fruit/hunt_fruit_scavenger_square_fruit_model.inc"
};
NJS_CNK_OBJECT scavenger_square_fruit = {
#include "chaostgrace/chao_race_hunt_fruit/scavenger_square_fruit.inc"
};

/* the art of the Egg Golem's ring of footholds. a foothold in an even slot
 * draws model a, and one in an odd slot model b. golem_foothold_pieces'
 * children are the pieces a shattered foothold breaks into */
#ifndef SHARED_GOLEM_FOOTHOLD_ART_H
#define SHARED_GOLEM_FOOTHOLD_ART_H

#include <ninja.h>

extern NJS_TEXLIST *golem_foothold_texlists[];
extern NJS_TEXLIST golem_foothold_a_texlist;
extern NJS_CNK_MODEL golem_foothold_a_model;
extern NJS_CNK_OBJECT golem_foothold_a_object;
extern NJS_TEXLIST golem_foothold_b_texlist;
extern NJS_CNK_MODEL golem_foothold_b_model;
extern NJS_CNK_OBJECT golem_foothold_b_object;
extern NJS_OBJECT golem_foothold_sensor_object;
extern NJS_OBJECT golem_foothold_side_object;
extern NJS_TEXLIST golem_foothold_pieces_texlist;
extern NJS_CNK_OBJECT golem_foothold_pieces[];
extern NJS_MOTION golem_foothold_sink_motion[];

#endif

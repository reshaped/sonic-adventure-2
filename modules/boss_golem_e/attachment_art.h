/* the models and texture lists of the Egg Golem's parts in this fight */
#ifndef BOSS_GOLEM_E_ATTACHMENT_ART_H
#define BOSS_GOLEM_E_ATTACHMENT_ART_H

#include <ninja.h>

extern NJS_TEXLIST golem_e_rubble_texlist;
extern NJS_TEXLIST golem_e_mechanism_texlist;
extern NJS_TEXLIST golem_e_piece_texlist;
extern NJS_TEXLIST golem_e_weak_point_a_texlist;
extern NJS_TEXLIST golem_e_weak_point_b_texlist;
extern NJS_TEXLIST golem_e_weak_point_c_texlist;
extern NJS_CNK_MODEL golem_e_weak_point_a_model;
extern NJS_CNK_MODEL golem_e_weak_point_b_model;
extern NJS_CNK_MODEL golem_e_weak_point_c_model;
extern NJS_CNK_MODEL golem_e_mechanism_model;
extern NJS_CNK_MODEL golem_e_light_a_model;
extern NJS_CNK_MODEL golem_e_light_b_model;
extern NJS_CNK_OBJECT golem_e_mechanism_object;
extern NJS_CNK_OBJECT golem_e_piece_object;
extern NJS_CNK_OBJECT golem_e_weak_point_a_object;
extern NJS_CNK_OBJECT golem_e_weak_point_b_object;
extern NJS_CNK_OBJECT golem_e_weak_point_c_object;
extern NJS_CNK_OBJECT golem_e_rubble_object;
/* the rubble's model and the piece's, declared as arrays of one, where each
 * model alone would do */
extern NJS_CNK_MODEL golem_e_rubble_model[1];
extern NJS_CNK_MODEL golem_e_piece_model[1];
extern NJS_TEXLIST golem_e_object_texlist;

#endif

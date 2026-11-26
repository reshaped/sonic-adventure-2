/* the art of the hornet and of the akahige, hornet.c's two builds. each holds a
 * model for each kind, the pieces each kind breaks into, and its kinds' names.
 * each also holds a bomb and the bomb's texture animation */
#ifndef SHARED_HORNET_ART_H
#define SHARED_HORNET_ART_H

#include <ninja.h>

#include "draw/texture_anim.h"

extern NJS_TEXLIST hornet_texlist;
extern NJS_CNK_OBJECT hornet_bomb_object;
extern NJS_CNK_MODEL bomb_model;
extern struct texture_anim bomb_anim;
extern char *hornet_form_names[4];

extern NJS_CNK_OBJECT hornet_3_object;
extern NJS_CNK_MODEL hornet_3_model;
extern NJS_CNK_MODEL hornet_3_piece_0;
extern NJS_CNK_MODEL hornet_3_piece_1;
extern NJS_CNK_MODEL hornet_3_piece_2;

extern NJS_CNK_OBJECT hornet_6_object;
extern NJS_CNK_MODEL hornet_6_model;
extern NJS_CNK_MODEL hornet_6_piece_0;
extern NJS_CNK_MODEL hornet_6_piece_1;
extern NJS_CNK_MODEL hornet_6_piece_2;

extern NJS_CNK_OBJECT hornet_9_object;
extern NJS_CNK_MODEL hornet_9_model;
extern NJS_CNK_MODEL hornet_9_piece_0;
extern NJS_CNK_MODEL hornet_9_piece_1;
extern NJS_CNK_MODEL hornet_9_piece_2;

extern NJS_CNK_OBJECT laser_hornet_object;
extern NJS_CNK_MODEL laser_hornet_model;
extern NJS_CNK_MODEL laser_hornet_piece_0;
extern NJS_CNK_MODEL laser_hornet_piece_1;
extern NJS_CNK_MODEL laser_hornet_piece_2;
extern NJS_CNK_MODEL laser_hornet_piece_3;

extern NJS_TEXLIST akahige_texlist;
extern NJS_CNK_OBJECT akahige_bomb_object;
extern NJS_CNK_MODEL akahige_bomb_model;
extern struct texture_anim akahige_bomb_anim;
extern char *akahige_form_names[2];

extern NJS_CNK_OBJECT akahige_3_object;
extern NJS_CNK_MODEL akahige_3_model;
extern NJS_CNK_MODEL akahige_3_piece_0;
extern NJS_CNK_MODEL akahige_3_piece_1;
extern NJS_CNK_MODEL akahige_3_piece_2;

extern NJS_CNK_OBJECT akahige_6_object;
extern NJS_CNK_MODEL akahige_6_model;
extern NJS_CNK_MODEL akahige_6_piece_0;
extern NJS_CNK_MODEL akahige_6_piece_1;
extern NJS_CNK_MODEL akahige_6_piece_2;

#endif

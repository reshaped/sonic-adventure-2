/* the kyoko's art: its body and spinner, a model for each gun, its pieces and
 * its motions */
#ifndef SHARED_ENEMY_KYOKO_ART_H
#define SHARED_ENEMY_KYOKO_ART_H
#include <ninja.h>
#include "draw/texture_anim.h"

extern NJS_CNK_MODEL kyoko_piece_0;
extern NJS_CNK_MODEL kyoko_capture_gun_model;
extern NJS_CNK_OBJECT kyoko_capture_gun[1];
extern NJS_CNK_MODEL kyoko_laser_gun_model;
extern NJS_CNK_OBJECT kyoko_laser_gun[1];
extern NJS_CNK_MODEL kyoko_bullet_gun_model;
extern NJS_CNK_OBJECT kyoko_bullet_gun[1];
extern NJS_CNK_MODEL kyoko_piece_1;
extern NJS_CNK_MODEL kyoko_piece_2;
extern NJS_CNK_MODEL kyoko_piece_3;
extern NJS_CNK_MODEL kyoko_piece_4;
extern NJS_CNK_MODEL kyoko_piece_5;
extern NJS_CNK_MODEL kyoko_piece_6;
extern NJS_CNK_MODEL kyoko_spinner_model;
extern NJS_CNK_OBJECT kyoko_spinner[1];
extern NJS_CNK_MODEL kyoko_body_model;
extern NJS_CNK_OBJECT kyoko_object;
extern struct texture_anim kyoko_body_anim;
extern NJS_TEXLIST kyoko_texlist;
extern NJS_MOTION kyoko_motion;
extern NJS_MOTION kyoko_unused_motion;
extern NJS_MOTION kyoko_jets_on_motion;
extern NJS_MOTION kyoko_jets_motion;

#endif

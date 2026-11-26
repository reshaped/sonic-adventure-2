/* the emi's art, for enemy_emi.c: texture lists, and each kind's model, pieces
 * and motions */
#ifndef SHARED_ENEMY_EMI_ART_H
#define SHARED_ENEMY_EMI_ART_H
#include <ninja.h>

#include "draw/texture_anim.h"
#include "shared/enemy_emi.h"

extern const char emi_texture_filenames[67];

/* the texture lists. emi_texlist, of seven textures, serves every kind but
 * EMI_SPIKED, which has a list of six */
extern NJS_TEXLIST emi_texlist;
extern NJS_TEXLIST emi_spiked_texlist;
/* each kind's model, EMI_DOOL drawing EMI_NORMAL's. a kind's _scroll_model is
 * the part whose texture scrolls as the emi walks. its _scroll_uv holds the
 * coordinates the draw writes into that part, each v moved by the scroll */
extern NJS_CNK_OBJECT emi_object;
extern NJS_CNK_MODEL emi_scroll_model;
extern struct texture_anim emi_scroll_uv;
extern NJS_CNK_OBJECT emi_spiked_object;
extern NJS_CNK_MODEL emi_spiked_scroll_model;
extern struct texture_anim emi_spiked_scroll_uv;
extern NJS_CNK_OBJECT emi_cannon_object;
extern NJS_CNK_MODEL emi_cannon_scroll_model;
extern struct texture_anim emi_cannon_scroll_uv;
extern NJS_CNK_OBJECT emi_armored_object;
extern NJS_CNK_MODEL emi_armored_scroll_model;
extern struct texture_anim emi_armored_scroll_uv;
/* the pieces a beaten emi breaks into, by kind. each kind's last piece is its
 * _scroll_model */
extern NJS_CNK_MODEL emi_piece_0;
extern NJS_CNK_MODEL emi_piece_1;
extern NJS_CNK_MODEL emi_piece_2;
extern NJS_CNK_MODEL emi_piece_3;
extern NJS_CNK_MODEL emi_piece_4;
extern NJS_CNK_MODEL emi_spiked_piece_0;
extern NJS_CNK_MODEL emi_spiked_piece_1;
extern NJS_CNK_MODEL emi_spiked_piece_2;
extern NJS_CNK_MODEL emi_spiked_piece_3;
extern NJS_CNK_MODEL emi_spiked_piece_4;
extern NJS_CNK_MODEL emi_cannon_piece_0;
extern NJS_CNK_MODEL emi_cannon_piece_1;
extern NJS_CNK_MODEL emi_cannon_piece_2;
extern NJS_CNK_MODEL emi_cannon_piece_3;
extern NJS_CNK_MODEL emi_cannon_piece_4;
extern NJS_CNK_MODEL emi_cannon_piece_5;
extern NJS_CNK_MODEL emi_armored_piece_0;
extern NJS_CNK_MODEL emi_armored_piece_1;
extern NJS_CNK_MODEL emi_armored_piece_2;
extern NJS_CNK_MODEL emi_armored_piece_3;
extern NJS_CNK_MODEL emi_armored_piece_4;
/* each kind's motion. EMI_CANNON has a second, for its shot */
extern NJS_MOTION emi_motion;
extern NJS_MOTION emi_spiked_motion;
extern NJS_MOTION emi_cannon_motion;
extern NJS_MOTION emi_cannon_shot_motion;
extern NJS_MOTION emi_armored_motion;
/* the names of a cannon's bombs, by bomb_kind */
extern const char *emi_bomb_names[];

/* the cannon's and the armored kind's own texture lists, which no code reads.
 * both kinds draw with emi_texlist */
extern NJS_TEXLIST emi_cannon_texlist, emi_armored_texlist;
extern const char *emi_kind_names[EMI_KINDS];
#endif

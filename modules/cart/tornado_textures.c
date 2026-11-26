/* the Tornado's texture list. fourteen of its names are its own, and seven
 * it shares with Tails's kart */
#include <ninja.h>

#include "cart/kart_art.h"
#include "cart/shared_texture_names.h"

struct tornado_filenames {
    char stg_s_bkfire[32];
    char stg_s_bkfireb[32];
    char s_t1_m_mune[32];
    char s_t2_wingb[32];
    char s_tileye[32];
    char s_tr_sonkuro[32];
    char s_tr_sonwhite[32];
    char s_tr_tailorge[32];
    char twpartsx[32];
    char tw_palet0[32];
    char tw_palet1[32];
    char tw_palet2[32];
    char tw_palet5[32];
    char tw_panel4[32];
};

static const struct tornado_filenames aircraft = {
#include "cart/tornado_textures/aircraft.inc"
};

static NJS_TEXNAME tornado_slots[21] = {
#include "cart/tornado_textures/tornado_slots.inc"
};
NJS_TEXLIST cart_tornado_textures = {
    tornado_slots, sizeof(tornado_slots) / sizeof(tornado_slots[0])};

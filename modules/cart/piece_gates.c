/* CART.PRS's gates and texture lists, from the disc. its D lies between
 * background.c and kart_eggman.c */
#include <ninja.h>

#include "cart/course.h"
#include "cart/race.h"
#include "cart/shared_texture_names.h"
#include "cart/track.h"

static struct cart_gate_point piece_01[2][4] = {
#include "cart/piece_gates/piece_01.inc"
};
static struct cart_gate_point piece_02[2][4] = {
#include "cart/piece_gates/piece_02.inc"
};
static struct cart_gate_point piece_03[2][4] = {
#include "cart/piece_gates/piece_03.inc"
};
static struct cart_gate_point piece_04[1][4] = {
#include "cart/piece_gates/piece_04.inc"
};
static struct cart_gate_point piece_05[2][4] = {
#include "cart/piece_gates/piece_05.inc"
};
static struct cart_gate_point piece_06[2][4] = {
#include "cart/piece_gates/piece_06.inc"
};
static struct cart_gate_point piece_07[2][4] = {
#include "cart/piece_gates/piece_07.inc"
};
static struct cart_gate_point piece_08[2][4] = {
#include "cart/piece_gates/piece_08.inc"
};
static struct cart_gate_point piece_09[4][4] = {
#include "cart/piece_gates/piece_09.inc"
};
static struct cart_gate_point piece_10[1][4] = {
#include "cart/piece_gates/piece_10.inc"
};
static struct cart_gate_point piece_11[3][4] = {
#include "cart/piece_gates/piece_11.inc"
};
static struct cart_gate_point piece_12[1][4] = {
#include "cart/piece_gates/piece_12.inc"
};
static struct cart_gate_point piece_13[1][4] = {
#include "cart/piece_gates/piece_13.inc"
};
static struct cart_gate_point piece_14[1][4] = {
#include "cart/piece_gates/piece_14.inc"
};
static struct cart_gate_point piece_15[1][4] = {
#include "cart/piece_gates/piece_15.inc"
};
static struct cart_gate_point piece_16[1][4] = {
#include "cart/piece_gates/piece_16.inc"
};
static struct cart_gate_point piece_17[2][4] = {
#include "cart/piece_gates/piece_17.inc"
};
static struct cart_gate_point piece_18[2][4] = {
#include "cart/piece_gates/piece_18.inc"
};
static struct cart_gate_point piece_19[1][4] = {
#include "cart/piece_gates/piece_19.inc"
};
static struct cart_gate_point piece_20[1][4] = {
#include "cart/piece_gates/piece_20.inc"
};
static struct cart_gate_point piece_21[1][4] = {
#include "cart/piece_gates/piece_21.inc"
};
static struct cart_gate_point piece_22[1][4] = {
#include "cart/piece_gates/piece_22.inc"
};
static struct cart_gate_point piece_23[1][4] = {
#include "cart/piece_gates/piece_23.inc"
};
static struct cart_gate_point piece_24[1][4] = {
#include "cart/piece_gates/piece_24.inc"
};
static struct cart_gate_point piece_25[1][4] = {
#include "cart/piece_gates/piece_25.inc"
};
static struct cart_gate_point piece_32[2][4] = {
#include "cart/piece_gates/piece_32.inc"
};
static struct cart_gate_point piece_33[2][4] = {
#include "cart/piece_gates/piece_33.inc"
};
static struct cart_gate_point piece_34[1][4] = {
#include "cart/piece_gates/piece_34.inc"
};
static struct cart_gate_point piece_35[1][4] = {
#include "cart/piece_gates/piece_35.inc"
};
static struct cart_gate_point piece_36[2][4] = {
#include "cart/piece_gates/piece_36.inc"
};
static struct cart_gate_point piece_37[2][4] = {
#include "cart/piece_gates/piece_37.inc"
};
static struct cart_gate_point piece_38[1][4] = {
#include "cart/piece_gates/piece_38.inc"
};
static struct cart_gate_point piece_39[1][4] = {
#include "cart/piece_gates/piece_39.inc"
};
static struct cart_gate_point piece_40[2][4] = {
#include "cart/piece_gates/piece_40.inc"
};
static struct cart_gate_point piece_41[2][4] = {
#include "cart/piece_gates/piece_41.inc"
};
static struct cart_gate_point piece_42[1][4] = {
#include "cart/piece_gates/piece_42.inc"
};
static struct cart_gate_point piece_43[1][4] = {
#include "cart/piece_gates/piece_43.inc"
};
static struct cart_gate_point piece_44[2][4] = {
#include "cart/piece_gates/piece_44.inc"
};
static struct cart_gate_point piece_45[2][4] = {
#include "cart/piece_gates/piece_45.inc"
};
static struct cart_gate_point piece_46[2][4] = {
#include "cart/piece_gates/piece_46.inc"
};
static struct cart_gate_point piece_47[2][4] = {
#include "cart/piece_gates/piece_47.inc"
};
static struct cart_gate_point piece_48[2][4] = {
#include "cart/piece_gates/piece_48.inc"
};
static struct cart_gate_point piece_49[2][4] = {
#include "cart/piece_gates/piece_49.inc"
};
static struct cart_gate_point piece_50[1][4] = {
#include "cart/piece_gates/piece_50.inc"
};
static struct cart_gate_point piece_51[1][4] = {
#include "cart/piece_gates/piece_51.inc"
};
static struct cart_gate_point piece_52[1][4] = {
#include "cart/piece_gates/piece_52.inc"
};
static struct cart_gate_point piece_53[1][4] = {
#include "cart/piece_gates/piece_53.inc"
};
static struct cart_gate_point piece_54[1][4] = {
#include "cart/piece_gates/piece_54.inc"
};
static struct cart_gate_point piece_55[2][4] = {
#include "cart/piece_gates/piece_55.inc"
};
static struct cart_gate_point piece_56[2][4] = {
#include "cart/piece_gates/piece_56.inc"
};
static struct cart_gate_point piece_57[1][4] = {
#include "cart/piece_gates/piece_57.inc"
};
static struct cart_gate_point piece_58[1][4] = {
#include "cart/piece_gates/piece_58.inc"
};
static struct cart_gate_point piece_59[1][4] = {
#include "cart/piece_gates/piece_59.inc"
};
static struct cart_gate_point piece_60[1][4] = {
#include "cart/piece_gates/piece_60.inc"
};
static struct cart_gate_point piece_61[1][4] = {
#include "cart/piece_gates/piece_61.inc"
};
static struct cart_gate_point piece_62[1][4] = {
#include "cart/piece_gates/piece_62.inc"
};
static struct cart_gate_point piece_63[1][4] = {
#include "cart/piece_gates/piece_63.inc"
};
static struct cart_gate_point piece_64[1][4] = {
#include "cart/piece_gates/piece_64.inc"
};
static struct cart_gate_point piece_65[1][4] = {
#include "cart/piece_gates/piece_65.inc"
};
static struct cart_gate_point piece_66[1][4] = {
#include "cart/piece_gates/piece_66.inc"
};
static struct cart_gate_point piece_67[1][4] = {
#include "cart/piece_gates/piece_67.inc"
};
static struct cart_gate_point piece_68[1][4] = {
#include "cart/piece_gates/piece_68.inc"
};
static struct cart_gate_point piece_69[1][4] = {
#include "cart/piece_gates/piece_69.inc"
};
static struct cart_gate_point piece_70[1][4] = {
#include "cart/piece_gates/piece_70.inc"
};
static struct cart_gate_point piece_71[1][4] = {
#include "cart/piece_gates/piece_71.inc"
};
static struct cart_gate_point piece_72[2][4] = {
#include "cart/piece_gates/piece_72.inc"
};
static struct cart_gate_point piece_73[2][4] = {
#include "cart/piece_gates/piece_73.inc"
};
static struct cart_gate_point piece_74[2][4] = {
#include "cart/piece_gates/piece_74.inc"
};
static struct cart_gate_point piece_75[2][4] = {
#include "cart/piece_gates/piece_75.inc"
};
static struct cart_gate_point piece_76[2][4] = {
#include "cart/piece_gates/piece_76.inc"
};
static struct cart_gate_point piece_77[2][4] = {
#include "cart/piece_gates/piece_77.inc"
};
static struct cart_gate_point piece_78[1][4] = {
#include "cart/piece_gates/piece_78.inc"
};
static struct cart_gate_point piece_79[1][4] = {
#include "cart/piece_gates/piece_79.inc"
};
static struct cart_gate_point piece_80[4][4] = {
#include "cart/piece_gates/piece_80.inc"
};
static struct cart_gate_point piece_81[1][4] = {
#include "cart/piece_gates/piece_81.inc"
};
static struct cart_gate_point piece_82[1][4] = {
#include "cart/piece_gates/piece_82.inc"
};
static struct cart_gate_point piece_83[1][4] = {
#include "cart/piece_gates/piece_83.inc"
};
static struct cart_gate_point piece_84[1][4] = {
#include "cart/piece_gates/piece_84.inc"
};
struct cart_gate_list cart_piece_gates[CART_PIECES] = {
#include "cart/piece_gates/cart_piece_gates.inc"
};
static NJS_TEXNAME race_slots[98] = {
#include "cart/piece_gates/race_slots.inc"
};
NJS_TEXLIST cart_race_textures = {
#include "cart/piece_gates/cart_race_textures.inc"
};
static NJS_TEXNAME tails_slots[39] = {
#include "cart/piece_gates/tails_slots.inc"
};
NJS_TEXLIST cart_tails_textures = {
#include "cart/piece_gates/cart_tails_textures.inc"
};
static NJS_TEXNAME rouge_slots[23] = {
#include "cart/piece_gates/rouge_slots.inc"
};
NJS_TEXLIST cart_rouge_textures = {
#include "cart/piece_gates/cart_rouge_textures.inc"
};

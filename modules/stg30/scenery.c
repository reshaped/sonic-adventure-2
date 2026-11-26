#include <sg_xpt.h>

#include "stg30/sky_land.h"
#include "draw/texture_anim.h"
#include "stg30/scenery.h"

/* one key of uvs, as texture_anim.h lays them out: an entry for each strip */
static Sint16 scroll_1_uvs[30] = {
#include "stg30/scenery/scroll_1_uvs.inc"
};
static struct texture_anim scroll_1 = {
#include "stg30/scenery/scroll_1.inc"
};

static Sint16 scroll_2_uvs[46] = {
#include "stg30/scenery/scroll_2_uvs.inc"
};
static struct texture_anim scroll_2 = {
#include "stg30/scenery/scroll_2.inc"
};

static Sint16 scroll_3_uvs[16] = {
#include "stg30/scenery/scroll_3_uvs.inc"
};
static struct texture_anim scroll_3 = {
#include "stg30/scenery/scroll_3.inc"
};

static Sint16 scroll_4_uvs[376] = {
#include "stg30/scenery/scroll_4_uvs.inc"
};
static struct texture_anim scroll_4 = {
#include "stg30/scenery/scroll_4.inc"
};

static Sint16 scroll_5_uvs[90] = {
#include "stg30/scenery/scroll_5_uvs.inc"
};
static struct texture_anim scroll_5 = {
#include "stg30/scenery/scroll_5.inc"
};

static Sint16 scroll_6_uvs[282] = {
#include "stg30/scenery/scroll_6_uvs.inc"
};
static struct texture_anim scroll_6 = {
#include "stg30/scenery/scroll_6.inc"
};

static Sint16 scroll_7_uvs[378] = {
#include "stg30/scenery/scroll_7_uvs.inc"
};
static struct texture_anim scroll_7 = {
#include "stg30/scenery/scroll_7.inc"
};

static Sint16 scroll_8_uvs[682] = {
#include "stg30/scenery/scroll_8_uvs.inc"
};
static struct texture_anim scroll_8 = {
#include "stg30/scenery/scroll_8.inc"
};

static Sint16 scroll_9_uvs[282] = {
#include "stg30/scenery/scroll_9_uvs.inc"
};
static struct texture_anim scroll_9 = {
#include "stg30/scenery/scroll_9.inc"
};

static Sint16 scroll_10_uvs[442] = {
#include "stg30/scenery/scroll_10_uvs.inc"
};
static struct texture_anim scroll_10 = {
#include "stg30/scenery/scroll_10.inc"
};

static Sint16 scroll_11_uvs[122] = {
#include "stg30/scenery/scroll_11_uvs.inc"
};
static struct texture_anim scroll_11 = {
#include "stg30/scenery/scroll_11.inc"
};

static Sint16 scroll_12_uvs[606] = {
#include "stg30/scenery/scroll_12_uvs.inc"
};
static struct texture_anim scroll_12 = {
#include "stg30/scenery/scroll_12.inc"
};

static Sint16 scroll_13_uvs[188] = {
#include "stg30/scenery/scroll_13_uvs.inc"
};
static struct texture_anim scroll_13 = {
#include "stg30/scenery/scroll_13.inc"
};

static Sint16 scroll_14_uvs[394] = {
#include "stg30/scenery/scroll_14_uvs.inc"
};
static struct texture_anim scroll_14 = {
#include "stg30/scenery/scroll_14.inc"
};

static Sint16 scroll_15_uvs[176] = {
#include "stg30/scenery/scroll_15_uvs.inc"
};
static struct texture_anim scroll_15 = {
#include "stg30/scenery/scroll_15.inc"
};

static Sint16 scroll_16_uvs[186] = {
#include "stg30/scenery/scroll_16_uvs.inc"
};
static struct texture_anim scroll_16 = {
#include "stg30/scenery/scroll_16.inc"
};

static Sint16 scroll_17_uvs[444] = {
#include "stg30/scenery/scroll_17_uvs.inc"
};
static struct texture_anim scroll_17 = {
#include "stg30/scenery/scroll_17.inc"
};

static Sint16 scroll_18_uvs[234] = {
#include "stg30/scenery/scroll_18_uvs.inc"
};
static struct texture_anim scroll_18 = {
#include "stg30/scenery/scroll_18.inc"
};

static Sint16 scroll_19_uvs[204] = {
#include "stg30/scenery/scroll_19_uvs.inc"
};
static struct texture_anim scroll_19 = {
#include "stg30/scenery/scroll_19.inc"
};

static Sint16 scroll_20_uvs[188] = {
#include "stg30/scenery/scroll_20_uvs.inc"
};
static struct texture_anim scroll_20 = {
#include "stg30/scenery/scroll_20.inc"
};

static Sint16 scroll_21_uvs[530] = {
#include "stg30/scenery/scroll_21_uvs.inc"
};
static struct texture_anim scroll_21 = {
#include "stg30/scenery/scroll_21.inc"
};

static Sint16 scroll_22_uvs[314] = {
#include "stg30/scenery/scroll_22_uvs.inc"
};
static struct texture_anim scroll_22 = {
#include "stg30/scenery/scroll_22.inc"
};

static Sint16 scroll_23_uvs[320] = {
#include "stg30/scenery/scroll_23_uvs.inc"
};
static struct texture_anim scroll_23 = {
#include "stg30/scenery/scroll_23.inc"
};

static Sint16 scroll_24_uvs[838] = {
#include "stg30/scenery/scroll_24_uvs.inc"
};
static struct texture_anim scroll_24 = {
#include "stg30/scenery/scroll_24.inc"
};

static Sint16 scroll_25_uvs[616] = {
#include "stg30/scenery/scroll_25_uvs.inc"
};
static struct texture_anim scroll_25 = {
#include "stg30/scenery/scroll_25.inc"
};

static Sint16 scroll_26_uvs[122] = {
#include "stg30/scenery/scroll_26_uvs.inc"
};
static struct texture_anim scroll_26 = {
#include "stg30/scenery/scroll_26.inc"
};

static Sint16 scroll_27_uvs[186] = {
#include "stg30/scenery/scroll_27_uvs.inc"
};
static struct texture_anim scroll_27 = {
#include "stg30/scenery/scroll_27.inc"
};

static Sint16 scroll_28_uvs[202] = {
#include "stg30/scenery/scroll_28_uvs.inc"
};
static struct texture_anim scroll_28 = {
#include "stg30/scenery/scroll_28.inc"
};

static Sint16 scroll_29_uvs[148] = {
#include "stg30/scenery/scroll_29_uvs.inc"
};
static struct texture_anim scroll_29 = {
#include "stg30/scenery/scroll_29.inc"
};

static Sint16 scroll_30_uvs[54] = {
#include "stg30/scenery/scroll_30_uvs.inc"
};
static struct texture_anim scroll_30 = {
#include "stg30/scenery/scroll_30.inc"
};

static Sint16 scroll_31_uvs[458] = {
#include "stg30/scenery/scroll_31_uvs.inc"
};
static struct texture_anim scroll_31 = {
#include "stg30/scenery/scroll_31.inc"
};

static Sint16 scroll_32_uvs[186] = {
#include "stg30/scenery/scroll_32_uvs.inc"
};
static struct texture_anim scroll_32 = {
#include "stg30/scenery/scroll_32.inc"
};

static Sint16 scroll_33_uvs[202] = {
#include "stg30/scenery/scroll_33_uvs.inc"
};
static struct texture_anim scroll_33 = {
#include "stg30/scenery/scroll_33.inc"
};

static Sint16 scroll_34_uvs[170] = {
#include "stg30/scenery/scroll_34_uvs.inc"
};
static struct texture_anim scroll_34 = {
#include "stg30/scenery/scroll_34.inc"
};

static Sint16 scroll_35_uvs[346] = {
#include "stg30/scenery/scroll_35_uvs.inc"
};
static struct texture_anim scroll_35 = {
#include "stg30/scenery/scroll_35.inc"
};

static Sint16 scroll_36_uvs[368] = {
#include "stg30/scenery/scroll_36_uvs.inc"
};
static struct texture_anim scroll_36 = {
#include "stg30/scenery/scroll_36.inc"
};

static Sint16 scroll_37_uvs[68] = {
#include "stg30/scenery/scroll_37_uvs.inc"
};
static struct texture_anim scroll_37 = {
#include "stg30/scenery/scroll_37.inc"
};

static Sint16 scroll_38_uvs[768] = {
#include "stg30/scenery/scroll_38_uvs.inc"
};
static struct texture_anim scroll_38 = {
#include "stg30/scenery/scroll_38.inc"
};

static Sint16 scroll_39_uvs[250] = {
#include "stg30/scenery/scroll_39_uvs.inc"
};
static struct texture_anim scroll_39 = {
#include "stg30/scenery/scroll_39.inc"
};

static Sint16 scroll_40_uvs[686] = {
#include "stg30/scenery/scroll_40_uvs.inc"
};
static struct texture_anim scroll_40 = {
#include "stg30/scenery/scroll_40.inc"
};

static Sint16 scroll_41_uvs[442] = {
#include "stg30/scenery/scroll_41_uvs.inc"
};
static struct texture_anim scroll_41 = {
#include "stg30/scenery/scroll_41.inc"
};

static Sint16 scroll_42_uvs[416] = {
#include "stg30/scenery/scroll_42_uvs.inc"
};
static struct texture_anim scroll_42 = {
#include "stg30/scenery/scroll_42.inc"
};

static Sint16 scroll_43_uvs[724] = {
#include "stg30/scenery/scroll_43_uvs.inc"
};
static struct texture_anim scroll_43 = {
#include "stg30/scenery/scroll_43.inc"
};

static Sint16 scroll_44_uvs[444] = {
#include "stg30/scenery/scroll_44_uvs.inc"
};
static struct texture_anim scroll_44 = {
#include "stg30/scenery/scroll_44.inc"
};

static Sint16 scroll_45_uvs[352] = {
#include "stg30/scenery/scroll_45_uvs.inc"
};
static struct texture_anim scroll_45 = {
#include "stg30/scenery/scroll_45.inc"
};

static Sint16 scroll_46_uvs[672] = {
#include "stg30/scenery/scroll_46_uvs.inc"
};
static struct texture_anim scroll_46 = {
#include "stg30/scenery/scroll_46.inc"
};

static Sint16 scroll_51_uvs[122] = {
#include "stg30/scenery/scroll_51_uvs.inc"
};
static struct texture_anim scroll_51 = {
#include "stg30/scenery/scroll_51.inc"
};

static Sint16 scroll_47_uvs[170] = {
#include "stg30/scenery/scroll_47_uvs.inc"
};
static struct texture_anim scroll_47 = {
#include "stg30/scenery/scroll_47.inc"
};

static Sint16 scroll_48_uvs[182] = {
#include "stg30/scenery/scroll_48_uvs.inc"
};
static struct texture_anim scroll_48 = {
#include "stg30/scenery/scroll_48.inc"
};

static Sint16 scroll_49_uvs[202] = {
#include "stg30/scenery/scroll_49_uvs.inc"
};
static struct texture_anim scroll_49 = {
#include "stg30/scenery/scroll_49.inc"
};

static Sint16 scroll_50_uvs[208] = {
#include "stg30/scenery/scroll_50_uvs.inc"
};
static struct texture_anim scroll_50 = {
#include "stg30/scenery/scroll_50.inc"
};

static Sint16 scroll_52_uvs[102] = {
#include "stg30/scenery/scroll_52_uvs.inc"
};
static struct texture_anim scroll_52 = {
#include "stg30/scenery/scroll_52.inc"
};

static Sint16 scroll_0_uvs[170] = {
#include "stg30/scenery/scroll_0_uvs.inc"
};
static struct texture_anim scroll_0 = {
#include "stg30/scenery/scroll_0.inc"
};

struct scenery_strip scenery_strips[SCENERY_STRIPS] = {
#include "stg30/scenery/scenery_strips.inc"
};

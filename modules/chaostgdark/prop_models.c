/* the Dark Garden's props, which set_chao_dark.bin places: fences, graves and
 * their shadows, a stump and two plants */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgdark/scenery.h"

/* the slot of each prop texture in AL_DARK_OBJ_TEX */
enum dark_prop_texture {
    PROP_TEX_GRASS = 0,
    PROP_TEX_FENCE_PANEL = 1,
    PROP_TEX_FENCE = 2,
    PROP_TEX_GRAVE_A = 3,
    PROP_TEX_GRAVE_STONE = 4,
    PROP_TEX_GRAVE_SHADOW = 5,
    PROP_TEX_GRAVE_B = 6,
    PROP_TEX_STUMP_BARK = 9,
    PROP_TEX_STUMP_WOOD = 10
};

static Sint16 dark_long_fence_polygons[661] = {
#include "chaostgdark/prop_models/dark_long_fence_polygons.inc"
};

static Sint32 dark_long_fence_vertices[355] = {
#include "chaostgdark/prop_models/dark_long_fence_vertices.inc"
};

static NJS_CNK_MODEL dark_long_fence_model = {
#include "chaostgdark/prop_models/dark_long_fence_model.inc"
};

NJS_CNK_OBJECT dark_long_fence = {
#include "chaostgdark/prop_models/dark_long_fence.inc"
};

static Sint16 dark_medium_fence_polygons[415] = {
#include "chaostgdark/prop_models/dark_medium_fence_polygons.inc"
};

static Sint32 dark_medium_fence_vertices[211] = {
#include "chaostgdark/prop_models/dark_medium_fence_vertices.inc"
};

static NJS_CNK_MODEL dark_medium_fence_model = {
#include "chaostgdark/prop_models/dark_medium_fence_model.inc"
};

NJS_CNK_OBJECT dark_medium_fence = {
#include "chaostgdark/prop_models/dark_medium_fence.inc"
};

static Sint16 dark_short_fence_polygons[415] = {
#include "chaostgdark/prop_models/dark_short_fence_polygons.inc"
};

static Sint32 dark_short_fence_vertices[211] = {
#include "chaostgdark/prop_models/dark_short_fence_vertices.inc"
};

static NJS_CNK_MODEL dark_short_fence_model = {
#include "chaostgdark/prop_models/dark_short_fence_model.inc"
};

NJS_CNK_OBJECT dark_short_fence = {
#include "chaostgdark/prop_models/dark_short_fence.inc"
};

static Sint16 dark_grave_a_polygons[137] = {
#include "chaostgdark/prop_models/dark_grave_a_polygons.inc"
};

static Sint32 dark_grave_a_vertices[67] = {
#include "chaostgdark/prop_models/dark_grave_a_vertices.inc"
};

static NJS_CNK_MODEL dark_grave_a_model = {
#include "chaostgdark/prop_models/dark_grave_a_model.inc"
};

NJS_CNK_OBJECT dark_grave_a = {
#include "chaostgdark/prop_models/dark_grave_a.inc"
};

static Sint16 dark_grave_a_shadow_polygons[25] = {
#include "chaostgdark/prop_models/dark_grave_a_shadow_polygons.inc"
};

static Sint32 dark_grave_a_shadow_vertices[19] = {
#include "chaostgdark/prop_models/dark_grave_a_shadow_vertices.inc"
};

static NJS_CNK_MODEL dark_grave_a_shadow_model = {
#include "chaostgdark/prop_models/dark_grave_a_shadow_model.inc"
};

NJS_CNK_OBJECT dark_grave_a_shadow = {
#include "chaostgdark/prop_models/dark_grave_a_shadow.inc"
};

static Sint16 dark_grave_b_polygons[371] = {
#include "chaostgdark/prop_models/dark_grave_b_polygons.inc"
};

static Sint32 dark_grave_b_vertices[371] = {
#include "chaostgdark/prop_models/dark_grave_b_vertices.inc"
};

static NJS_CNK_MODEL dark_grave_b_model = {
#include "chaostgdark/prop_models/dark_grave_b_model.inc"
};

NJS_CNK_OBJECT dark_grave_b = {
#include "chaostgdark/prop_models/dark_grave_b.inc"
};

static Sint16 dark_grave_b_shadow_polygons[25] = {
#include "chaostgdark/prop_models/dark_grave_b_shadow_polygons.inc"
};

static Sint32 dark_grave_b_shadow_vertices[19] = {
#include "chaostgdark/prop_models/dark_grave_b_shadow_vertices.inc"
};

static NJS_CNK_MODEL dark_grave_b_shadow_model = {
#include "chaostgdark/prop_models/dark_grave_b_shadow_model.inc"
};

NJS_CNK_OBJECT dark_grave_b_shadow = {
#include "chaostgdark/prop_models/dark_grave_b_shadow.inc"
};

static Sint16 dark_stump_polygons[137] = {
#include "chaostgdark/prop_models/dark_stump_polygons.inc"
};

static Sint32 dark_stump_vertices[79] = {
#include "chaostgdark/prop_models/dark_stump_vertices.inc"
};

static NJS_CNK_MODEL dark_stump_model = {
#include "chaostgdark/prop_models/dark_stump_model.inc"
};

NJS_CNK_OBJECT dark_stump = {
#include "chaostgdark/prop_models/dark_stump.inc"
};

static Sint16 dark_kuwa_a_blade_7_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_7_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_7_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_7_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_7_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_7_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_7 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_7.inc"
};

static Sint16 dark_kuwa_a_blade_6_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_6_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_6_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_6_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_6_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_6_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_6 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_6.inc"
};

static Sint16 dark_kuwa_a_blade_5_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_5_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_5_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_5_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_5_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_5_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_5 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_5.inc"
};

static Sint16 dark_kuwa_a_blade_4_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_4_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_4_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_4_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_4_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_4_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_4 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_4.inc"
};

static Sint16 dark_kuwa_a_blade_3_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_3_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_3_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_3_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_3_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_3_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_3 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_3.inc"
};

static Sint16 dark_kuwa_a_blade_2_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_2_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_2_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_2_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_2_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_2_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_2 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_2.inc"
};

static Sint16 dark_kuwa_a_blade_1_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_1_polygons.inc"
};

static Sint32 dark_kuwa_a_blade_1_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_1_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_blade_1_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_1_model.inc"
};

static NJS_CNK_OBJECT dark_kuwa_a_blade_1 = {
#include "chaostgdark/prop_models/dark_kuwa_a_blade_1.inc"
};

static Sint16 dark_kuwa_a_polygons[25] = {
#include "chaostgdark/prop_models/dark_kuwa_a_polygons.inc"
};

static Sint32 dark_kuwa_a_vertices[19] = {
#include "chaostgdark/prop_models/dark_kuwa_a_vertices.inc"
};

static NJS_CNK_MODEL dark_kuwa_a_model = {
#include "chaostgdark/prop_models/dark_kuwa_a_model.inc"
};

NJS_CNK_OBJECT dark_kuwa_a = {
#include "chaostgdark/prop_models/dark_kuwa_a.inc"
};

static Sint16 dark_grass_b_blade_5_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_5_polygons.inc"
};

static Sint32 dark_grass_b_blade_5_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_5_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_blade_5_model = {
#include "chaostgdark/prop_models/dark_grass_b_blade_5_model.inc"
};

static NJS_CNK_OBJECT dark_grass_b_blade_5 = {
#include "chaostgdark/prop_models/dark_grass_b_blade_5.inc"
};

static Sint16 dark_grass_b_blade_4_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_4_polygons.inc"
};

static Sint32 dark_grass_b_blade_4_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_4_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_blade_4_model = {
#include "chaostgdark/prop_models/dark_grass_b_blade_4_model.inc"
};

static NJS_CNK_OBJECT dark_grass_b_blade_4 = {
#include "chaostgdark/prop_models/dark_grass_b_blade_4.inc"
};

static Sint16 dark_grass_b_blade_3_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_3_polygons.inc"
};

static Sint32 dark_grass_b_blade_3_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_3_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_blade_3_model = {
#include "chaostgdark/prop_models/dark_grass_b_blade_3_model.inc"
};

static NJS_CNK_OBJECT dark_grass_b_blade_3 = {
#include "chaostgdark/prop_models/dark_grass_b_blade_3.inc"
};

static Sint16 dark_grass_b_blade_2_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_2_polygons.inc"
};

static Sint32 dark_grass_b_blade_2_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_2_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_blade_2_model = {
#include "chaostgdark/prop_models/dark_grass_b_blade_2_model.inc"
};

static NJS_CNK_OBJECT dark_grass_b_blade_2 = {
#include "chaostgdark/prop_models/dark_grass_b_blade_2.inc"
};

static Sint16 dark_grass_b_blade_1_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_1_polygons.inc"
};

static Sint32 dark_grass_b_blade_1_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_blade_1_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_blade_1_model = {
#include "chaostgdark/prop_models/dark_grass_b_blade_1_model.inc"
};

static NJS_CNK_OBJECT dark_grass_b_blade_1 = {
#include "chaostgdark/prop_models/dark_grass_b_blade_1.inc"
};

static Sint16 dark_grass_b_polygons[25] = {
#include "chaostgdark/prop_models/dark_grass_b_polygons.inc"
};

static Sint32 dark_grass_b_vertices[19] = {
#include "chaostgdark/prop_models/dark_grass_b_vertices.inc"
};

static NJS_CNK_MODEL dark_grass_b_model = {
#include "chaostgdark/prop_models/dark_grass_b_model.inc"
};

NJS_CNK_OBJECT dark_grass_b = {
#include "chaostgdark/prop_models/dark_grass_b.inc"
};

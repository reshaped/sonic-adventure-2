#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/backdrop.h"
#include "stg22/background_textures.h"

/* the background's texture names, in the image's order. its texture list
 * below names them in the order the models' texture numbers count */
static const char spacenbg128[12] = {
#include "stg22/background_bank/spacenbg128.inc"
};
static const char spacenbg64[12] = {
#include "stg22/background_bank/spacenbg64.inc"
};
static const char miu128_cg016[16] = {
#include "stg22/background_bank/miu128_cg016.inc"
};
static const char tx_poison001[16] = {
#include "stg22/background_bank/tx_poison001.inc"
};
static const char tx_poison002[16] = {
#include "stg22/background_bank/tx_poison002.inc"
};
static const char tx_poison003[16] = {
#include "stg22/background_bank/tx_poison003.inc"
};
static const char tx_poison004[16] = {
#include "stg22/background_bank/tx_poison004.inc"
};
static const char tx_poison005[16] = {
#include "stg22/background_bank/tx_poison005.inc"
};
static const char tx_poison006[16] = {
#include "stg22/background_bank/tx_poison006.inc"
};
static const char tx_poison007[16] = {
#include "stg22/background_bank/tx_poison007.inc"
};
static const char tx_poison008[16] = {
#include "stg22/background_bank/tx_poison008.inc"
};
static const char tx_poison009[16] = {
#include "stg22/background_bank/tx_poison009.inc"
};
static const char tx_poison010[16] = {
#include "stg22/background_bank/tx_poison010.inc"
};
static const char tx_poison011[16] = {
#include "stg22/background_bank/tx_poison011.inc"
};
static const char tx_poison012[16] = {
#include "stg22/background_bank/tx_poison012.inc"
};
static const char tx_poison013[16] = {
#include "stg22/background_bank/tx_poison013.inc"
};
static const char miu128_cg006[16] = {
#include "stg22/background_bank/miu128_cg006.inc"
};
static const char miu128_cg018[16] = {
#include "stg22/background_bank/miu128_cg018.inc"
};
static const char miu32_cg005[12] = {
#include "stg22/background_bank/miu32_cg005.inc"
};
static const char miu32_cg008[12] = {
#include "stg22/background_bank/miu32_cg008.inc"
};
static const char miu32_cg009[12] = {
#include "stg22/background_bank/miu32_cg009.inc"
};
static const char tx_light001[12] = {
#include "stg22/background_bank/tx_light001.inc"
};
static const char tx_light002[12] = {
#include "stg22/background_bank/tx_light002.inc"
};
static const char tx_light003[12] = {
#include "stg22/background_bank/tx_light003.inc"
};
static const char tx_light004[12] = {
#include "stg22/background_bank/tx_light004.inc"
};
static const char tx_light005[12] = {
#include "stg22/background_bank/tx_light005.inc"
};

static NJS_TEXNAME sky_textures[2] = {
#include "stg22/background_bank/sky_textures.inc"
};

NJS_TEXLIST gadget_sky_texlist = {
#include "stg22/background_bank/gadget_sky_texlist.inc"
};

static Sint16 gadget_sky_object_polygons[925] = {
#include "stg22/background_bank/gadget_sky_object_polygons.inc"
};

static Sint32 gadget_sky_object_vertices[463] = {
#include "stg22/background_bank/gadget_sky_object_vertices.inc"
};

static NJS_CNK_MODEL gadget_sky_object_model = {
#include "stg22/background_bank/gadget_sky_object_model.inc"
};

NJS_CNK_OBJECT gadget_sky_object = {
#include "stg22/background_bank/gadget_sky_object.inc"
};

static NJS_TEXNAME strip_textures[1] = {
#include "stg22/background_bank/strip_textures.inc"
};

NJS_TEXLIST gadget_strip_texlist = {
#include "stg22/background_bank/gadget_strip_texlist.inc"
};

static Sint16 gadget_strip_object_polygons[181] = {
#include "stg22/background_bank/gadget_strip_object_polygons.inc"
};

static Sint32 gadget_strip_object_vertices[187] = {
#include "stg22/background_bank/gadget_strip_object_vertices.inc"
};

static NJS_CNK_MODEL gadget_strip_object_model = {
#include "stg22/background_bank/gadget_strip_object_model.inc"
};

NJS_CNK_OBJECT gadget_strip_object[1] = {
#include "stg22/background_bank/gadget_strip_object.inc"
};

struct sign_mesh {
    Sint16 polygons[25];
    Sint32 vertices[19];
    NJS_CNK_MODEL model;
};

/* The exporter kept a local one-texture binding with each panel. The
 * three texture lists registered by the stage are named separately. */
struct single_texture_sign {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    struct sign_mesh mesh;
};

static NJS_TEXNAME sign_panel_00_textures[13] = {
#include "stg22/background_bank/sign_panel_00_textures.inc"
};

NJS_TEXLIST gadget_background_sign_texlist = {
#include "stg22/background_bank/gadget_background_sign_texlist.inc"
};

static struct sign_mesh sign_panel_00_mesh = {
#include "stg22/background_bank/sign_panel_00_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_00 = {
#include "stg22/background_bank/gadget_sign_panel_00.inc"
};

static struct single_texture_sign sign_panel_01_asset = {
#include "stg22/background_bank/sign_panel_01_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_01 = {
#include "stg22/background_bank/gadget_sign_panel_01.inc"
};

static NJS_TEXNAME sign_panel_02_textures[1] = {
#include "stg22/background_bank/sign_panel_02_textures.inc"
};

NJS_TEXLIST gadget_sign_panel_02_texlist = {
#include "stg22/background_bank/gadget_sign_panel_02_texlist.inc"
};

static struct sign_mesh sign_panel_02_mesh = {
#include "stg22/background_bank/sign_panel_02_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_02 = {
#include "stg22/background_bank/gadget_sign_panel_02.inc"
};

static struct single_texture_sign sign_panel_03_asset = {
#include "stg22/background_bank/sign_panel_03_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_03 = {
#include "stg22/background_bank/gadget_sign_panel_03.inc"
};

static NJS_TEXNAME sign_panel_04_textures[1] = {
#include "stg22/background_bank/sign_panel_04_textures.inc"
};

NJS_TEXLIST gadget_sign_panel_04_texlist = {
#include "stg22/background_bank/gadget_sign_panel_04_texlist.inc"
};

static struct sign_mesh sign_panel_04_mesh = {
#include "stg22/background_bank/sign_panel_04_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_04 = {
#include "stg22/background_bank/gadget_sign_panel_04.inc"
};

static struct single_texture_sign sign_panel_05_asset = {
#include "stg22/background_bank/sign_panel_05_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_05 = {
#include "stg22/background_bank/gadget_sign_panel_05.inc"
};

static struct single_texture_sign sign_panel_06_asset = {
#include "stg22/background_bank/sign_panel_06_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_06 = {
#include "stg22/background_bank/gadget_sign_panel_06.inc"
};

static struct single_texture_sign sign_panel_07_asset = {
#include "stg22/background_bank/sign_panel_07_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_07 = {
#include "stg22/background_bank/gadget_sign_panel_07.inc"
};

static struct single_texture_sign sign_panel_08_asset = {
#include "stg22/background_bank/sign_panel_08_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_08 = {
#include "stg22/background_bank/gadget_sign_panel_08.inc"
};

static struct single_texture_sign sign_panel_09_asset = {
#include "stg22/background_bank/sign_panel_09_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_09 = {
#include "stg22/background_bank/gadget_sign_panel_09.inc"
};

static struct single_texture_sign sign_panel_10_asset = {
#include "stg22/background_bank/sign_panel_10_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_10 = {
#include "stg22/background_bank/gadget_sign_panel_10.inc"
};

static struct single_texture_sign sign_panel_11_asset = {
#include "stg22/background_bank/sign_panel_11_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_11 = {
#include "stg22/background_bank/gadget_sign_panel_11.inc"
};

static struct single_texture_sign sign_panel_12_asset = {
#include "stg22/background_bank/sign_panel_12_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_12 = {
#include "stg22/background_bank/gadget_sign_panel_12.inc"
};

static struct single_texture_sign sign_panel_13_asset = {
#include "stg22/background_bank/sign_panel_13_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_13 = {
#include "stg22/background_bank/gadget_sign_panel_13.inc"
};

static struct single_texture_sign sign_panel_14_asset = {
#include "stg22/background_bank/sign_panel_14_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_14 = {
#include "stg22/background_bank/gadget_sign_panel_14.inc"
};

static struct single_texture_sign sign_panel_15_asset = {
#include "stg22/background_bank/sign_panel_15_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_15 = {
#include "stg22/background_bank/gadget_sign_panel_15.inc"
};

static struct single_texture_sign sign_panel_16_asset = {
#include "stg22/background_bank/sign_panel_16_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_16 = {
#include "stg22/background_bank/gadget_sign_panel_16.inc"
};

static struct single_texture_sign sign_panel_17_asset = {
#include "stg22/background_bank/sign_panel_17_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_17 = {
#include "stg22/background_bank/gadget_sign_panel_17.inc"
};

static struct single_texture_sign sign_panel_18_asset = {
#include "stg22/background_bank/sign_panel_18_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_18 = {
#include "stg22/background_bank/gadget_sign_panel_18.inc"
};

static struct single_texture_sign sign_panel_19_asset = {
#include "stg22/background_bank/sign_panel_19_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_19 = {
#include "stg22/background_bank/gadget_sign_panel_19.inc"
};

static struct single_texture_sign sign_panel_20_asset = {
#include "stg22/background_bank/sign_panel_20_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_20 = {
#include "stg22/background_bank/gadget_sign_panel_20.inc"
};

static struct single_texture_sign sign_panel_21_asset = {
#include "stg22/background_bank/sign_panel_21_asset.inc"
};

NJS_CNK_OBJECT gadget_sign_panel_21 = {
#include "stg22/background_bank/gadget_sign_panel_21.inc"
};

static NJS_TEXNAME textures[26] = {
#include "stg22/background_bank/textures.inc"
};

NJS_TEXLIST gadget_background_texlist = {
#include "stg22/background_bank/gadget_background_texlist.inc"
};

static NJS_TEXNAME pulse_a_textures[1] = {
#include "stg22/background_bank/pulse_a_textures.inc"
};

NJS_TEXLIST gadget_pulse_texlist = {
#include "stg22/background_bank/gadget_pulse_texlist.inc"
};

static Sint16 pulse_a_lamp_3_polygons[129] = {
#include "stg22/background_bank/pulse_a_lamp_3_polygons.inc"
};

static Sint32 pulse_a_lamp_3_vertices[147] = {
#include "stg22/background_bank/pulse_a_lamp_3_vertices.inc"
};

static NJS_CNK_MODEL pulse_a_lamp_3_model = {
#include "stg22/background_bank/pulse_a_lamp_3_model.inc"
};

static NJS_CNK_OBJECT pulse_a_lamp_3 = {
#include "stg22/background_bank/pulse_a_lamp_3.inc"
};

static Sint16 pulse_a_lamp_2_polygons[129] = {
#include "stg22/background_bank/pulse_a_lamp_2_polygons.inc"
};

static Sint32 pulse_a_lamp_2_vertices[147] = {
#include "stg22/background_bank/pulse_a_lamp_2_vertices.inc"
};

static NJS_CNK_MODEL pulse_a_lamp_2_model = {
#include "stg22/background_bank/pulse_a_lamp_2_model.inc"
};

static NJS_CNK_OBJECT pulse_a_lamp_2 = {
#include "stg22/background_bank/pulse_a_lamp_2.inc"
};

static Sint16 pulse_a_lamp_1_polygons[129] = {
#include "stg22/background_bank/pulse_a_lamp_1_polygons.inc"
};

static Sint32 pulse_a_lamp_1_vertices[147] = {
#include "stg22/background_bank/pulse_a_lamp_1_vertices.inc"
};

static NJS_CNK_MODEL pulse_a_lamp_1_model = {
#include "stg22/background_bank/pulse_a_lamp_1_model.inc"
};

static NJS_CNK_OBJECT pulse_a_lamp_1 = {
#include "stg22/background_bank/pulse_a_lamp_1.inc"
};

static Sint16 gadget_pulse_a_polygons[129] = {
#include "stg22/background_bank/gadget_pulse_a_polygons.inc"
};

static Sint32 gadget_pulse_a_vertices[147] = {
#include "stg22/background_bank/gadget_pulse_a_vertices.inc"
};

static NJS_CNK_MODEL gadget_pulse_a_model = {
#include "stg22/background_bank/gadget_pulse_a_model.inc"
};

NJS_CNK_OBJECT gadget_pulse_a = {
#include "stg22/background_bank/gadget_pulse_a.inc"
};

NJS_CNK_OBJECT gadget_pulse_a_root = {
#include "stg22/background_bank/gadget_pulse_a_root.inc"
};

static NJS_TEXNAME pulse_b_textures[1] = {
#include "stg22/background_bank/pulse_b_textures.inc"
};

NJS_TEXLIST gadget_pulse_b_texlist = {
#include "stg22/background_bank/gadget_pulse_b_texlist.inc"
};

static Sint16 pulse_b_lamp_3_polygons[91] = {
#include "stg22/background_bank/pulse_b_lamp_3_polygons.inc"
};

static Sint32 pulse_b_lamp_3_vertices[99] = {
#include "stg22/background_bank/pulse_b_lamp_3_vertices.inc"
};

static NJS_CNK_MODEL pulse_b_lamp_3_model = {
#include "stg22/background_bank/pulse_b_lamp_3_model.inc"
};

static NJS_CNK_OBJECT pulse_b_lamp_3 = {
#include "stg22/background_bank/pulse_b_lamp_3.inc"
};

static Sint16 pulse_b_lamp_2_polygons[91] = {
#include "stg22/background_bank/pulse_b_lamp_2_polygons.inc"
};

static Sint32 pulse_b_lamp_2_vertices[99] = {
#include "stg22/background_bank/pulse_b_lamp_2_vertices.inc"
};

static NJS_CNK_MODEL pulse_b_lamp_2_model = {
#include "stg22/background_bank/pulse_b_lamp_2_model.inc"
};

static NJS_CNK_OBJECT pulse_b_lamp_2 = {
#include "stg22/background_bank/pulse_b_lamp_2.inc"
};

static Sint16 pulse_b_lamp_1_polygons[91] = {
#include "stg22/background_bank/pulse_b_lamp_1_polygons.inc"
};

static Sint32 pulse_b_lamp_1_vertices[99] = {
#include "stg22/background_bank/pulse_b_lamp_1_vertices.inc"
};

static NJS_CNK_MODEL pulse_b_lamp_1_model = {
#include "stg22/background_bank/pulse_b_lamp_1_model.inc"
};

static NJS_CNK_OBJECT pulse_b_lamp_1 = {
#include "stg22/background_bank/pulse_b_lamp_1.inc"
};

static Sint16 gadget_pulse_b_polygons[91] = {
#include "stg22/background_bank/gadget_pulse_b_polygons.inc"
};

static Sint32 gadget_pulse_b_vertices[99] = {
#include "stg22/background_bank/gadget_pulse_b_vertices.inc"
};

static NJS_CNK_MODEL gadget_pulse_b_model = {
#include "stg22/background_bank/gadget_pulse_b_model.inc"
};

NJS_CNK_OBJECT gadget_pulse_b = {
#include "stg22/background_bank/gadget_pulse_b.inc"
};

NJS_CNK_OBJECT gadget_pulse_b_root = {
#include "stg22/background_bank/gadget_pulse_b_root.inc"
};

static NJS_TEXNAME glow_texlist_textures[2] = {
#include "stg22/background_bank/glow_texlist_textures.inc"
};

NJS_TEXLIST gadget_glow_texlist = {
#include "stg22/background_bank/gadget_glow_texlist.inc"
};

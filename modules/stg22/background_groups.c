/* bgtex22, the PVM that texture_load_shared loads for the background, and
 * the background's texture lists. each list takes its textures from the PVM
 * by name */
#include <ninja.h>
#include "stg22/background_textures.h"
#include "stg22/stage_data.h"
#include "system/texture.h"

static NJS_TEXLIST *background_group[] = {
    &gadget_strip_texlist,         &gadget_background_sign_texlist,
    &gadget_sign_panel_02_texlist, &gadget_sign_panel_04_texlist,
    &gadget_pulse_texlist,         &gadget_pulse_b_texlist,
    &gadget_sky_texlist,           &gadget_surface_texlist,
    &gadget_glow_texlist,          NULL,
};

NJS_TEXLIST **gadget_background_groups[] = {background_group, NULL};
struct texture_pvm gadget_background_pvms[] = {
    {"bgtex22", &gadget_background_texlist}, {NULL, NULL}};

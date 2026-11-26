/* Eternal Engine's scrolling and flashing scenery, and its two turning skies */
#ifndef STG24_SCENERY_H
#define STG24_SCENERY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/texture_anim.h"
#include "shared/common_object.h"
#include "system/texture.h"

/* a scenery object whose texture scrolls. eternal_engine.c plays animation
 * with texture_anim_apply. no code reads unk_08, which is 0 in every entry */
struct scroll_entry {
    NJS_CNK_OBJECT *object;
    struct texture_anim *animation;
    Sint32 unk_08;
};
extern struct land_table engine_pulse_land;
extern const char engine_pulse_land_filenames[72];
extern struct land_table engine_poison_land;
extern const char engine_poison_land_filenames[221];
extern NJS_CNK_OBJECT engine_scroll_object_0;
extern NJS_CNK_OBJECT engine_scroll_object_1;
extern NJS_CNK_OBJECT engine_scroll_object_2;
extern NJS_CNK_OBJECT engine_scroll_object_3;
extern NJS_CNK_OBJECT engine_scroll_object_4;
extern NJS_CNK_OBJECT engine_scroll_object_5;
extern NJS_CNK_OBJECT engine_scroll_object_6;
extern NJS_CNK_OBJECT engine_scroll_object_7;
extern NJS_CNK_OBJECT engine_scroll_object_8;
extern NJS_CNK_OBJECT engine_scroll_object_9;

extern struct scroll_entry engine_scrolls[];
extern Uint32 engine_scroll_speed;
extern Bool engine_sky_built;
extern struct model_build engine_sky_builds[2];
extern Angle engine_sky_pitch;
extern Angle engine_outer_sky_spin;
extern Angle engine_inner_sky_spin;

extern NJS_TEXLIST engine_background_archive;
extern NJS_TEXLIST **engine_bg_groups[];
extern struct texture_pvm engine_bg_pvms[];
extern NJS_TEXLIST engine_sky_texlist;
extern NJS_CNK_OBJECT engine_outer_sky;
extern NJS_CNK_OBJECT engine_inner_sky;
extern NJS_TEXLIST engine_pulse_texlist;
extern NJS_TEXLIST engine_poison_texlist;

#endif

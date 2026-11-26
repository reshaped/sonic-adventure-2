/* the background of Knuckles's route: its PVM table, texture lists and fogs,
 * and the starts of its tasks */
#ifndef STG38_BACKGROUND_H
#define STG38_BACKGROUND_H
#include "shared/fog.h"
#include "system/texture.h"

struct task;
extern NJS_TEXLIST core_knuckles_background_textures;
extern NJS_TEXLIST *core_knuckles_background_lists[];
extern NJS_TEXLIST **core_knuckles_bg_groups[];
extern struct texture_pvm core_knuckles_bg_pvms[];
extern struct fog_setting core_knuckles_fog;
extern struct fog_setting core_knuckles_upper_fog;
void core_knuckles_background_init(void);
void core_knuckles_hazards_start(void);
void core_knuckles_first_fence_start(void);
void core_knuckles_second_fence_start(void);
extern struct task *core_knuckles_specks;
extern Sint32 core_knuckles_water_side;
/* the side of the water line the background's step last set the fog and
 * the specks for. from WATER_SIDE_UNSET, it sets them for the side it next
 * finds the view on */
enum { WATER_SIDE_UNSET, WATER_SIDE_UNDER, WATER_SIDE_ABOVE };

#endif

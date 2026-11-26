/* the background of Rouge's route: its PVM table, texture lists and fogs, and
 * the starts of its tasks */
#ifndef STG37_BACKGROUND_H
#define STG37_BACKGROUND_H
#include "shared/fog.h"
#include "system/texture.h"

struct task;
extern NJS_TEXLIST core_rouge_background_textures;
extern NJS_TEXLIST *core_rouge_background_lists[];
extern NJS_TEXLIST **core_rouge_bg_groups[];
extern struct texture_pvm core_rouge_bg_pvms[];
extern struct fog_setting core_rouge_fog;
extern struct fog_setting core_rouge_upper_fog;
void core_rouge_background_init(void);
void core_rouge_colliders_start(void);
void core_rouge_camera_start(void);
void core_rouge_cover_start(void);

#endif

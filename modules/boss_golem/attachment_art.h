/* the models and texture lists of the Egg Golem's parts in this fight */
#ifndef BOSS_GOLEM_ATTACHMENT_ART_H
#define BOSS_GOLEM_ATTACHMENT_ART_H

#include <ninja.h>

#include "draw/texture_anim.h"

extern NJS_TEXLIST golem_head_model_texlist;
extern NJS_TEXLIST golem_flash_texlist;
extern NJS_TEXLIST golem_rubble_texlist;
extern NJS_TEXLIST golem_head_platform_a_texlist;
extern NJS_TEXLIST golem_head_platform_b_texlist;
extern NJS_TEXLIST golem_head_platform_c_texlist;
extern NJS_TEXLIST golem_platform_a_texlist;
extern NJS_TEXLIST golem_platform_b_texlist;
extern NJS_TEXLIST golem_platform_c_texlist;
extern NJS_CNK_OBJECT golem_platform_a_object;
extern NJS_CNK_OBJECT golem_platform_b_object;
extern NJS_CNK_OBJECT golem_platform_c_object;
/* @hack type: the flash's node declared as an array, where the node alone
 * would do */
extern NJS_CNK_OBJECT golem_flash_object[];
/* the head model's node, declared as an array, where the node alone would
 * do */
extern NJS_CNK_OBJECT golem_head_model_object[];
extern NJS_CNK_OBJECT golem_head_platform_a_object;
extern NJS_CNK_OBJECT golem_head_platform_b_object;
extern NJS_CNK_OBJECT golem_head_platform_c_object;
extern NJS_CNK_MODEL golem_rubble_model;
extern NJS_TEXLIST golem_object_texlist;
extern struct texture_anim golem_flash_anim;

#endif

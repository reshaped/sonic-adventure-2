/* the cart overlay's entry points, its course's load and release, and the
 * stage hooks it fills */
#ifndef CART_STAGE_H
#define CART_STAGE_H
#include <ninja.h>

#include "object/set.h"

struct hud_number_cell;
struct icon_rect;
struct task;

/* the HUD's cells, which race_hud.c names */
#define CART_HUD_SPRITES 34
extern NJS_TEXANIM cart_hud_sprites[CART_HUD_SPRITES];
extern struct object_kind cart_kinds[13];
extern struct object_list cart_objects;
/* the entry points head.c gives the stage. cart_step loads the course at its
 * first step */
void cart_init(void);
void cart_exit(void);
void cart_step(struct task *task);
/* keep mode, which nothing reads: CART_MODE_START from cart_init, and
 * CART_MODE_CHOSEN once cart_debug_menu takes a row */
enum { CART_MODE_START, CART_MODE_CHOSEN };
void cart_set_mode(int mode);
/* the debug menu of play modes. no code calls it */
void cart_debug_menu(void);
/* load the course of cart_setup.course_mode */
void cart_load_course(void);
/* release the course. mode is a play mode, and CART_PLAY_STORY releases a
 * story race's life icon and chase textures too */
void cart_unload_course(int mode);
/* stage_hooks.action_icon and number_cell, which give the UVs of their
 * sprites of cart_hud_sprites */
void cart_action_icon(struct icon_rect *rect);
void cart_number_cell(struct hud_number_cell *cell);
#endif

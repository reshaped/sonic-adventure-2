/* the transfer machine's models and motions, which machine_art.c's tables
 * pick by garden */
#ifndef CHAO_MACHINE_MODELS_H
#define CHAO_MACHINE_MODELS_H
#include <ninja.h>

extern NJS_CNK_OBJECT machine_neutral_base;
extern NJS_CNK_OBJECT machine_neutral_button;
extern NJS_CNK_OBJECT machine_neutral_door;
extern NJS_CNK_OBJECT machine_neutral_body;
extern NJS_CNK_OBJECT machine_hero_base;
extern NJS_CNK_OBJECT machine_hero_button;
extern NJS_CNK_OBJECT machine_hero_trim_object;
extern NJS_CNK_OBJECT machine_hero_body;
extern NJS_CNK_OBJECT machine_dark_base;
extern NJS_CNK_OBJECT machine_dark_button;
extern NJS_CNK_OBJECT machine_dark_body;
extern NJS_MOTION machine_hero_door_motion;
extern NJS_MOTION machine_hero_trim_motion;
extern NJS_MOTION machine_dark_door_motion;

#endif

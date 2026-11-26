/* the transfer machine's textures, models and motions, indexed by garden:
 * neutral, hero and dark. a null entry is a part that garden's machine
 * lacks */
#ifndef CHAO_MACHINE_ART_H
#define CHAO_MACHINE_ART_H
#include <ninja.h>
#include <sg_xpt.h>

extern NJS_TEXLIST *machine_textures[3];
extern NJS_CNK_OBJECT *machine_models[3];
extern NJS_MOTION *machine_door_motions[3];
extern NJS_CNK_OBJECT *machine_hero_trim[3];
extern NJS_MOTION *machine_hero_trim_motions[3];
extern Uint16 machine_screen_textures[3][3];
extern NJS_CNK_OBJECT *machine_buttons[3];
extern NJS_CNK_OBJECT *machine_doors[3];
extern NJS_CNK_OBJECT *machine_bases[3];

#endif

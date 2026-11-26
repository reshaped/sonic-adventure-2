/* the art of the G.U.N. beetle's kinds, and the actions and shapes beetle.c
 * gives them */
#ifndef SHARED_BEETLE_ART_H
#define SHARED_BEETLE_ART_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"

extern const char beetle_texture_filenames[131];

enum beetle_action_id {
    BEETLE_ACTION_HOVER,
    BEETLE_ACTION_BOMBER,
    BEETLE_ACTION_GUNNER,
    BEETLE_ACTION_GUNNER_FIRE,
    BEETLE_ACTION_BUMPER,
    BEETLE_ACTION_BUMPER_BUMP,
    BEETLE_ACTION_COUNT
};

#define BEETLE_DISCHARGE_FRAMES 9

/* @hack type: the four kinds' objects declared as arrays of one, where each
 * object alone would do */
extern NJS_TEXLIST beetle_texlist;
extern NJS_CNK_OBJECT beetle_object[1];
extern NJS_CNK_MODEL *beetle_pieces[];

extern NJS_TEXLIST gunner_texlist;
extern NJS_CNK_OBJECT gunner_object[1];
extern NJS_CNK_MODEL *gunner_pieces[];

extern NJS_TEXLIST bomber_texlist;
extern NJS_CNK_OBJECT bomber_object[1];
extern NJS_CNK_MODEL *bomber_pieces[];

extern NJS_TEXLIST bumper_texlist;
extern NJS_CNK_OBJECT bumper_object[1];
extern NJS_CNK_MODEL *bumper_pieces[];

extern NJS_TEXLIST electric_texlist;
extern NJS_TEXLIST discharge_surface_texlist;
extern NJS_CNK_MODEL discharge_model;
extern NJS_CNK_OBJECT discharge_object;
extern struct texture_anim discharge_anim;
extern NJS_TEXLIST discharge_texlists[BEETLE_DISCHARGE_FRAMES];

extern struct collision_shape beetle_shapes[4];
extern struct enemy_action beetle_actions[BEETLE_ACTION_COUNT];

/* the strip headers a warp beetle's draw marks translucent, with
 * NJD_FST_UA, while it appears or vanishes. each _STRIP is a header's index
 * in its polygon list, counted in Sint16s */
extern Sint16 beetle_canopy_polygons[];
#define BEETLE_CANOPY_LIGHT_STRIP 50
#define BEETLE_CANOPY_SHELL_STRIP 144
extern Sint16 beetle_lower_fin_polygons[];
#define BEETLE_LOWER_FIN_STRIP 10
extern Sint16 beetle_rotor_polygons[];
#define BEETLE_ROTOR_STRIP 10
extern Sint16 beetle_body_polygons[];
#define BEETLE_BODY_BASE_STRIP 10
#define BEETLE_BODY_SHELL_STRIP 264
extern Sint16 beetle_rotor_disc_polygons[];
#define BEETLE_ROTOR_DISC_STRIP 8

#endif

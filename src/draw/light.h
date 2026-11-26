/* the lights the game hands to Ninja's chunk and direct drawers */
#ifndef DRAW_LIGHT_H
#define DRAW_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/game_state.h"

#define LIGHTS 12
/* the light the land draws in, which light_init makes light_after_draw */
#define LIGHT_LAND 2
/* the light a DEBRIS_LIGHT piece of shared/debris.h draws in. the meteors of
 * Final Rush, Meteor Herd, Final Chase and Cosmic Wall draw in it too */
#define LIGHT_DEBRIS 3
/* a light under which a player's draw sets no DRAW_CONTROL_SHADOW, as under
 * LIGHT_VOLUME */
#define LIGHT_UNK_1 1
/* the light a player takes inside a global light switch's volume. at a win's
 * end, the stage may give it to the winner and the loser it places. each
 * placing gives it in place of light_camera where it tests for
 * STAGE_RADICAL_HIGHWAY or STAGE_LOST_COLONY. no player's draw sets
 * DRAW_CONTROL_SHADOW under it, so no shadow falls on the player's body */
#define LIGHT_VOLUME 5
/* the light enemies and effects draw in, and compile their models in */
#define LIGHT_EFFECT 9

/* a light: its direction, how bright it and the ambient light are, and its
 * color */
struct light {
    NJS_VECTOR dir;
    Float intensity;
    Float ambient;
    Float r, g, b;
};

extern struct light lights[LIGHTS];
/* the index in lights of the light camera_apply and camera_apply_light hand
 * Ninja. the stage selects that light for the players it places */
extern Sint8 light_camera;
/* the index in lights of the light a draw hands Ninja as it ends. only a
 * draw with a light of its own does so */
extern Sint8 light_after_draw;
/* the index in lights of each player's light, and the index light_save last
 * kept for each */
extern Sint8 light_selected[GAME_PLAYERS];
extern Sint8 light_saved[GAME_PLAYERS];

/* set light_camera to 0 and light_after_draw to LIGHT_LAND, and give both
 * players light 0, selected and saved. the lights themselves are left as they
 * were */
void light_init(void);
/* light_set copies *light into the light at index. light_set_ambient sets
 * only that light's ambient. index must be below LIGHTS, unchecked */
void light_set(int index, const struct light *light);
void light_set_ambient(int index, Float ambient);
/* light_select_camera sets light_camera to index, and light_select_after_draw
 * sets light_after_draw to index */
void light_select_camera(Sint8 index);
void light_select_after_draw(Sint8 index);
/* make the light at index player's light. only player's low bit is read, and
 * light_selected keeps index's low byte */
void light_select(int index, int player);
/* hand Ninja the light player has selected, or the light at index. Ninja's
 * direct, easy and simple lights all take it, its direction turned by the
 * current matrix. only player's low bit is read */
void light_apply_player(int player);
void light_apply(int index);

/* pass (x, y, z) to njCnkSetEasyLight or njCnkSetSimpleLight.
 * src/camera/camera.c defines both */
void easy_light_set_direction(Float x, Float y, Float z);
void simple_light_set_direction(Float x, Float y, Float z);

#endif

/* the carrier stages' deck fixtures: RADER, LIGHT, LIGHTW, SIGN and WALL */
#ifndef SHARED_DECK_SCENERY_H
#define SHARED_DECK_SCENERY_H
#include <ninja.h>

struct task;

/* each fixture's texture lists, null-ended, which the carrier stages' texture
 * groups hold */
extern NJS_TEXLIST *deck_radar_texlists[];
extern NJS_TEXLIST *deck_wall_texlists[];
extern NJS_TEXLIST *deck_light_texlists[];
extern NJS_TEXLIST *deck_sign_texlists[];

/* the fixtures' inits. RADER turns its dish. LIGHT's and LIGHTW's lamps pulse
 * with each task's own timer. SIGN's panel blinks with game_state.steps, so
 * every sign blinks together */
void deck_radar_init(struct task *task);
void deck_light_init(struct task *task);
void deck_light_w_init(struct task *task);
void deck_sign_init(struct task *task);
/* WALL has a fixed box centered 20 units above its set position */
void deck_wall_init(struct task *task);

#endif

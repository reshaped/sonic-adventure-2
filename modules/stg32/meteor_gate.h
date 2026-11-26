#ifndef STG32_METEOR_GATE_H
#define STG32_METEOR_GATE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST *meteor_container_textures[];
extern NJS_TEXLIST *large_meteor_textures[];
extern NJS_TEXLIST *meteor_shutter_textures[];

/* MDCONTBOX, a solid box whose lamp blinks. where METEOBIG's sphere touches
 * it, it breaks into pieces and scores 20. pos places it, and ang.y turns it */
void meteor_container_init(struct task *task);
/* METEOBIG, a meteor that a player's attack can push. it breaks apart where it
 * hits a wall. it also breaks apart the step after hits that hold no player's
 * attack, unless the first is a player's task. it starts 5 above pos. a player
 * within 180 of pos at its init leaves it with no step or draw */
void large_meteor_init(struct task *task);
/* METEOSHUT, a solid gate whose two lamps blink. where METEOBIG's sphere
 * touches it, it breaks, and draws and collides as its broken model from then
 * on. pos places it, and ang.y turns it */
void meteor_shutter_init(struct task *task);

/* BIGSHUTTER, an iris that opens while the switch that ang.x's low byte numbers
 * is on. it removes itself once fully open */
extern NJS_TEXLIST *herd_iris_textures[];
void herd_iris_init(struct task *task);

#endif

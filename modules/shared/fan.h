/* the fan, FAN in Wild Canyon's object list: a stand whose blades turn. it
 * comes bright or dark, and plays a sound while a player is near */
#ifndef SHARED_FAN_H
#define SHARED_FAN_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the kinds' blades and
 * stands. each is in the kinds' order and ended by null */
extern NJS_TEXLIST *fan_blades_texlists[];
extern NJS_TEXLIST *fan_stand_texlists[];

/* the object list's step for FAN. unless half_try_leave or
 * placement_try_leave returns TRUE for task, it gives task the fan's own
 * step, draw and destructor.
 *
 * task needs a work. pos places the fan, and ang turns it by njRotateY, then
 * njRotateX. each step adds ang.z to the blades' turn about z. the fan draws
 * at 1 + scl.y in x and y, and 1 + scl.z in z. the whole part of |scl.x|,
 * modulo 2, picks the kind: 0 for bright, 1 for dark.
 *
 * the fan plays sound 8 at pos in each step that a player lies within 200 of
 * it. player 1 counts only in a two-player game. the players' works must not
 * be null, unchecked */
void fan_init(struct task *task);

#endif

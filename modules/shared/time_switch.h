/* the time switch, CC_SWITCH in the object lists of Cannon's Core's routes.
 * every switch shares one countdown. time stops while the countdown has steps
 * left: game_state.time_stopped then holds player 1's bit */
#ifndef SHARED_TIME_SWITCH_H
#define SHARED_TIME_SWITCH_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST time_switch_base_texlist;
extern NJS_TEXLIST time_switch_cap_texlist;
extern NJS_TEXLIST time_switch_effect_texlist;
extern NJS_TEXLIST *time_switch_texlists[];
/* the object list's step for CC_SWITCH. striking the switch's target or
 * pressing its cap sets the countdown to the placement's scl.z steps. after a
 * strike, the countdown runs at once. after a press, it waits until the cap
 * has been still for 10 steps */
void time_switch_step(struct task *task);

#endif

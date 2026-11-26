/* the four-valve tank, TANK in Eternal Engine's object list. it bursts once
 * all four of its valves are knocked off. its burst and the capsule bomb's
 * blast shake a pad with vibration_pulses_create */
#ifndef SHARED_FOUR_VALVE_TANK_H
#define SHARED_FOUR_VALVE_TANK_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the tank, its pieces and its
 * dust, ended by null */
extern NJS_TEXLIST *four_valve_tank_texlists[];

/* the object list's step for TANK. it does nothing where task leaves at its
 * placement's range. where the heap can't give the tank's spring, it does
 * nothing, and the next step tries again. otherwise it gives task the tank's
 * step, draw and destructor, and a collision record of one capsule. where the
 * heap can't give the record, the tank goes on without one.
 *
 * task needs a work. pos places the tank, and ang.y turns its body about y.
 * ang.y turns the valves about a line along z instead. so the valves keep to
 * the body only where ang.y is 0. each later step tests the range as the
 * first did.
 *
 * task gets a child at each valve still on, which an attack can knock off. a
 * valve the heap can't give a task never comes off. bit n of the placement's
 * state is set where valve n is off. the tank starts with those valves off,
 * and its destructor records the valves that are off then.
 *
 * with all four valves off, the tank bursts. the burst scores 20 points,
 * throws the tank's 15 pieces and shakes player 0's pad twice. it ends the
 * task */
void four_valve_tank_init(struct task *task);
/* shake player's pad pulses times, interval steps apart. each pulse calls
 * vibration_start with power and freq, and a time of 0 for no stop time. a
 * task named TaVibExec starts the first pulse at its first step, and ends
 * after the last. where task_create fails, nothing shakes. pulses must be from
 * 1 to 127, and interval from 1 to 255, unchecked */
void vibration_pulses_create(int player, int power, int pulses, int interval,
                             int freq);

#endif

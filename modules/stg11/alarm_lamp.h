/* the alarm lamp, SIREN in Iron Gate's object list, whose head turns. it is
 * the lamp of shared/alarm_lamp.h, compiled again */
#ifndef STG11_ALARM_LAMP_H
#define STG11_ALARM_LAMP_H
#include <ninja.h>

struct task;

/* the lamp's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *iron_gate_alarm_lamp_texlists[];

/* the object list's step for SIREN. it does what alarm_lamp_init does, but
 * the draw turns Ninja's fog off for the head. it then turns the fog on,
 * whether or not it was on before */
void iron_gate_alarm_lamp_init(struct task *task);

#endif

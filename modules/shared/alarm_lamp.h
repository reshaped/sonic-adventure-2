/* the alarm lamp, SIREN in Prison Lane's object list, whose head turns.
 * stg11/alarm_lamp.h is the same object compiled a second time */
#ifndef SHARED_ALARM_LAMP_H
#define SHARED_ALARM_LAMP_H
#include <ninja.h>

struct task;

/* the lamp's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *alarm_lamp_texlists[];

/* the object list's step for SIREN. task needs a work, whose pos places the
 * lamp and whose ang turns it. the work's timer holds the head's heading,
 * which starts at random. where the heap has no room for the lamp's
 * collision record, the lamp goes on without one */
void alarm_lamp_init(struct task *task);

#endif

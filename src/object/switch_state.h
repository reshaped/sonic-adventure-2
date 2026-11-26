/* the stage's switches, which fall.c defines and zeroes, and switch.c sets
 * and reads */
#ifndef OBJECT_SWITCH_STATE_H
#define OBJECT_SWITCH_STATE_H
#include <sg_xpt.h>

#define SWITCH_IDS 32

/* a byte for each switch ID. its SWITCH_ON bit is set while the switch is
 * on. its SWITCH_WRITTEN bit marks a write, which a SWITCH_TIMED switch then
 * clears */
extern Uint8 stage_switch_flags[SWITCH_IDS];
#define SWITCH_ON 0x1
#define SWITCH_WRITTEN 0x2

#endif

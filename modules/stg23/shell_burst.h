/* the shell burst, a blast model that grows for 30 steps */
#ifndef STG23_SHELL_BURST_H
#define STG23_SHELL_BURST_H
#include <ninja.h>

struct task;

/* start a burst at pos: the blast model, which grows ever slower for 30 steps
 * and then ends. returns the burst's task, or null where the heap has no room
 * for the task or its record. no code calls it */
struct task *base_shell_burst_create(NJS_POINT3 *pos);

#endif

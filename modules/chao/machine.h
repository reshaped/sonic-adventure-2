/* the garden's transfer machine: its task, or null while there is none, and
 * whether it holds the player. while it does, the garden's camera follows
 * the machine */
#ifndef CHAO_MACHINE_H
#define CHAO_MACHINE_H
#include <sg_xpt.h>
struct task;
extern struct task *garden_transfer_machine;
extern Sint32 garden_machine_in_use;
#endif

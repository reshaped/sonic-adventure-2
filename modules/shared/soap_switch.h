/* the soap switch, SOAP SW in a stage's object list. once a player has
 * passed through it, it sets its placement's state. it draws nothing */
#ifndef SHARED_SOAP_SWITCH_H
#define SHARED_SOAP_SWITCH_H

struct task;

/* the object list's step for SOAP SW. task needs a work, whose pos places
 * the switch. the low bit of ang.z picks its shape. 0 picks a box of half
 * sizes scl, and 1 a sphere of radius scl.x. each size is cut toward 0 to a
 * whole number. the call clears ang.x and the low bytes of ang.y and ang.z.
 * ang.y and then ang.z turn the box. the heap must have room for the
 * switch's collision record: without it, the call reads through null.
 *
 * the switch's work takes the id WORK_ID_SOAP_SWITCH, which Sonic's moves
 * look for. once a player has touched the switch and left it, the switch
 * sets its placement's state to 1 */
void soap_switch_init(struct task *task);

#endif

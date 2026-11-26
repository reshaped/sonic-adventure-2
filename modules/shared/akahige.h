#ifndef SHARED_AKAHIGE_H
#define SHARED_AKAHIGE_H

struct task;

/* the akahige's step, which keeps hornet_step's contract but for what
 * follows. its kinds are HORNET_3 and HORNET_6 alone. a whole part of scl.x
 * outside them makes a HORNET_3. it clears its placement's used mark as it
 * sets up, so it always carries bombs. a bomb that a walker's lock holds as it
 * dies leaves the mark as it is. while time is stopped, its loose bombs still
 * drift and it still sends out jets. its attack then still ends once a bomb
 * starts to chase */
void akahige_step(struct task *task);

#endif

#ifndef EVENT_TAILS_H
#define EVENT_TAILS_H

/* Tails' two tails, as the scene file's tails[0] holds them.
 * event_tails_start puts a bending jiggle's copy in each tail's place, and
 * event_tails_step bends both a step. event_tails_stop frees the jiggles and
 * their copies. the model mustn't draw again until the scene file is read anew.
 * each does nothing while tails[0] is null */
void event_tails_start(void);
void event_tails_stop(void);
void event_tails_step(void);

#endif

/* the event player's state, which its script and its debug controls
 * share */
#ifndef EVENT_PLAYBACK_H
#define EVENT_PLAYBACK_H
#include <sg_xpt.h>

#include "event/lighting.h"

#define EVENT_LIGHTS 4

/* Big plays his scene's motions in turn, each once player 0 presses A.
 * event_big_motion is the one he is on. it is -1 once a scene that doesn't
 * loop has played its last. event_big_frame is his frame in it, which runs
 * only while event_big_triggered is set */
extern Float event_big_frame;
extern Sint32 event_big_motion;
extern Bool event_big_triggered;
extern struct event_fog_cue event_script_fog;
extern struct event_lighting event_lights[EVENT_LIGHTS];
extern struct event_fade event_foreground, event_background;
extern Sint32 event_sequence_index;
extern Bool event_free_camera;
extern Bool event_reset_pressed;
#endif

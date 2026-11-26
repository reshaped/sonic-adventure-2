/* the screen effect: an animated model drawn under a camera motion of its
 * own. it shows and hides at once as its work's shown changes */
#ifndef ADVERTISE_SCREEN_EFFECT_H
#define ADVERTISE_SCREEN_EFFECT_H
#include <sg_xpt.h>

#include "advertise/menus.h"

struct screen_effect_anim {
    Float level; /* 0 while hidden, and 1 while shown */
    Float frame; /* the frame of the model's motion */
};
extern struct screen_effect_anim screen_effect_anim;
/* menu_works.work_17 points here. */
extern struct screen_effect_work screen_effect_work;

/* create the task, screenEffectExec, with the work cleared and the effect
 * hidden at its motion's first frame. the task's destructor releases the
 * texture */
void screen_effect_create(void);
/* clear the work but loaded, and hide the effect at once, back at its
 * motion's first frame */
void screen_effect_reset(void);
/* load the texture, or release it, and set loaded to match. neither does
 * anything where loaded matches already */
void screen_effect_load(void);
void screen_effect_release(void);

#endif

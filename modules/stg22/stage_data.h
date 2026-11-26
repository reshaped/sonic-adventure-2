#ifndef STG22_STAGE_DATA_H
#define STG22_STAGE_DATA_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct path;
struct task;

extern struct path *crazy_gadget_paths[];
extern struct land_table crazy_gadget_land;
extern const char crazy_gadget_land_filenames[752];
extern NJS_TEXLIST gadget_land_texlist;
extern NJS_CNK_MODEL gadget_pulse_marker_a;
extern NJS_CNK_MODEL gadget_pulse_marker_b;

extern NJS_TEXLIST **gadget_background_groups[];
extern struct texture_pvm gadget_background_pvms[];
extern NJS_TEXLIST **gadget_object_groups[];
extern struct texture_pvm gadget_object_pvms[];
extern struct set_head *gadget_set;
extern struct task *gadget_background_task;
extern struct task *gadget_event_task;

/* the step of the event task that crazy_gadget_step creates, with a work.
 * while the task has no child, the step makes the rails under it. in the
 * hero story it plays the stage's voices */
void gadget_events_step(struct task *task);
/* the step of the background task that crazy_gadget_init creates, with a
 * work. it animates the land's surfaces, and makes player 0 fall out in a
 * kill volume. it lays the glow under player 0, glow.h's GADGET_GLOW(task),
 * which it allocates while that is null */
void gadget_background_step(struct task *task);
/* the background task's draw: the kill volumes under the debug display, then
 * the scrolling land, the moving strips, the pulses, the signs, the glow and
 * the sky. it compiles the pulses into WORK_AREA on each call */
void gadget_background_draw(struct task *task);
/* the background task's destructor. it frees the glow, and clears
 * gadget_background_task where that names task */
void gadget_background_destroy(struct task *task);

#endif

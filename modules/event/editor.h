#ifndef EVENT_EDITOR_H
#define EVENT_EDITOR_H
#include <sg_xpt.h>

/* the debug editors of the event's sprites and of its model bursts, which the
 * second controller drives. cursor is the burst editor's selected field */
enum event_editor_mode { EDIT_OFF, EDIT_SPRITES, EDIT_BURSTS };
struct event_editor {
    enum event_editor_mode mode;
    Sint32 cursor;
};
extern struct event_editor event_editor;
/* whether the event starts again at its last frame, rather than finishing */
extern Bool event_loop;

/* close the editor and the free camera. while a sequence runs, it also stops
 * the debug print */
void event_editor_close(void);
/* step the open editor. with none open, it reads the button pairs that open
 * one or switch a debug setting. it does nothing while a sequence runs */
void event_editor_step(void);
void event_editor_draw_sprite(void);
void event_editor_draw_burst(void);
/* move the free camera by the second controller, and make it Ninja's camera */
void event_editor_set_camera(void);
#endif

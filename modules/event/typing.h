#ifndef EVENT_TYPING_H
#define EVENT_TYPING_H
#include <sg_xpt.h>

/* the typed line: the first line i of event_messages whose
 * event_tracks->lines[i].frame is EVENT_TYPED_FRAME. that entry's duration
 * holds the x of the line's left edge on the screen, in pixels */
#define EVENT_TYPED_FRAME (-1)

/* find the typed line and ready it to type from its start. FALSE where the
 * event has none. event_typing_step needs a start that returned TRUE */
Bool event_typing_start(void);
/* step the line and draw it as typed so far. TRUE once the whole line has
 * shown and every sprite it drew is destroyed */
Bool event_typing_step(void);
#ifdef NONMATCHING
/* this side's call for a skip that comes while the line types. end the
 * line where it stands: it types no more, and event_typing_step returns
 * TRUE once its sprites are destroyed, as after its last character */
void event_typing_end(void);
#endif
#endif

#ifndef EVENT_EQUIPMENT_H
#define EVENT_EQUIPMENT_H
#include <ninja.h>

/* hide the parts of the loaded event that the save's upgrades rule out. an
 * upgrade the save has hides the part it replaces, and one it lacks hides
 * its own parts */
void event_hide_unequipped(void);
/* event_attachments_reset forgets the nodes seen the frame before. between
 * event_attachments_watch and event_attachments_unwatch, a draw records the
 * matrix of each node an attachment rides. event_attachments_draw then draws
 * each attachment the save's upgrades show there, so it comes after the
 * owners' draws */
void event_attachments_reset(void);
void event_attachments_watch(NJS_CNK_OBJECT *owner);
void event_attachments_unwatch(void);
void event_attachments_draw(void);

#endif

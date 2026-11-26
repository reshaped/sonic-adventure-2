/* the search, in GCC's build, of a messages file for one event's list */
#ifndef EVENT_MESSAGE_LIST_H
#define EVENT_MESSAGE_LIST_H
#include <sg_xpt.h>

#include "event/file_span.h"
#include "event/messages.h"

/* find event's list in the messages file at bank, bytes long. the file must
 * be an array of struct message_list, ended within bytes by one with a
 * negative event. the first list for event may hold at most limit messages.
 * its array of messages must be 4-byte aligned and lie within the file. each
 * message's text must end within the file. no other list is checked.
 *
 * it returns TRUE with *selected that list, or null where the file holds
 * none for event. it returns FALSE with *selected null where the file or the
 * list breaks a rule above */
static Bool event_message_list(void *bank, Uint32 bytes, int event,
                               Uint32 limit, struct message_list **selected)
{
    struct message_list *list = bank;
    struct message_list *found = NULL;
    Uint32 left = bytes;
    Sint32 i;

    *selected = NULL;
    while (left >= sizeof(list->event)) {
        if (list->event < 0) {
            *selected = found;
            return TRUE;
        }
        if (left < sizeof(*list)) {
            return FALSE;
        }
        if (list->event == event && found == NULL) {
            if ((Uint32)list->count > limit ||
                (list->count &&
                 (((uintptr_t)list->messages & 3) ||
                  (Uint32)list->count >
                      event_file_remaining(bank, bytes, list->messages) /
                          sizeof(*list->messages)))) {
                return FALSE;
            }
            for (i = 0; i < list->count; i++) {
                const char *text = list->messages[i].text;
                Uint32 room = event_file_remaining(bank, bytes, text);

                while (room && *text != '\0') {
                    room--;
                    text++;
                }
                if (!room) {
                    return FALSE;
                }
            }
            found = list;
        }
        left -= sizeof(*list);
        list++;
    }
    return FALSE;
}

#endif

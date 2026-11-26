/* the subtitles of full and mini events, as their language files hold them,
 * and their lines' voices */
#ifndef EVENT_MESSAGES_H
#define EVENT_MESSAGES_H
#include <sg_xpt.h>

struct message {
    char unk_00[4];
    char *text;
};

/* a language file's list for one event. a list with a negative event ends
 * the file */
struct message_list {
    Sint32 event;
    struct message *messages;
    Sint32 count;
};

/* the voice of an event's line, in event_voices. key is event * EVENT_KEYS
 * plus the line, and voice the number voice_play_priority_2 plays. a key of
 * EVENT_VOICES_END ends the table */
struct event_voice {
    Sint32 key, voice;
};
extern struct event_voice event_voices[];
#define EVENT_KEYS 1000
#define EVENT_VOICES_END (-1)
#endif

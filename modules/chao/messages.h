/* the messages of the Chao areas: a file of texts in game_state's text
 * language */
#ifndef CHAO_MESSAGES_H
#define CHAO_MESSAGES_H
#include <sg_xpt.h>

struct text_sprite;

/* read the messages from name, with the text language's suffix and .prs
 * added, and return the file. it opens with a table of byte offsets from its
 * start, one for each message. name holds at most 249 bytes. a read that
 * fails returns NULL and keeps the file there was. one that succeeds doesn't
 * free that file: call garden_messages_free first */
const Sint32 *garden_messages_load(const char *name);
void garden_messages_free(void);
/* message id's text, which lasts until garden_messages_free, or NULL where no
 * file is read */
const char *garden_message(int id);
/* a text sprite of text in the text language. where width and height are both
 * given, they get the text's size in pixels */
struct text_sprite *garden_message_sprite(const char *text, Uint16 *width,
                                          Uint16 *height);

#endif

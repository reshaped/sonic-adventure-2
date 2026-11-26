/* message sprites: a message or a text drawn once, by a text sprite of its
 * own */
#ifndef SHARED_MESSAGE_SPRITE_H
#define SHARED_MESSAGE_SPRITE_H
#include <ninja.h>
#include <sg_xpt.h>

struct text_sprite;

/* the messages: for each language of system/text.h, a table of texts by
 * message number. shared/messages/tables.c defines it */
struct message_tables {
    char **japanese;
    char **english;
    char **french;
    char **spanish;
    char **german;
};
extern struct message_tables message_tables;

/* in the calls below, message indexes message_tables' table for
 * game_state.text_language, which must be one of system/text.h's languages.
 * only message_sprite_text and message_sprite_count check it */

/* message's text, or null where the language is none of system/text.h's */
char *message_sprite_text(int message);
/* make a sprite of message's text, as text_sprite_create does */
struct text_sprite *message_sprite_create(int message);
/* draw a sprite of message's text once, in white. the text's top left corner
 * lands at pos's x and y. a text whose first character is '\a' or '\t' is
 * centered on the screen's x of 320 instead. each of the texture's pixels is
 * scale a side. a task destroys the sprite on the second step after the call,
 * or never where task_create fails. where text_sprite_create fails, the call
 * reads through null */
void message_sprite_draw(int message, const NJS_POINT3 *pos, Float scale);
/* message_sprite_draw of text. message_sprite_draw_text_color draws it in
 * *color, or in white where color is null */
void message_sprite_draw_text(const char *text, const NJS_POINT3 *pos,
                              Float scale);
void message_sprite_draw_text_color(const char *text, const NJS_POINT3 *pos,
                                    Float scale, const Uint32 *color);
/* message_sprite_draw, and set the sprite's band color to a translucent
 * blue, which the draw doesn't show. where text_sprite_create fails, the call
 * writes through null */
void message_sprite_draw_set_band(int message, const NJS_POINT3 *pos,
                                  Float scale);
/* the count of message's bytes equal to character, or 0 where the language is
 * none of system/text.h's */
int message_sprite_count(int message, char character);

#endif

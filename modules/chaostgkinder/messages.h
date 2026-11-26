/* the kindergarten's messages, in two banks. a bank holds a script and a
 * string file. a script is a table of entries, one for each line of text.
 * a message is the index of its first line's entry. an entry's text
 * indexes the string file's opening table. the table holds each string's byte
 * offset from the file's start */
#ifndef CHAOSTGKINDER_MESSAGES_H
#define CHAOSTGKINDER_MESSAGES_H
#include <ninja.h>
#include <string.h>

struct task;

/* the banks. KINDER_WHOLE_BANK holds the kindergarten's own messages, from
 * msgChaoKinWhole. KINDER_SERVICE_BANK holds the doctor's or the principal's,
 * which their rooms load */
enum kinder_message_bank { KINDER_WHOLE_BANK, KINDER_SERVICE_BANK };

/* an entry's next is the message of its next line, or one of these.
 * KINDER_MESSAGE_CONTINUE makes the entry after it the next line.
 * KINDER_MESSAGE_PAGE ends the page, and the entry after it starts the next
 * page. KINDER_MESSAGE_END and KINDER_MESSAGE_STOP end the message */
enum kinder_message_link {
    KINDER_MESSAGE_STOP = -2,
    KINDER_MESSAGE_END = -1,
    KINDER_MESSAGE_CONTINUE = 0x1000,
    KINDER_MESSAGE_PAGE = 0x2000
};

struct kinder_message_entry {
    Sint16 text, next;
};

/* a message that names no entry. kinder_dialog_set clears the dialog for it */
#define KINDER_NO_MESSAGE (-1)

enum kinder_message_label_flags {
    KINDER_MESSAGE_LABEL_TRACK = 1,
    KINDER_MESSAGE_LABEL_CENTER = 2,
    KINDER_MESSAGE_LABEL_LITERAL = 4,
    KINDER_MESSAGE_LABEL_FIT = 8
};

/* a label's text is the page that starts at message in bank, or text under
 * KINDER_MESSAGE_LABEL_LITERAL. under KINDER_MESSAGE_LABEL_TRACK, the label
 * is selected while *selection is id, and draws in selected_color then. under
 * KINDER_MESSAGE_LABEL_FIT, it shrinks to fit width. under
 * KINDER_MESSAGE_LABEL_CENTER, pos is its center. the colors are 0xAARRGGBB,
 * and pos.z is the depth */
struct kinder_message_label {
    Sint8 bank;
    Uint8 flags, id;
    Uint32 color, selected_color;
    int message;
    const Uint8 *selection;
    NJS_POINT3 pos;
    Float scale, width;
    const char *text;
};

/* load the string file name, with the language's suffix and .prs, into
 * bank, after kinder_messages_release empties it. returns the strings, or
 * NULL where the file doesn't load */
Sint32 *kinder_messages_load(const char *name, Sint8 bank);
/* free bank's strings, and forget its script, which the bank doesn't own */
void kinder_messages_release(Sint8 bank);
/* make kinder_default_script KINDER_WHOLE_BANK's script */
void kinder_message_reset(void);
/* the shared sprite is the one kinder_message_label_page's labels draw.
 * kinder_message_texture_load frees it and makes it anew from
 * kinder_placeholder_text, and kinder_message_texture_release frees it */
void kinder_message_texture_load(void);
void kinder_message_texture_release(void);
/* bank's strings or script, or NULL where it has none */
Sint32 *kinder_string_bank(Sint8 bank);
struct kinder_message_entry *kinder_script_bank(Sint8 bank);
/* the index of message's string, or -1 where bank has no script */
int kinder_message_text_index(int message, Sint8 bank);
/* the message of the line after message's. it is KINDER_NO_MESSAGE where
 * message's line ends its page, or where bank has no script */
int kinder_message_next_line(int message, Sint8 bank);
/* queue the page that starts at message for the dialog to show after the
 * ones it holds. a dialog holds four pages, and drops one past them */
void kinder_dialog_append(Sint16 message, Sint8 bank);
/* the text of message's line, or NULL where bank lacks its script or its
 * strings */
char *kinder_message_text(int message, Sint8 bank);
/* the message that starts the page after message's, or KINDER_NO_MESSAGE
 * where there is none */
int kinder_message_next_page(int message, Sint8 bank);
/* make entries bank's script, which the bank doesn't own */
void kinder_message_table(struct kinder_message_entry *entries, Sint8 bank);
/* the dialog, a band across the screen's foot that shows a page of a
 * message. kinder_dialog_create makes it parent's child, unless it exists
 * already or parent is NULL. the dialog starts idle, so delay and style
 * change nothing. every other kinder_dialog_ call needs the dialog to exist */
void kinder_dialog_create(struct task *parent, Sint16 delay, int style);
/* show the page that starts at message, or clear the dialog where message is
 * negative */
void kinder_dialog_set(Sint16 message, Sint8 bank);
/* show and hide the blinking arrow that asks the player to go on */
void kinder_dialog_show_arrow(void);
void kinder_dialog_hide_arrow(void);
/* show a copy of text, length bytes long, as the dialog's page */
void kinder_dialog_set_text(const char *text, size_t length);
/* a label of caption, as a child of parent. returns its task, or NULL where
 * the label can't be made */
struct task *
kinder_message_label_create(struct task *parent,
                            const struct kinder_message_label *caption);
/* a label as kinder_message_label_create makes it, which fades in. it holds
 * until kinder_message_label_fade_out fades it out and kills it. under
 * KINDER_MESSAGE_LABEL_TRACK, it is selected while *selection is id plus
 * selection_offset */
struct task *
kinder_message_label_fade_in(struct task *parent,
                             const struct kinder_message_label *caption,
                             Uint8 selection_offset);
/* the size a label of kinder_message_label_create or
 * kinder_message_label_page takes on the screen, in pixels. a label without
 * a sprite takes 0 by 0 */
void kinder_message_label_size(struct task *task, Uint16 *width,
                               Uint16 *height);
/* fade out a label kinder_message_label_fade_in made, and kill it */
void kinder_message_label_fade_out(struct task *caption);
/* make *result a copy of text with insert in place of its marker, and
 * return TRUE. text must hold the marker, its first '<' and the byte after
 * it. returns FALSE where the copy can't be allocated, and *result is then
 * NULL. the caller frees *result with syFree */
int kinder_message_insert(const char *text, const char *insert, char **result);
/* a label as kinder_message_label_create makes it, of the page that starts
 * at message. the page becomes the shared sprite, and killing the label
 * makes the placeholder's sprite again. its characters lie 2 pixels apart
 * where the page then measures under 512 pixels wide. where it doesn't, they
 * lie 1 apart, and where that doesn't fit either, 0. where 0 doesn't fit,
 * they lie 2 apart */
struct task *
kinder_message_label_page(struct task *parent,
                          const struct kinder_message_label *caption);

#endif

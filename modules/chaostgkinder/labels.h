/* the kindergarten's labels. a label is a task that draws one of the room
 * captions in the game's text language */
#ifndef CHAOSTGKINDER_LABELS_H
#define CHAOSTGKINDER_LABELS_H
#include <sg_xpt.h>

struct task;
struct text_sprite;

/* the room captions, in the order of the corridor's slots */
enum kinder_caption {
    KINDER_CAPTION_EXIT,
    KINDER_CAPTION_BLACK_MARKET,
    KINDER_CAPTION_CLASSROOM,
    KINDER_CAPTION_HEALTH_CENTER,
    KINDER_CAPTION_LIBRARY,
    KINDER_CAPTION_PRINCIPAL,
    KINDER_CAPTION_PLAYGROUND,
    KINDER_CAPTION_BULLETIN_BOARD,
    KINDER_CAPTIONS
};

/* a label's caption text, its sprite and its place on the screen. flags
 * holds the KINDER_LABEL_ bits. choice is the answer a label holds, which
 * kinder_label_create sets to 0. no code sets KINDER_LABEL_ACCEPT or
 * KINDER_LABEL_CANCEL */
struct kinder_label {
    const char *text;
    struct text_sprite *sprite;
    Sint32 flags;
    Float x, y;
    Sint32 choice;
};

#define KINDER_LABEL_ACCEPT 1
#define KINDER_LABEL_CANCEL 2
#define KINDER_LABEL_VISIBLE 4 /* the label draws while it is set */
/* kinder_label_create sets it, and nothing reads it */
#define KINDER_LABEL_UNK_8 8

/* make a label of caption, below KINDER_CAPTIONS, as a child task of
 * parent. it shows at x, y on the screen. returns the label's task. where an
 * allocation fails, it frees what it made and returns NULL */
struct task *kinder_label_create(struct task *parent, int caption, int x,
                                 int y);
/* the label of a task kinder_label_create made */
struct kinder_label *kinder_label_data(struct task *task);

#endif

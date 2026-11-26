#include <shinobi.h>
#include <sg_xpt.h>

#include "chaostgkinder/labels.h"
#include "draw/text_sprite.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"

#include "chaostgkinder/labels/text.inc"

/* each language's room captions. the Japanese ones are Shift JIS. the
 * others are Latin-1, and empty for the black market and the playground */
typedef const char *room_caption;
typedef const room_caption *room_caption_table;

static const room_caption room_labels_ja[KINDER_CAPTIONS] = {
#include "chaostgkinder/labels/room_labels_ja.inc"
};
static const room_caption room_labels_en[KINDER_CAPTIONS] = {
#include "chaostgkinder/labels/room_labels_en.inc"
};
static const room_caption room_labels_fr[KINDER_CAPTIONS] = {
#include "chaostgkinder/labels/room_labels_fr.inc"
};
static const room_caption room_labels_es[KINDER_CAPTIONS] = {
#include "chaostgkinder/labels/room_labels_es.inc"
};
static const room_caption room_labels_de[KINDER_CAPTIONS] = {
#include "chaostgkinder/labels/room_labels_de.inc"
};
static const room_caption_table room_labels[TEXT_LANGUAGES] = {
#include "chaostgkinder/labels/room_labels.inc"
};

/* a label's task work, with the label where task_work holds ang.x */
struct label_work {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    struct kinder_label *label;
};
#define LABEL_WORK(task) ((struct label_work *)(task)->work)
enum label_mode { LABEL_START, LABEL_RUN };

static void label_destroy(struct task *task)
{
    /* @hack spelling: the local work, where LABEL_WORK(task)->label would
     * do */
    struct label_work *work = LABEL_WORK(task);
    struct kinder_label *label = work->label;

    if (label) {
        if (label->sprite) {
            text_sprite_destroy(label->sprite);
        }
        syFree(label);
    }
}

static void label_draw(struct task *task)
{
    /* @hack spelling: the local work, where LABEL_WORK(task)->label would
     * do */
    struct label_work *work = LABEL_WORK(task);
    struct kinder_label *label = work->label;

    if (label->flags & KINDER_LABEL_VISIBLE) {
        /* @hack spelling: the local sprite, where passing label->sprite
         * would do */
        struct text_sprite *sprite = label->sprite;
        text_sprite_draw_all(sprite);
    }
}

static void label_step(struct task *task)
{
    struct label_work *work = LABEL_WORK(task);

    /* @hack spelling: a switch of one case, where an if would do */
    switch (work->mode) {
    case LABEL_START:
        task->draw = label_draw;
        task->destructor = label_destroy;
        work->mode++;
        break;
    }
}

struct task *kinder_label_create(struct task *parent, int caption, int x, int y)
{
    struct task *task;
    struct text_sprite *sprite;
    struct kinder_label *label;

    /* @hack spelling: label and sprite set to NULL after their
     * declarations, where initializers would do */
    label = NULL;
    sprite = NULL;
    /* @hack spelling: each assignment inside its test, where a statement of
     * its own would do */
    if (!(task = task_create_child(TASK_WORK, label_step, parent))) {
        goto failed;
    }
    if (!(label = syMalloc(sizeof(*label)))) {
        goto failed;
    }
    label->text = room_labels[game_state.text_language][caption];
    if (!(sprite = text_sprite_create(label->text, game_state.text_language))) {
        goto failed;
    }
    LABEL_WORK(task)->label = label;
    label->sprite = sprite;
    label->flags = KINDER_LABEL_VISIBLE | KINDER_LABEL_UNK_8;
    label->x = x;
    label->y = y;
    label->choice = 0;
    sprite->x = x;
    sprite->y = y;
    sprite->band_color.color = 0;
    return task;

failed:
    if (sprite) {
        text_sprite_destroy(sprite);
    }
    if (label) {
        syFree(label);
    }
    if (task) {
        task_kill(task);
    }
    return NULL;
}

struct kinder_label *kinder_label_data(struct task *task)
{
    return LABEL_WORK(task)->label;
}

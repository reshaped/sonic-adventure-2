#include <sg_xpt.h>

#include "chaostgkinder/form.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/widget.h"
#include "system/task.h"

enum prompt_mode {
    PROMPT_CLOSED = -1,
    PROMPT_START,
    PROMPT_CREATE,
    PROMPT_SHOWN
};
enum prompt_button { PROMPT_EDIT, PROMPT_LEAVE };
#define CORRIDOR_FROM_LIBRARY 4

static const struct form_text_desc buttons[2] = {
#include "chaostgkinder/book_prompt/buttons.inc"
};
static const struct form_desc choices[2] = {
#include "chaostgkinder/book_prompt/choices.inc"
};
static const struct form_panel_desc panel = {
#include "chaostgkinder/book_prompt/panel.inc"
};
static const struct form_desc prompt_desc = {FORM_PANEL, &panel};
static struct kinder_form *prompt = NULL;

static void on_prompt(void *owner, struct kinder_form *sender, Sint32 event,
                      Uint32 id)
{
    struct task *task = owner;
    struct task_work *work = task->work;
    /* @hack spelling: each close's store through task->work, where work
     * would do */
    switch (work->mode) {
    case PROMPT_SHOWN:
        switch (event) {
        case WIDGET_ACCEPT:
            switch (id) {
            case PROMPT_EDIT:
                prompt->ops->destroy(prompt);
                prompt = NULL;
                task->work->mode = PROMPT_CLOSED;
                kinder_request_room(KINDER_EDITOR, 0);
                break;
            case PROMPT_LEAVE:
                prompt->ops->destroy(prompt);
                prompt = NULL;
                task->work->mode = PROMPT_CLOSED;
                kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_LIBRARY);
                break;
            }
            break;
        case WIDGET_CANCEL:
            prompt->ops->destroy(prompt);
            prompt = NULL;
            task->work->mode = PROMPT_CLOSED;
            kinder_request_room(KINDER_CORRIDOR, CORRIDOR_FROM_LIBRARY);
            break;
        case WIDGET_DOWN:
        case WIDGET_UP:
            switch (id) {
            case PROMPT_EDIT:
                form_panel_focus_id(prompt, PROMPT_LEAVE);
                break;
            case PROMPT_LEAVE:
                form_panel_focus_id(prompt, PROMPT_EDIT);
                break;
            }
            break;
        }
        break;
    }
}

static void prompt_destroy(struct task *task)
{
    if (prompt) {
        prompt->ops->destroy(prompt);
        prompt = NULL;
    }
}

static void prompt_draw(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case PROMPT_SHOWN:
        if (prompt) {
            prompt->ops->draw(prompt, 0.0F, 0.0F, 0.0F);
        }
        break;
    }
}

void kinder_book_prompt_step(struct task *task)
{
    struct task_work *work = task->work;
    /* mode as an int, so that case 0xff lies in the switch's range */
    switch ((int)work->mode) {
    case PROMPT_START:
        task->draw = prompt_draw;
        task->destructor = prompt_destroy;
        prompt = NULL;
        work->mode = PROMPT_CREATE;
        break;
    case PROMPT_CREATE:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((prompt = kinder_form_create(NULL, &prompt_desc)) != NULL) {
            form_panel_set_callback(prompt, on_prompt, task);
            work->mode = PROMPT_SHOWN;
        }
        break;
    case PROMPT_SHOWN:
        kinder_form_poll(prompt);
        if (prompt) {
            prompt->ops->tick(prompt);
        }
        break;
    /* @hack redundant: case 0xff, which mode, a Sint8, never equals */
    case 0xff:
        break;
    }
}

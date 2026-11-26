#include <sg_xpt.h>
#include "chaostgkinder/connection_notice.h"
#include "chaostgkinder/widget.h"
#include "system/task.h"

enum connection_notice_state {
    NOTICE_CLOSED = 0,
    NOTICE_WAIT = 1,
    NOTICE_CONFIRM = 2,
    NOTICE_CANCELLED = 3,
    NOTICE_DISABLED = 5
};
enum confirm_control { CONFIRM_YES = 1, CONFIRM_NO };
static int wait_event(void *owner, struct kinder_widget *widget, int event);
static int accept_cancel(void *owner, struct kinder_widget *widget, int event);
static int keep_waiting(void *owner, struct kinder_widget *widget, int event);
static int confirm_event(void *owner, struct kinder_widget *widget, int event);
static int ignore_cancel(void *owner, struct kinder_widget *widget, int event);

#pragma section ATA
static struct kinder_rect selected[6] = {
#include "chaostgkinder/connection_notice/selected.inc"
};
static struct kinder_rect unselected[3] = {
#include "chaostgkinder/connection_notice/unselected.inc"
};
static struct kinder_label_desc waiting = {
#include "chaostgkinder/connection_notice/waiting.inc"
};
static struct kinder_widget_desc wait_label = {
#include "chaostgkinder/connection_notice/wait_label.inc"
};
static struct kinder_rect wait_fill = {
#include "chaostgkinder/connection_notice/wait_fill.inc"
};
static struct kinder_container_desc wait_panel = {
#include "chaostgkinder/connection_notice/wait_panel.inc"
};
static struct kinder_widget_desc wait_dialog = {
#include "chaostgkinder/connection_notice/wait_dialog.inc"
};
static struct kinder_button_desc yes_style = {
#include "chaostgkinder/connection_notice/yes_style.inc"
};
static struct kinder_widget_desc yes = {
#include "chaostgkinder/connection_notice/yes.inc"
};
static struct kinder_button_desc no_style = {
#include "chaostgkinder/connection_notice/no_style.inc"
};
static struct kinder_widget_desc no = {
#include "chaostgkinder/connection_notice/no.inc"
};
static struct kinder_label_desc confirm = {
#include "chaostgkinder/connection_notice/confirm.inc"
};
static struct kinder_widget_desc confirm_label = {
#include "chaostgkinder/connection_notice/confirm_label.inc"
};
static struct kinder_rect confirm_fill = {
#include "chaostgkinder/connection_notice/confirm_fill.inc"
};
static struct kinder_container_desc confirm_panel = {
#include "chaostgkinder/connection_notice/confirm_panel.inc"
};
static struct kinder_widget_desc confirm_dialog = {
#include "chaostgkinder/connection_notice/confirm_dialog.inc"
};

/* a second wait notice, which a cancel can't close. its notify takes the
 * cancel and does nothing with it */
static struct kinder_label_desc fixed_wait_text = {
#include "chaostgkinder/connection_notice/fixed_wait_text.inc"
};
static struct kinder_widget_desc fixed_wait_label = {
    WIDGET_LABEL, 0, 0, 0, 1, 240, 32, &fixed_wait_text, NULL};
static struct kinder_rect fixed_wait_fill = {
#include "chaostgkinder/connection_notice/fixed_wait_fill.inc"
};
static struct kinder_container_desc fixed_wait_panel = {
#include "chaostgkinder/connection_notice/fixed_wait_panel.inc"
};
static struct kinder_widget_desc fixed_wait_dialog = {
    WIDGET_CONTAINER, 0, 200, 128, 600, 240, 32, &fixed_wait_panel,
    ignore_cancel};
#pragma section

static Sint32 state; /* a connection_notice_state */
static struct kinder_widget *wait_window;
static struct kinder_widget *confirm_window;

static void show_wait(void);
static void show_confirm(void);

static int wait_event(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        kinder_widget_remove(wait_window);
        wait_window->ops->destroy(wait_window);
        show_confirm();
        handled = TRUE;
        break;
    }
    return handled;
}

static void show_wait(void)
{
    wait_window = kinder_widget_create(&wait_dialog);
#ifdef NONMATCHING
    /* this side closes the notice where kinder_widget_create makes no window,
     * which the matching side goes on with */
    if (!wait_window) {
        state = NOTICE_CLOSED;
        return;
    }
#endif
    kinder_widget_add(wait_window, &wait_label);
    kinder_widget_push(wait_window);
    state = NOTICE_WAIT;
}

static int accept_cancel(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        kinder_widget_remove(confirm_window);
        confirm_window->ops->destroy(confirm_window);
        state = NOTICE_CANCELLED;
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        kinder_widget_select(widget->parent, CONFIRM_NO);
        handled = TRUE;
        break;
    }
    return handled;
}

static int keep_waiting(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        kinder_widget_remove(confirm_window);
        confirm_window->ops->destroy(confirm_window);
        show_wait();
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        kinder_widget_select(widget->parent, CONFIRM_YES);
        handled = TRUE;
        break;
    }
    return handled;
}

static int confirm_event(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        kinder_widget_remove(confirm_window);
        confirm_window->ops->destroy(confirm_window);
        state = NOTICE_CLOSED;
        handled = TRUE;
        break;
    }
    return handled;
}

static void show_confirm(void)
{
    confirm_window = kinder_widget_create(&confirm_dialog);
#ifdef NONMATCHING
    /* this side closes the notice where kinder_widget_create makes no window,
     * which the matching side goes on with */
    if (!confirm_window) {
        state = NOTICE_CLOSED;
        return;
    }
#endif
    kinder_widget_add(confirm_window, &yes);
    kinder_widget_add(confirm_window, &no);
    kinder_widget_add(confirm_window, &confirm_label);
    kinder_widget_select(confirm_window, CONFIRM_NO);
    kinder_widget_push(confirm_window);
    state = NOTICE_CONFIRM;
}

static int ignore_cancel(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        handled = TRUE;
        break;
    }
    return handled;
}
/* @bug logic: a menu that ends while the notice shows leaves it on the
 * window stack, which goes on drawing it. notice_destroy, at 0x0c9de2be,
 * closes nothing. connect_account leaves for MENU_DISCONNECTED with the
 * notice open, at 0x0c9d56dc and 0x0c9d5756 */
static void notice_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the destroy closes whichever
     * window is open */
    if (state == NOTICE_CONFIRM) {
        kinder_widget_remove(confirm_window);
        confirm_window->ops->destroy(confirm_window);
    } else {
        connection_notice_close();
    }
    state = NOTICE_CLOSED;
#endif
}
static void notice_draw(struct task *task)
{
}
static void notice_step(struct task *task)
{
    if (!task->destructor) {
        task->draw = notice_draw;
        task->destructor = notice_destroy;
    }
}
void connection_notice_start(struct task *parent)
{
    task_create_child(0, notice_step, parent);
    state = NOTICE_CLOSED;
}
/* @hack type: external linkage for connection_notice_disable, which no
 * code calls, where static would do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void connection_notice_disable(void)
{
    state = NOTICE_DISABLED;
}
void connection_notice_open(void)
{
    switch (state) {
    case NOTICE_CLOSED:
        show_wait();
        break;
    case NOTICE_WAIT:
        break;
    }
}
void connection_notice_close(void)
{
    switch (state) {
    case NOTICE_WAIT:
        kinder_widget_remove(wait_window);
        wait_window->ops->destroy(wait_window);
        state = NOTICE_CLOSED;
        break;
    }
}
Bool connection_notice_cancelled(void)
{
    return state == NOTICE_CANCELLED;
}

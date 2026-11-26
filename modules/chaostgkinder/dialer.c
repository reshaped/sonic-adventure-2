#include <sg_xpt.h>
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/network.h"
#include "chaostgkinder/service_menu.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"

enum dial_stage {
    DIAL_START,
    DIAL_NEXT_NUMBER,
    DIAL_WAIT_LINE,
    DIAL_WAIT_PPP,
    DIAL_CONNECTED,
    DIAL_FAILED
};
enum dial_string {
    DIAL_NOTICE = 652,
    DIAL_TITLE,
    DIAL_NUMBER,
    DIAL_ANSWER,
    DIAL_SUCCESS,
    DIAL_FAILURE
};

static int dialog_event(void *owner, struct kinder_widget *widget, int event);
static int accept_notice(void *owner, struct kinder_widget *widget, int event);
static int decline_notice(void *owner, struct kinder_widget *widget, int event);
static int cancel_notice(void *owner, struct kinder_widget *widget, int event);

static const struct kinder_rect selected_face[15] = {
#include "chaostgkinder/dialer/selected_face.inc"
};
static const struct kinder_rect normal_face[7] = {
#include "chaostgkinder/dialer/normal_face.inc"
};
static const struct kinder_label_desc title_text = {
#include "chaostgkinder/dialer/title_text.inc"
};
static const struct kinder_widget_desc title = {
#include "chaostgkinder/dialer/title.inc"
};
static const struct kinder_label_desc status_text = {
#include "chaostgkinder/dialer/status_text.inc"
};
static const struct kinder_widget_desc status = {
#include "chaostgkinder/dialer/status.inc"
};
static const struct kinder_rect dialog_fill = {
#include "chaostgkinder/dialer/dialog_fill.inc"
};
static const struct kinder_container_desc dialog_panel = {
#include "chaostgkinder/dialer/dialog_panel.inc"
};
static const struct kinder_widget_desc dial_dialog = {
#include "chaostgkinder/dialer/dial_dialog.inc"
};
static const struct kinder_label_desc notice_text = {
#include "chaostgkinder/dialer/notice_text.inc"
};
static const struct kinder_widget_desc notice_label = {
#include "chaostgkinder/dialer/notice_label.inc"
};
static const struct kinder_button_desc yes_style = {
#include "chaostgkinder/dialer/yes_style.inc"
};
static const struct kinder_widget_desc yes = {
#include "chaostgkinder/dialer/yes.inc"
};
static const struct kinder_button_desc no_style = {
#include "chaostgkinder/dialer/no_style.inc"
};
static const struct kinder_widget_desc no = {
#include "chaostgkinder/dialer/no.inc"
};
static const struct kinder_rect notice_fill = {
#include "chaostgkinder/dialer/notice_fill.inc"
};
static const struct kinder_container_desc notice_panel = {
#include "chaostgkinder/dialer/notice_panel.inc"
};
static const struct kinder_widget_desc notice_dialog = {
#include "chaostgkinder/dialer/notice_dialog.inc"
};
#define STATUS_ID 1
#define YES_ID 1
#define NO_ID 2
#define CONNECTED_FRAMES 60
/* the characters the status line's text holds, its terminator included */
#define STATUS_TEXT_SIZE 128

static Sint32 *reply;
/* an enum dial_stage */
static Sint32 stage;
/* the number dialed, then the frames left before a connected dialog closes */
static Sint32 attempt;
static Uint16 number_text[STATUS_TEXT_SIZE];
static struct kinder_widget *dialog;
static struct kinder_widget *notice;

static void dial_number(int number);
static void report_failure(void);
static void report_connection(void);
static void report_answer(void);

static int dialog_event(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_TICK:
        switch (stage) {
        case DIAL_START:
            attempt = -1;
            stage = DIAL_NEXT_NUMBER;
            break;
        case DIAL_NEXT_NUMBER:
            while (++attempt < ISP_PHONE_NUMBERS) {
                if (*kinder_connection_number(attempt)) {
                    dial_number(attempt);
                    stage = DIAL_WAIT_LINE;
                    /* @hack spelling: the goto past report_failure, where
                     * testing attempt after the loop would do */
                    goto number_found;
                }
            }
            report_failure();
        number_found:
            break;
        case DIAL_WAIT_LINE:
            switch (kinder_connection_state()) {
            case CONNECTION_DIALING:
                break;
            case CONNECTION_ONLINE:
            case CONNECTION_NEGOTIATING:
                report_answer();
                break;
            default:
                kinder_connection_hang_up();
                stage = DIAL_NEXT_NUMBER;
                break;
            }
            break;
        case DIAL_WAIT_PPP:
            switch (kinder_connection_state()) {
            case CONNECTION_NEGOTIATING:
                break;
            case CONNECTION_ONLINE:
                kinder_resolver_start();
                report_connection();
                break;
            default:
                report_failure();
                break;
            }
            break;
        case DIAL_CONNECTED:
            if (--attempt == 0) {
                *reply = DIALER_CONNECTED;
                kinder_widget_remove(dialog);
                dialog->ops->destroy(dialog);
            }
            break;
        case DIAL_FAILED:
            break;
        }
        break;
    case WIDGET_CANCEL:
        /* @hack spelling: a switch of one case, where an if would do */
        switch (stage) {
        case DIAL_FAILED:
            *reply = DIALER_CANCELLED;
            kinder_widget_remove(dialog);
            dialog->ops->destroy(dialog);
            handled = TRUE;
            break;
        }
        break;
    }
    return handled;
}
static void show_dialer(void)
{
    stage = DIAL_START;
    dialog = kinder_widget_create(&dial_dialog);
#ifdef NONMATCHING
    /* this side cancels the dial where kinder_widget_create makes no dialog,
     * which the matching side goes on with */
    if (!dialog) {
        *reply = DIALER_CANCELLED;
        return;
    }
#endif
    kinder_widget_add(dialog, &title);
    kinder_widget_add(dialog, &status);
    kinder_widget_push(dialog);
}
static void dial_number(int number)
{
    struct kinder_widget *label;
    kinder_connection_dial(number);
    label = kinder_widget_find(dialog, STATUS_ID);
    kinder_text_format(number_text, kinder_string(DIAL_NUMBER),
                       kinder_connection_number(number));
    kinder_text_set(label, number_text);
}
static void report_failure(void)
{
    struct kinder_widget *label;
    stage = DIAL_FAILED;
    label = kinder_widget_find(dialog, STATUS_ID);
    kinder_text_set(label, kinder_string(DIAL_FAILURE));
}
static void report_connection(void)
{
    struct kinder_widget *label;
    stage = DIAL_CONNECTED;
    label = kinder_widget_find(dialog, STATUS_ID);
    kinder_text_set(label, kinder_string(DIAL_SUCCESS));
    attempt = CONNECTED_FRAMES;
}
static void report_answer(void)
{
    struct kinder_widget *label;
    stage = DIAL_WAIT_PPP;
    label = kinder_widget_find(dialog, STATUS_ID);
    kinder_text_set(label, kinder_string(DIAL_ANSWER));
}
static int accept_notice(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        kinder_widget_remove(notice);
        notice->ops->destroy(notice);
        show_dialer();
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        kinder_widget_select(widget->parent, NO_ID);
        handled = TRUE;
        break;
    }
    return handled;
}
static int decline_notice(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    /* @hack spelling: one local for both the reply and handled, where
     * DIALER_CANCELLED and TRUE would do */
    int cancelled = DIALER_CANCELLED;
    switch (event) {
    case WIDGET_ACCEPT:
        *reply = cancelled;
        kinder_widget_remove(notice);
        notice->ops->destroy(notice);
        handled = cancelled;
        break;
    case WIDGET_LEFT:
        kinder_widget_select(widget->parent, YES_ID);
        handled = cancelled;
        break;
    }
    return handled;
}
static int cancel_notice(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        *reply = DIALER_CANCELLED;
        kinder_widget_remove(notice);
        notice->ops->destroy(notice);
        handled = TRUE;
        break;
    }
    return handled;
}
static void show_notice(void)
{
    switch (kinder_connection_state()) {
    default:
    case CONNECTION_IDLE:
        *reply = DIALER_PENDING;
        notice = kinder_widget_create(&notice_dialog);
#ifdef NONMATCHING
        /* this side cancels the dial where kinder_widget_create makes no
         * notice, which the matching side goes on with */
        if (!notice) {
            *reply = DIALER_CANCELLED;
            return;
        }
#endif
        kinder_widget_add(notice, &notice_label);
        kinder_widget_add(notice, &yes);
        kinder_widget_focus(notice, kinder_widget_add(notice, &no));
        kinder_widget_push(notice);
        break;
    case CONNECTION_DIALING:
    case CONNECTION_NEGOTIATING:
    case CONNECTION_ONLINE:
        *reply = DIALER_CONNECTED;
        break;
    }
}
void kinder_dialer_open(Sint32 *result)
{
    reply = result;
    show_notice();
}

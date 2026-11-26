#include <sg_xpt.h>
#include <string.h>
#include "chaostgkinder/account_packet.h"
#include "chaostgkinder/account_profile_state.h"
#include "chaostgkinder/account_regions.h"
#include "chaostgkinder/account_service.h"
#include "chaostgkinder/address_service.h"
#include "chaostgkinder/connection_notice.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/network.h"
#include "chaostgkinder/services.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"
#include "system/task.h"

enum account_screen {
    MENU_START,
    MENU_ISP,
    MENU_CONFIRM_MAIL,
    MENU_CONNECT,
    MENU_USER_NAME,
    MENU_CHECK_NAME,
    MENU_PROFILE_NAME,
    MENU_CONTACT,
    MENU_VISIBILITY,
    MENU_LOCATION,
    MENU_CONFIRM_CREATE,
    MENU_CREATE,
    MENU_SUCCESS,
    MENU_NAME_TAKEN,
    MENU_EXISTING,
    MENU_FIND,
    MENU_MISSING,
    MENU_CONFIRM_REMOVE,
    MENU_REMOVE,
    MENU_REMOVED,
    MENU_READ,
    MENU_EDIT,
    MENU_WRITE,
    MENU_NO_ISP,
    MENU_DISCONNECTED,
    MENU_ERROR,
    MENU_EXIT
};
static struct task *menu_task = NULL, *account_task = NULL,
                   *address_task = NULL;
/* the location dialog's lists. choose_location, refresh_districts and
 * refresh_regions set a list's entries and count before they add it */
static struct kinder_list_desc country_choice_style = {
#include "chaostgkinder/account_menu/country_choice_style.inc"
};

static struct kinder_list_desc district_choice_style = {
#include "chaostgkinder/account_menu/district_choice_style.inc"
};

static struct kinder_list_desc region_choice_style = {
#include "chaostgkinder/account_menu/region_choice_style.inc"
};

/* a button draws its _focus rects while focused, and its _normal ones
 * otherwise. the yes and no buttons of the mail, submit and remove dialogs
 * draw the confirm_ pair. every other button draws the action_ pair */
static const struct kinder_rect confirm_focus[6] = {
#include "chaostgkinder/account_menu/confirm_focus.inc"
};

static const struct kinder_rect confirm_normal[3] = {
#include "chaostgkinder/account_menu/confirm_normal.inc"
};

static const struct kinder_rect action_focus[6] = {
#include "chaostgkinder/account_menu/action_focus.inc"
};

static const struct kinder_rect action_normal[3] = {
#include "chaostgkinder/account_menu/action_normal.inc"
};

static const struct kinder_rect account_short_field = {
#include "chaostgkinder/account_menu/account_short_field.inc"
};

static const struct kinder_rect account_short_inset[6] = {
#include "chaostgkinder/account_menu/account_short_inset.inc"
};

static const struct kinder_rect account_short_rows[6] = {
#include "chaostgkinder/account_menu/account_short_rows.inc"
};

/* a dialog's control holds its number in its id's top byte */
#define CONTROL_NUMBER_SHIFT 24
#define CONTROL(n) ((n) << CONTROL_NUMBER_SHIFT)
#include "chaostgkinder/account_layout.h"

/* the numbers of each dialog's controls, as account_layout.h numbers them */
enum mail_control { MAIL_YES = 2, MAIL_NO };
enum user_control { USER_FIELD = 1, USER_NEXT, USER_QUIT };
enum profile_control { PROFILE_FIELD = 1, PROFILE_NEXT };
enum contact_control { CONTACT_FIELD = 1, CONTACT_NEXT };
enum visibility_control { VISIBILITY_YES = 1, VISIBILITY_NO };

/* a screen opens at PHASE_OPEN, and PUSH moves it to PHASE_POLL */
enum screen_phase { PHASE_OPEN, PHASE_POLL };
/* the codes a dialog's formatted text has room for */
#define MESSAGE_CODES 256

static enum account_menu_result result;
static enum account_screen screen;
static Sint32 phase;
static enum account_screen previous_screen;
static struct address_request endpoint;
static struct account_mail_request mail_request;
static Uint16 user_name[ACCOUNT_NAME_CODES];
static char profile_name[ACCOUNT_PROFILE_NAME_BYTES],
    isp_mail[ACCOUNT_CONTACT_BYTES], contact[ACCOUNT_CONTACT_BYTES];
static Sint32 user_id, profile_id;
static Bool visible;
static Uint16 country_key, region_key;
static Uint16 message[MESSAGE_CODES];
static struct kinder_widget *mail_window, *user_window;
static struct account_name_request name_request;
static struct kinder_widget *profile_window, *contact_window,
    *visibility_window;
static Sint32 location_depth, country, region, district;
static struct kinder_widget *location_window, *submit_window;
static struct account_create_request create_request;
static struct kinder_widget *success_window, *taken_window, *found_window;
#define GO_TO(next)                                                            \
    do {                                                                       \
        screen = (next);                                                       \
        phase = PHASE_OPEN;                                                    \
    } while (0)
static void quit_account(int status)
{
    result = status;
    if (menu_task) {
        task_kill(menu_task);
        menu_task = NULL;
    }
}
#define CLOSE(window)                                                          \
    do {                                                                       \
        kinder_widget_remove(window);                                          \
        (window)->ops->destroy(window);                                        \
    } while (0)
#define ADD(window, desc) kinder_widget_add((window), &(desc))
#define FOCUS(window, desc) kinder_widget_focus((window), ADD(window, desc))
#define FIND(window, number) kinder_widget_find((window), CONTROL(number))
#define SELECT(parent, number) kinder_widget_select((parent), CONTROL(number))
#ifdef NONMATCHING
/* this side closes the account menu where kinder_widget_create makes no window,
 * which the matching side goes on with */
#define OPEN(window, desc)                                                     \
    do {                                                                       \
        (window) = kinder_widget_create(&(desc));                              \
        if (!(window)) {                                                       \
            quit_account(MENU_CLOSED);                                         \
            return;                                                            \
        }                                                                      \
    } while (0)
#else
#define OPEN(window, desc) (window) = kinder_widget_create(&(desc))
#endif
#define PUSH(window)                                                           \
    do {                                                                       \
        kinder_widget_push(window);                                            \
        phase = PHASE_POLL;                                                    \
    } while (0)

static void leave_user(int next);
static void leave_profile(int next);
static void leave_contact(int next);
static void leave_visibility(int next);
static void refresh_districts(void);
static void refresh_regions(void);
static void leave_location(int next);
static void leave_submit(int next);
static void leave_success(int next);
static void leave_taken(int next);
static void leave_existing(int next);
static void leave_missing(int next);

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side by pack_field. it packs
 * as kinder_text_pack_n does, but size counts bytes, the terminator's included.
 * a code that would leave the terminator no room ends the text */
static void pack_field(const Uint16 *text, char *dst, int size)
{
    Uint16 code;
    while ((code = *text++) != 0 && size > (code > 255 ? 2 : 1)) {
        if (code > 255) {
            *dst++ = code >> 8;
            --size;
        }
        *dst++ = (char)code;
        --size;
    }
    *dst = 0;
}
#define PACK_FIELD pack_field
#else
/* @bug memory: a field of kana packs past the end of profile_name or
 * contact. kinder_text_pack_n counts codes, not bytes, and a code past 255
 * packs as two. the profile field's 12 codes take up to 25 bytes, past
 * profile_name's 17. the contact field's 48 take up to 97, past contact's 49.
 * leave_profile at 0x0c9d5c18 and leave_contact at 0x0c9d5dda pack them */
#define PACK_FIELD kinder_text_pack_n
#endif

#ifdef NONMATCHING
/* this side goes to MENU_ERROR where a request isn't sent, which the matching
 * side ignores */
#define SUBMIT(request)                                                        \
    do {                                                                       \
        if (!(request)) {                                                      \
            GO_TO(MENU_ERROR);                                                 \
            return;                                                            \
        }                                                                      \
    } while (0)
#else
#define SUBMIT(request) (request)
#endif

static void read_isp(void)
{
    if (kinder_isp_result) {
        GO_TO(MENU_NO_ISP);
    } else {
        memset(isp_mail, 0, sizeof(isp_mail));
        /* @bug logic: a player who connects by the second ISP account is
         * looked up by the first account's mail. read_isp at 0x0c9d53c8 and
         * connect_account at 0x0c9d573e read kinder_isp_accounts[0], where
         * enter_contact at 0x0c9d5d70 and the dialer read the account
         * kinder_selected_isp names */
        strncpy(isp_mail, kinder_isp_accounts[0].mail, sizeof(isp_mail) - 1);
        GO_TO(MENU_CONFIRM_MAIL);
    }
}
static int mail_yes_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_RIGHT:
        SELECT(mail_window, MAIL_NO);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        CLOSE(mail_window);
        GO_TO(MENU_CONNECT);
        handled = TRUE;
        break;
    }
    return handled;
}
static int mail_no_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_LEFT:
        SELECT(mail_window, MAIL_YES);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        CLOSE(mail_window);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static int mail_dialog_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        CLOSE(mail_window);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void confirm_mail(void)
{
    switch (phase) {
    case PHASE_OPEN:
        kinder_text_unpack_n(message, isp_mail, sizeof(isp_mail) - 1);
        OPEN(mail_window, mail_dialog);
        kinder_text_set(ADD(mail_window, mail_address), message);
        FOCUS(mail_window, mail_yes);
        ADD(mail_window, mail_no);
        ADD(mail_window, mail_prompt);
        ADD(mail_window, mail_dialog_fill);
        PUSH(mail_window);
        break;
    }
}
enum connect_phase {
    START_ADDRESS,
    WAIT_ADDRESS,
    WAIT_ENDPOINT,
    WAIT_ACCOUNT,
    WAIT_MAIL
};
static void connect_account(void)
{
    struct task *service;
    /* @hack spelling: the local service, set from address_task in the
     * switch's operand, where WAIT_ADDRESS would read address_task */
    switch (service = address_task, phase) {
    case START_ADDRESS:
        address_task = address_service_create(menu_task);
#ifdef NONMATCHING
        /* this side goes to MENU_ERROR where address_service_create makes no
         * service, which the matching side starts through the null */
        if (!address_task) {
            GO_TO(MENU_ERROR);
            return;
        }
#endif
        address_service_start(address_task);
        connection_notice_open();
        phase = WAIT_ADDRESS;
        break;
    case WAIT_ADDRESS:
        switch (address_service_state(service)) {
        case ADDRESS_LOOKUP:
        case ADDRESS_CONNECTING:
        case ADDRESS_WAIT_GREETING:
        case ADDRESS_WAIT_ACK:
            break;
        case ADDRESS_READY:
            endpoint.service = ADDRESS_ACCOUNT_SERVICE;
            if (address_service_submit(address_task, &endpoint)) {
                phase = WAIT_ENDPOINT;
            }
            break;
        case ADDRESS_IDLE:
        case ADDRESS_FAILED:
            GO_TO(MENU_DISCONNECTED);
            break;
        }
        break;
    case WAIT_ENDPOINT:
        switch (endpoint.result) {
        case ADDRESS_PENDING:
            break;
        case ADDRESS_FOUND:
            address_service_finish();
            address_service_destroy(address_task);
            account_task = account_service_create(menu_task);
#ifdef NONMATCHING
            /* this side goes to MENU_ERROR where account_service_create makes
             * no service, which the matching side sets through the null */
            if (!account_task) {
                GO_TO(MENU_ERROR);
                return;
            }
#endif
            account_service_endpoint(account_task, endpoint.address,
                                     endpoint.port);
            account_service_start(account_task);
            phase = WAIT_ACCOUNT;
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    case WAIT_ACCOUNT:
        switch (account_service_state(account_task)) {
        case ACCOUNT_READY:
            /* @bug logic: the first ISP account's mail, as in read_isp, at
             * 0x0c9d573e */
            mail_request.mail = kinder_isp_accounts[0].mail;
            mail_request.name = user_name;
            SUBMIT(account_find_mail(account_task, &mail_request));
            phase = WAIT_MAIL;
            break;
        case ACCOUNT_CONNECTING:
        case ACCOUNT_WAIT_GREETING:
        case ACCOUNT_WAIT_ACK:
            break;
        default:
            GO_TO(MENU_DISCONNECTED);
            break;
        }
        break;
    case WAIT_MAIL:
        switch (mail_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
            connection_notice_close();
            user_id = mail_request.user_id;
            if (user_id == ACCOUNT_NO_USER) {
                GO_TO(MENU_USER_NAME);
            } else {
                GO_TO(MENU_EXISTING);
            }
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
    }
    if (connection_notice_cancelled()) {
        GO_TO(MENU_DISCONNECTED);
    }
}
static int user_field_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_DOWN:
        SELECT(widget->parent, USER_NEXT);
        handled = TRUE;
        break;
    }
    return handled;
}
static int user_next_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    struct kinder_widget *parent = widget->parent;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(parent, USER_FIELD);
        break;
    case WIDGET_RIGHT:
        SELECT(parent, USER_QUIT);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        leave_user(MENU_CHECK_NAME);
        handled = TRUE;
        break;
    }
    return handled;
}
static int user_quit_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    struct kinder_widget *parent = widget->parent;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(parent, USER_FIELD);
        break;
    case WIDGET_LEFT:
        SELECT(parent, USER_NEXT);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        leave_user(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static int user_dialog_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_user(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void enter_user(void)
{
    switch (phase) {
    case PHASE_OPEN:
        OPEN(user_window, user_dialog);
        FOCUS(user_window, user_field);
        ADD(user_window, user_title);
        ADD(user_window, user_prompt);
        ADD(user_window, user_next);
        ADD(user_window, user_quit);
        ADD(user_window, user_dialog_fill);
        PUSH(user_window);
        break;
    }
}
static void leave_user(int next)
{
    struct kinder_widget *field = FIND(user_window, USER_FIELD);
#ifdef NONMATCHING
    /* this side bounds the copy at ACCOUNT_NAME_CODES - 1. the terminator then
     * stays inside user_name whatever the field holds */
    kinder_wide_terminate(user_name, kinder_frame_text(field),
                          ACCOUNT_NAME_CODES - 1);
#else
    /* a bound of ACCOUNT_NAME_CODES lets the terminator land one code past
     * user_name's end. none does, as the user field holds 12 codes */
    kinder_wide_terminate(user_name, kinder_frame_text(field),
                          ACCOUNT_NAME_CODES);
#endif
    CLOSE(user_window);
    GO_TO(next);
}
static void check_user(void)
{
    switch (phase) {
    case PHASE_OPEN:
        connection_notice_open();
        name_request.name = user_name;
        SUBMIT(account_check_name(account_task, &name_request));
        phase = PHASE_POLL;
        break;
    case PHASE_POLL:
        switch (name_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
            connection_notice_close();
            user_id = name_request.user_id;
            if (name_request.taken) {
                GO_TO(MENU_NAME_TAKEN);
            } else {
                GO_TO(MENU_PROFILE_NAME);
            }
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    }
}
static int profile_next_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, PROFILE_FIELD);
        break;
    case WIDGET_ACCEPT:
        leave_profile(MENU_CONTACT);
        handled = TRUE;
        break;
    }
    return handled;
}
static int profile_field_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_DOWN:
        SELECT(widget->parent, PROFILE_NEXT);
        handled = TRUE;
        break;
    }
    return handled;
}
static int profile_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_profile(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void enter_profile(void)
{
    switch (phase) {
    case PHASE_OPEN:
        OPEN(profile_window, profile_dialog);
        FOCUS(profile_window, profile_field);
        ADD(profile_window, profile_prompt);
        kinder_button_set_texlist(ADD(profile_window, profile_next),
                                  &kinder_widget_texlist);
        ADD(profile_window, profile_dialog_fill);
        PUSH(profile_window);
        break;
    }
}
static void leave_profile(int next)
{
    struct kinder_widget *field = FIND(profile_window, PROFILE_FIELD);
    PACK_FIELD(kinder_frame_text(field), profile_name, sizeof(profile_name));
    CLOSE(profile_window);
    GO_TO(next);
}
static int contact_field_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_DOWN:
        SELECT(widget->parent, CONTACT_NEXT);
        handled = TRUE;
        break;
    }
    return handled;
}
static int contact_next_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        SELECT(widget->parent, CONTACT_FIELD);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        leave_contact(MENU_VISIBILITY);
        handled = TRUE;
        break;
    }
    return handled;
}
static int contact_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    switch (event) {
    case WIDGET_CANCEL:
        leave_contact(MENU_EXIT);
        quit_account(MENU_CLOSED);
#ifdef NONMATCHING
        /* this side returns that the dialog took the cancel */
        return TRUE;
#else
        break;
#endif
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it returns FALSE for an event it
     * didn't take */
    return FALSE;
#endif
    /* @bug uninitialized: contact_dialog_input returns no value, and the
     * widgets read what R0 holds as whether it took the event, at
     * 0x0c9d5cb4 */
} /* NOLINT(clang-diagnostic-return-type) */
static void enter_contact(void)
{
    Uint16 text[sizeof(contact) + 1];
    struct kinder_widget *field;
    switch (phase) {
    case PHASE_OPEN:
        kinder_text_unpack_n(text,
                             kinder_isp_accounts[kinder_selected_isp].mail,
                             sizeof(contact));
        OPEN(contact_window, contact_dialog);
        ADD(contact_window, contact_dialog_fill);
        ADD(contact_window, contact_prompt);
        ADD(contact_window, contact_next);
        field = ADD(contact_window, contact_field);
        kinder_widget_focus(contact_window, field);
        kinder_frame_set_text(field, text);
        PUSH(contact_window);
        break;
    }
}
static void leave_contact(int next)
{
    struct kinder_widget *field = FIND(contact_window, CONTACT_FIELD);
    PACK_FIELD(kinder_frame_text(field), contact, sizeof(contact));
    CLOSE(contact_window);
    GO_TO(next);
}
static int visibility_yes_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_RIGHT:
        SELECT(widget->parent, VISIBILITY_NO);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        visible = TRUE;
        leave_visibility(MENU_LOCATION);
        handled = TRUE;
        break;
    }
    return handled;
}
static int visibility_no_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_LEFT:
        SELECT(widget->parent, VISIBILITY_YES);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        visible = FALSE;
        leave_visibility(MENU_LOCATION);
        handled = TRUE;
        break;
    }
    return handled;
}
static int visibility_dialog_input(void *owner, struct kinder_widget *widget,
                                   int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_visibility(MENU_CONTACT);
        handled = TRUE;
        break;
    }
    return handled;
}
static void choose_visibility(void)
{
    switch (phase) {
    case PHASE_OPEN:
        OPEN(visibility_window, visibility_dialog);
        ADD(visibility_window, visibility_prompt);
        ADD(visibility_window, visibility_dialog_fill);
        ADD(visibility_window, visibility_no);
        FOCUS(visibility_window, visibility_yes);
        PUSH(visibility_window);
        break;
    }
}
static void leave_visibility(int next)
{
    CLOSE(visibility_window);
    GO_TO(next);
}
/* @hack spelling: SHC gives these request and reply members their own
 * pool addresses in the account task. */
#ifdef NONMATCHING
/* the records' fields, as the packet and service headers declare them.
 * this side's objects don't define the members' own names, which only
 * symbols.txt places */
#define FIND_NAME account_find_request.name
#define FIND_STATUS account_find_request.status
#define FIND_VALUE account_find_request.value
#define READ_DESTINATION account_read_request.record
#define RECEIVED_NAME account_read_record.profile.name
#define RECEIVED_CONTACT account_read_record.profile.text
#define RECEIVED_VISIBILITY account_read_record.profile.flag
#define RECEIVED_COUNTRY account_read_record.profile.group
#define RECEIVED_REGION account_read_record.profile.choice
#define WRITE_SOURCE account_write_request.record
#define WRITE_DESTINATION account_write_request.reply
#define OUTGOING_NAME account_write_record.profile.name
#define OUTGOING_CONTACT account_write_record.profile.text
#define OUTGOING_VISIBILITY account_write_record.profile.flag
#else
extern const char *account_find_name;
extern Sint32 account_find_status;
extern Uint32 account_find_value;
extern struct account_record *account_read_destination;
extern char account_received_name[17], account_received_contact[49];
extern Sint32 account_received_visibility;
extern Sint16 account_received_country, account_received_region;
extern struct account_record *account_write_source;
extern struct account_status_packet *account_write_destination;
extern char account_outgoing_name[17], account_outgoing_contact[49];
extern Sint32 account_outgoing_visibility;
#define FIND_NAME account_find_name
#define FIND_STATUS account_find_status
#define FIND_VALUE account_find_value
#define READ_DESTINATION account_read_destination
#define RECEIVED_NAME account_received_name
#define RECEIVED_CONTACT account_received_contact
#define RECEIVED_VISIBILITY account_received_visibility
#define RECEIVED_COUNTRY account_received_country
#define RECEIVED_REGION account_received_region
#define WRITE_SOURCE account_write_source
#define WRITE_DESTINATION account_write_destination
#define OUTGOING_NAME account_outgoing_name
#define OUTGOING_CONTACT account_outgoing_contact
#define OUTGOING_VISIBILITY account_outgoing_visibility
#endif
static int country_choice_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    int key;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 4);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        switch (location_depth) {
        case 0:
            SELECT(widget->parent, 4);
            break;
        case 1:
        case 2:
            SELECT(widget->parent, 2);
            break;
        }
        break;
    case WIDGET_SELECTED:
        key = kinder_list_key(widget);
        if (country != key) {
            country = key;
            refresh_regions();
        }
        handled = TRUE;
        break;
    }
    return handled;
}
static int district_choice_input(void *owner, struct kinder_widget *widget,
                                 int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 2);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        SELECT(widget->parent, 4);
        break;
    case WIDGET_SELECTED:
        district = kinder_list_key(widget);
        break;
    }
    return handled;
}
static int region_choice_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    int key;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 1);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        switch (location_depth) {
        default:
        case 1:
            SELECT(widget->parent, 4);
            break;
        case 2:
            SELECT(widget->parent, 3);
            break;
        }
        break;
    case WIDGET_SELECTED:
        key = kinder_list_key(widget);
        if (region != key) {
            region = key;
            refresh_districts();
        }
        handled = TRUE;
        break;
    }
    return handled;
}
static int location_next_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        switch (location_depth) {
        default:
        case 0:
            SELECT(widget->parent, 1);
            break;
        case 1:
            SELECT(widget->parent, 2);
            break;
        case 2:
            SELECT(widget->parent, 3);
            break;
        }
        break;
    case WIDGET_ACCEPT:
        leave_location(MENU_CONFIRM_CREATE);
        handled = TRUE;
        break;
    }
    return handled;
}
static int location_dialog_input(void *owner, struct kinder_widget *widget,
                                 int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_location(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void refresh_districts(void)
{
    const struct account_districts *items;
    kinder_widget_drop(location_window, CONTROL(3));
    items = kinder_profile_countries.regions[country - 1].districts;
    if (items) {
        items = &items[region - 1];
        district_choice_style.entries = items->items;
        district_choice_style.count = items->count;
        district_choice_style.selected = 1;
        ADD(location_window, district_choice);
        location_depth = 2;
    } else {
        location_depth = 1;
        district = 0;
    }
}
static void refresh_regions(void)
{
    const struct account_regions *items;
    kinder_widget_drop(location_window, CONTROL(2));
    items = &kinder_profile_countries.regions[country - 1];
    if (items->items) {
        region_choice_style.entries = items->items;
        region_choice_style.count = items->count;
        region_choice_style.selected = 1;
        ADD(location_window, region_choice);
        region = 1;
        refresh_districts();
    } else {
        kinder_widget_drop(location_window, CONTROL(3));
        location_depth = 0;
        region = 0;
    }
}
static void choose_location(void)
{
    switch (phase) {
    case 0:
        country_choice_style.entries = kinder_profile_countries.items;
        country_choice_style.count = kinder_profile_countries.count;
        OPEN(location_window, location_dialog);
        ADD(location_window, location_prompt);
        ADD(location_window, country_label);
        ADD(location_window, region_label);
        FOCUS(location_window, country_choice);
        ADD(location_window, location_next);
        ADD(location_window, location_dialog_fill);
        kinder_widget_push(location_window);
        country = 1;
        refresh_regions();
        phase = 1;
        break;
    }
}
static void leave_location(int next)
{
    country_key = country;
    if (region) {
        if (district) {
            region_key = district;
        } else {
            region_key = region;
        }
    }
    CLOSE(location_window);
    GO_TO(next);
}
static int submit_yes_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_RIGHT:
        SELECT(submit_window, 3);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        leave_submit(MENU_CREATE);
        handled = TRUE;
        break;
    }
    return handled;
}
static int submit_no_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_LEFT:
        SELECT(submit_window, 2);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        leave_submit(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static int submit_dialog_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_submit(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void confirm_create(void)
{
    switch (phase) {
    case 0:
        OPEN(submit_window, submit_dialog);
        FOCUS(submit_window, submit_yes);
        ADD(submit_window, submit_no);
        ADD(submit_window, submit_prompt);
        ADD(submit_window, submit_dialog_fill);
        PUSH(submit_window);
        break;
    }
}
static void leave_submit(int next)
{
    CLOSE(submit_window);
    GO_TO(next);
}
static void create_account(void)
{
    switch (phase) {
    case 0:
        create_request.name = profile_name;
        create_request.text = contact;
        create_request.flag = visible;
        create_request.group = country_key;
        create_request.choice = region_key;
        SUBMIT(account_create_profile(account_task, &create_request));
        phase = 1;
        break;
    case 1:
        switch (create_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
            GO_TO(MENU_SUCCESS);
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    }
}
static int success_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_success(MENU_EXIT);
        quit_account(MENU_SAVED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void show_success(void)
{
    switch (phase) {
    case 0:
        kinder_text_format(message, kinder_string(567), user_name);
        OPEN(success_window, success_dialog);
        kinder_text_set(ADD(success_window, success_message), message);
        ADD(success_window, success_dialog_fill);
        PUSH(success_window);
        break;
    }
}
static void leave_success(int next)
{
    CLOSE(success_window);
    GO_TO(next);
}
static int taken_field_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_DOWN:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    }
    return handled;
}
static int taken_find_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_taken(MENU_FIND);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 4);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 3);
        handled = TRUE;
        break;
    }
    return handled;
}
static int taken_retry_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_taken(MENU_USER_NAME);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 4);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 3);
        handled = TRUE;
        break;
    }
    return handled;
}
static int taken_remove_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_taken(MENU_CONFIRM_REMOVE);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 4);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    }
    return handled;
}
static int taken_dialog_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    return FALSE;
}
static void show_taken(void)
{
    switch (phase) {
    case 0:
        OPEN(taken_window, taken_dialog);
        ADD(taken_window, taken_message);
        FOCUS(taken_window, taken_field);
        ADD(taken_window, taken_find);
        ADD(taken_window, taken_retry);
        ADD(taken_window, taken_remove);
        ADD(taken_window, taken_dialog_fill);
        PUSH(taken_window);
        break;
    }
}
static void leave_taken(int next)
{
    CLOSE(taken_window);
    previous_screen = screen;
    GO_TO(next);
}
static int found_field_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_DOWN:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    }
    return handled;
}
static int found_find_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_existing(MENU_FIND);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 4);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 3);
        handled = TRUE;
        break;
    }
    return handled;
}
static int found_quit_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_existing(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 4);
        handled = TRUE;
        break;
    }
    return handled;
}
static int found_remove_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_existing(MENU_CONFIRM_REMOVE);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 3);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    }
    return handled;
}
static int found_dialog_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_existing(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void show_existing(void)
{
    switch (phase) {
    case 0:
        kinder_text_format(message, kinder_string(552), user_name);
        OPEN(found_window, found_dialog);
        kinder_text_set(ADD(found_window, found_message), message);
        FOCUS(found_window, found_field);
        ADD(found_window, found_find);
        ADD(found_window, found_quit);
        ADD(found_window, found_remove);
        ADD(found_window, found_dialog_fill);
        PUSH(found_window);
        break;
    }
}
static void leave_existing(int next)
{
    struct kinder_widget *field = FIND(found_window, 1);
    PACK_FIELD(kinder_frame_text(field), profile_name, sizeof(profile_name));
    CLOSE(found_window);
    previous_screen = screen;
    GO_TO(next);
}
static void find_profile(void)
{
    switch (phase) {
    case 0:
        FIND_NAME = profile_name;
        SUBMIT(account_find_profile(account_task, &account_find_request));
        phase = 1;
        break;
    case 1:
        switch (account_find_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
            switch (FIND_STATUS) {
            case 0:
                profile_id = FIND_VALUE;
                GO_TO(MENU_READ);
                break;
            default:
                GO_TO(MENU_MISSING);
                break;
            }
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    }
}
static int missing_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_missing(previous_screen);
        handled = TRUE;
        break;
    }
    return handled;
}
static void show_missing(void)
{
    switch (phase) {
    case 0:
        OPEN(account_missing_window, missing_dialog);
        ADD(account_missing_window, missing_message);
        ADD(account_missing_window, missing_title);
        ADD(account_missing_window, missing_dialog_fill);
        PUSH(account_missing_window);
        break;
    }
}
static void leave_missing(int next)
{
    CLOSE(account_missing_window);
    GO_TO(next);
}
static void leave_remove(int next);
static void leave_removed(int next);
static void refresh_edit_districts(void);
static void refresh_edit_regions(void);
static void leave_edit(int next);
static void leave_isp(int next);
static void leave_disconnect(int next);
static void leave_failure(int next);

static int remove_yes_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_remove(MENU_REMOVE);
        handled = TRUE;
        break;
    case WIDGET_RIGHT:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    }
    return handled;
}
static int remove_no_input(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_remove(previous_screen);
        handled = TRUE;
        break;
    case WIDGET_LEFT:
        SELECT(widget->parent, 1);
        handled = TRUE;
        break;
    }
    return handled;
}
static int remove_dialog_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_CANCEL:
        leave_remove(previous_screen);
        handled = TRUE;
        break;
    }
    return handled;
}
static void confirm_remove(void)
{
    switch (phase) {
    case 0:
        OPEN(account_remove_window, remove_dialog);
        FOCUS(account_remove_window, remove_yes);
        ADD(account_remove_window, remove_no);
        ADD(account_remove_window, remove_message);
        ADD(account_remove_window, remove_dialog_fill);
        PUSH(account_remove_window);
        break;
    }
}
static void leave_remove(int next)
{
    CLOSE(account_remove_window);
    GO_TO(next);
}
static int removed_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_removed(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void show_removed(void)
{
    switch (phase) {
    case 0:
        OPEN(account_removed_window, removed_dialog);
        ADD(account_removed_window, removed_message);
        ADD(account_removed_window, removed_dialog_fill);
        PUSH(account_removed_window);
        break;
    }
}
static void leave_removed(int next)
{
    CLOSE(account_removed_window);
    GO_TO(next);
}
static void read_profile(void)
{
    switch (phase) {
    case 0:
        READ_DESTINATION = &account_read_record;
        SUBMIT(account_read_profile(account_task, &account_read_request));
        phase = 1;
        break;
    case 1:
        switch (account_read_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
#ifdef NONMATCHING
            /* Each fixed-width field may fill its wire record. */
            RECEIVED_NAME[sizeof(RECEIVED_NAME) - 1] = 0;
            RECEIVED_CONTACT[sizeof(RECEIVED_CONTACT) - 1] = 0;
#endif
            strcpy(profile_name, RECEIVED_NAME);
            strcpy(contact, RECEIVED_CONTACT);
            visible = RECEIVED_VISIBILITY;
            country_key = RECEIVED_COUNTRY;
            region_key = RECEIVED_REGION;
#ifdef NONMATCHING
            /* this side takes country 1 and no region for a country key outside
             * kinder_profile_countries. the editor indexes that table by the
             * key */
            if (!country_key || country_key > kinder_profile_countries.count) {
                country_key = 1;
                region_key = 0;
            }
#endif
            GO_TO(MENU_EDIT);
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    }
}
static int edit_profile_field_input(void *owner, struct kinder_widget *widget,
                                    int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        SELECT(widget->parent, 8);
        handled = TRUE;
        break;
    case WIDGET_DOWN:
        SELECT(widget->parent, 3);
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_country_input(void *owner, struct kinder_widget *widget,
                              int event)
{
    int handled = FALSE;
    int key;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 2);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        switch (account_edit_depth) {
        case 0:
            SELECT(widget->parent, 6);
            break;
        case 1:
        case 2:
            SELECT(widget->parent, 4);
            break;
        }
        break;
    case WIDGET_SELECTED:
        key = kinder_list_key(widget);
        if (account_edit_country_key != (Uint32)key) {
            account_edit_country_key = key;
            refresh_edit_regions();
        }
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_region_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    int key;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 3);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        switch (account_edit_depth) {
        default:
        case 1:
            SELECT(widget->parent, 6);
            break;
        case 2:
            SELECT(widget->parent, 5);
            break;
        }
        break;
    case WIDGET_SELECTED:
        key = kinder_list_key(widget);
        if (account_edit_region_key != key) {
            account_edit_region_key = key;
            refresh_edit_districts();
        }
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_district_input(void *owner, struct kinder_widget *widget,
                               int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        handled = TRUE;
        SELECT(widget->parent, 4);
        break;
    case WIDGET_DOWN:
        handled = TRUE;
        SELECT(widget->parent, 6);
        break;
    case WIDGET_SELECTED:
        account_edit_district_key = kinder_list_key(widget);
        break;
    }
    return handled;
}
static int edit_contact_field_input(void *owner, struct kinder_widget *widget,
                                    int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        switch (account_edit_depth) {
        default:
            SELECT(widget->parent, 3);
            break;
        case 1:
            SELECT(widget->parent, 4);
            break;
        case 2:
            SELECT(widget->parent, 5);
            break;
        }
        handled = TRUE;
        break;
    case WIDGET_DOWN:
        SELECT(widget->parent, 7);
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_visibility_input(void *owner, struct kinder_widget *widget,
                                 int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_UP:
        SELECT(widget->parent, 6);
        handled = TRUE;
        break;
    case WIDGET_DOWN:
        SELECT(widget->parent, 8);
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_submit_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
        leave_edit(MENU_WRITE);
        handled = TRUE;
        break;
    case WIDGET_UP:
        SELECT(widget->parent, 7);
        handled = TRUE;
        break;
    case WIDGET_DOWN:
        SELECT(widget->parent, 2);
        handled = TRUE;
        break;
    }
    return handled;
}
static int edit_dialog_input(void *owner, struct kinder_widget *widget,
                             int event)
{
    return FALSE;
}
static void refresh_edit_districts(void)
{
    const struct account_districts *items;
    kinder_widget_drop(account_edit_window, CONTROL(5));
    items = kinder_profile_countries.regions[account_edit_country_key - 1]
                .districts;
    if (items) {
        items = &items[account_edit_region_key - 1];
        account_edit_district_style.entries = items->items;
        account_edit_district_style.count = items->count;
        account_edit_district_style.selected = 1;
        ADD(account_edit_window, edit_district);
        account_edit_depth = 2;
    } else {
        account_edit_depth = 1;
        account_edit_district_key = 0;
    }
}
static void refresh_edit_regions(void)
{
    const struct account_regions *items;
    kinder_widget_drop(account_edit_window, CONTROL(4));
    items = &kinder_profile_countries.regions[account_edit_country_key - 1];
    if (items->items) {
        account_edit_region_style.entries = items->items;
        account_edit_region_style.count = items->count;
        account_edit_region_style.selected = 1;
        ADD(account_edit_window, edit_region);
        account_edit_region_key = 1;
        refresh_edit_districts();
    } else {
        kinder_widget_drop(account_edit_window, CONTROL(5));
        account_edit_depth = 0;
        account_edit_region_key = 0;
    }
}
#ifdef NONMATCHING
/* A trailing lead byte in a saved field has no second byte. Stop there
 * before the packed reader advances past the record. */
static void unpack_field(Uint16 *dst, const char *src, int size)
{
    int code;
    while (size-- > 0 && (code = (Uint8)*src++) != 0) {
        if (code >= 0x7f) {
            if (size == 0 || *src == 0) {
                break;
            }
            code = (code << 8) | (Uint8)*src++;
            --size;
        }
        *dst++ = code;
    }
    *dst = 0;
}
#define UNPACK_FIELD(dst, src) unpack_field(dst, src, sizeof(src))
#else
#define UNPACK_FIELD(dst, src) kinder_text_unpack(dst, src)
#endif
static void edit_profile(void)
{
    Uint16 name[17];
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. text holds as many
     * codes as contact holds characters */
    Uint16 text[sizeof(contact)];
#else
    /* @bug memory: the server's 48 single-byte characters can exceed the
     * 32 codes reserved for the contact field at 0x0c9d75c2. */
    Uint16 text[32];
#endif
    struct kinder_widget *field;
    const struct account_regions *items;
    int key;
    int i;
    int j;
    switch (phase) {
    case 0:
        OPEN(account_edit_window, edit_dialog);
        ADD(account_edit_window, edit_user_label);
        ADD(account_edit_window, edit_profile_label);
        ADD(account_edit_window, edit_country_label);
        ADD(account_edit_window, edit_region_label);
        ADD(account_edit_window, edit_contact_label);
        ADD(account_edit_window, edit_visibility_label);
        UNPACK_FIELD(name, profile_name);
        field = ADD(account_edit_window, edit_profile_field);
        kinder_frame_set_text(field, name);
        UNPACK_FIELD(text, contact);
        field = ADD(account_edit_window, edit_contact_field);
        kinder_frame_set_text(field, text);
        field = ADD(account_edit_window, edit_visibility);
        kinder_list_select(field, visible ? 0 : 1);
        ADD(account_edit_window, edit_dialog_fill);
        field = ADD(account_edit_window, edit_user_name);
        kinder_caption_set_text(field, user_name);
        field = ADD(account_edit_window, edit_submit);
        kinder_widget_focus(account_edit_window, field);
        account_edit_country_style.entries = kinder_profile_countries.items;
        account_edit_country_style.count = kinder_profile_countries.count;
        field = ADD(account_edit_window, edit_country);
        account_edit_country_key = country_key;
        kinder_list_select(field, country_key);
        account_edit_depth = 0;
        if (region_key != 0) {
            items = &kinder_profile_countries.regions[country_key - 1];
            if (items->items) {
                account_edit_region_style.entries = items->items;
                account_edit_region_style.count = items->count;
                field = ADD(account_edit_window, edit_region);
                key = region_key;
                if (items->districts) {
                    for (i = 0; i < items->count; i++) {
                        for (j = 0; j < items->districts[i].count; j++) {
                            if (items->districts[i].items[j].key == key) {
                                kinder_list_select(field, i + 1);
#ifdef NONMATCHING
                                /* this side stores the region as the key i + 1.
                                 * the read of items[account_edit_region_key -
                                 * 1] takes that key. it keeps the district's
                                 * key */
                                account_edit_region_key = i + 1;
                                account_edit_district_key = region_key;
#else
                                account_edit_region_key = i;
#endif
                                account_edit_district_style.entries =
                                    items->districts[i].items;
                                account_edit_district_style.count =
                                    items->districts[i].count;
                                field = ADD(account_edit_window, edit_district);
                                kinder_list_select(field, region_key);
                                account_edit_depth = 2;
                                goto ready;
                            }
                        }
                    }
                } else {
                    kinder_list_select(field, key);
                    account_edit_region_key = region_key;
                    account_edit_depth = 1;
                }
            }
        }
    ready:
        kinder_widget_push(account_edit_window);
        phase = 1;
        break;
    }
}
static void leave_edit(int next)
{
    struct kinder_widget *field;
    country_key = account_edit_country_key;
    if (account_edit_region_key) {
        if (account_edit_district_key) {
            region_key = account_edit_district_key;
        } else {
            region_key = account_edit_region_key;
        }
    }
    field = FIND(account_edit_window, 2);
    PACK_FIELD(kinder_frame_text(field), profile_name, sizeof(profile_name));
    field = FIND(account_edit_window, 6);
    PACK_FIELD(kinder_frame_text(field), contact, sizeof(contact));
    visible = !kinder_list_key(FIND(account_edit_window, 7));
    CLOSE(account_edit_window);
    GO_TO(next);
}
static void write_profile(void)
{
    switch (phase) {
    case 0:
        WRITE_SOURCE = &account_write_record;
        WRITE_DESTINATION = &account_write_reply;
        account_write_init(&account_write_record);
        strcpy(OUTGOING_NAME, profile_name);
        strcpy(OUTGOING_CONTACT, contact);
        OUTGOING_VISIBILITY = visible;
        account_write_record.profile.group = country_key;
        /* @hack spelling: storing the region in the call expression keeps
         * the record address out of a callee-saved register. */
#ifdef NONMATCHING
        /* the plain form the hack tag names, and a request that isn't sent goes
         * to MENU_ERROR */
        account_write_record.profile.choice = region_key;
        SUBMIT(account_write_profile(account_task, &account_write_request));
#else
        account_write_record.profile.choice = region_key,
        account_write_profile(account_task, &account_write_request);
#endif
        phase = 1;
        break;
    case 1:
        switch (account_write_request.request.result) {
        case ACCOUNT_PENDING:
            break;
        case ACCOUNT_ANSWERED:
            switch (account_write_reply.status) {
            case 0:
                GO_TO(MENU_SUCCESS);
                break;
            default:
                GO_TO(MENU_ERROR);
                break;
            }
            break;
        default:
            GO_TO(MENU_ERROR);
            break;
        }
        break;
    }
}
static int isp_dialog_input(void *owner, struct kinder_widget *widget,
                            int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_isp(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void no_isp(void)
{
    switch (phase) {
    case 0:
        OPEN(account_isp_window, isp_dialog);
        ADD(account_isp_window, isp_dialog_fill);
        ADD(account_isp_window, isp_message);
        PUSH(account_isp_window);
        break;
    }
}
static void leave_isp(int next)
{
    CLOSE(account_isp_window);
    GO_TO(next);
}
static int disconnect_dialog_input(void *owner, struct kinder_widget *widget,
                                   int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_disconnect(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void disconnected(void)
{
    switch (phase) {
    case 0:
        OPEN(account_disconnect_window, disconnect_dialog);
        ADD(account_disconnect_window, disconnect_message);
        ADD(account_disconnect_window, disconnect_dialog_fill);
        PUSH(account_disconnect_window);
        break;
    }
}
static void leave_disconnect(int next)
{
    CLOSE(account_disconnect_window);
    GO_TO(next);
}
static int failure_dialog_input(void *owner, struct kinder_widget *widget,
                                int event)
{
    int handled = FALSE;
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        leave_failure(MENU_EXIT);
        quit_account(MENU_CLOSED);
        handled = TRUE;
        break;
    }
    return handled;
}
static void account_failure(void)
{
    switch (phase) {
    case 0:
        OPEN(account_failure_window, failure_dialog);
        ADD(account_failure_window, failure_message);
        ADD(account_failure_window, failure_dialog_fill);
        PUSH(account_failure_window);
        break;
    }
}
static void leave_failure(int next)
{
    CLOSE(account_failure_window);
    GO_TO(next);
}

static void account_menu_destroy(struct task *task)
{
    kinder_widget_textures_release();
}
static void account_menu_draw(struct task *task)
{
}
static void account_menu_step(struct task *task)
{
    if (!task->destructor) {
        task->destructor = account_menu_destroy;
        task->draw = account_menu_draw;
    }
    switch (screen) {
    case MENU_START:
        kinder_widget_textures_load();
        GO_TO(MENU_ISP);
        break;
    case MENU_ISP:
        read_isp();
        break;
    case MENU_CONFIRM_MAIL:
        confirm_mail();
        break;
    case MENU_CONNECT:
        connect_account();
        break;
    case MENU_USER_NAME:
        enter_user();
        break;
    case MENU_CHECK_NAME:
        check_user();
        break;
    case MENU_PROFILE_NAME:
        enter_profile();
        break;
    case MENU_CONTACT:
        enter_contact();
        break;
    case MENU_VISIBILITY:
        choose_visibility();
        break;
    case MENU_LOCATION:
        choose_location();
        break;
    case MENU_CONFIRM_CREATE:
        confirm_create();
        break;
    case MENU_CREATE:
        create_account();
        break;
    case MENU_SUCCESS:
        show_success();
        break;
    case MENU_NAME_TAKEN:
        show_taken();
        break;
    case MENU_EXISTING:
        show_existing();
        break;
    case MENU_FIND:
        find_profile();
        break;
    case MENU_MISSING:
        show_missing();
        break;
    case MENU_CONFIRM_REMOVE:
        confirm_remove();
        break;
    case MENU_REMOVE:
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: the removal
         * goes back to the screen before */
        GO_TO(previous_screen);
#else
        /* @bug logic: the store at 0x0c9d7ed0 sets phase one. No arm
         * starts a removal request or leaves phase one. */
        switch (phase) {
        case 0:
            phase = 1;
            break;
        }
#endif
        break;
    case MENU_REMOVED:
        show_removed();
        break;
    case MENU_READ:
        read_profile();
        break;
    case MENU_EDIT:
        edit_profile();
        break;
    case MENU_WRITE:
        write_profile();
        break;
    case MENU_NO_ISP:
        no_isp();
        break;
    case MENU_DISCONNECTED:
        disconnected();
        break;
    case MENU_ERROR:
        account_failure();
        break;
    case MENU_EXIT:
        task_kill(task);
        break;
    }
}
int kinder_account_menu_create(struct task *parent)
{
    if ((menu_task = task_create_child(0, account_menu_step, parent)) != NULL) {
        GO_TO(MENU_START);
        result = MENU_ACTIVE;
        connection_notice_start(menu_task);
    }
    return menu_task != NULL;
}
Sint32 kinder_account_menu_result(void)
{
    return result;
}
void kinder_account_menu_ids(Sint32 *ids)
{
    ids[0] = user_id;
    ids[1] = profile_id;
}

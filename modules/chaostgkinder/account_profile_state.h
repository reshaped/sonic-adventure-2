/* the state the account menu's profile editor keeps between its task's
 * steps. besides its lists' styles and its windows, it holds each request to
 * the service with its records */
#ifndef CHAOSTGKINDER_ACCOUNT_PROFILE_STATE_H
#define CHAOSTGKINDER_ACCOUNT_PROFILE_STATE_H
#include <sg_xpt.h>
#include "chaostgkinder/account_packet.h"
#include "chaostgkinder/account_service.h"
#include "chaostgkinder/widget.h"

extern struct kinder_list_desc account_edit_country_style;
extern struct kinder_list_desc account_edit_region_style;
extern struct kinder_list_desc account_edit_district_style;
extern struct account_find_request account_find_request;
extern struct kinder_widget *account_missing_window, *account_remove_window,
    *account_removed_window;
extern struct account_read_request account_read_request;
extern struct account_record account_read_record;
extern Sint32 account_edit_depth;
extern Uint32 account_edit_country_key;
extern Sint32 account_edit_region_key, account_edit_district_key;
extern struct kinder_widget *account_edit_window;
extern struct account_write_request account_write_request;
extern struct account_record account_write_record;
extern struct account_status_packet account_write_reply;
extern struct kinder_widget *account_isp_window, *account_disconnect_window,
    *account_failure_window;
#endif

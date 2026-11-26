#include <sg_xpt.h>
#include "chaostgkinder/account_profile_state.h"
#include "chaostgkinder/account_service.h"
#include "chaostgkinder/account_packet.h"
#include "chaostgkinder/widget.h"
#define EMPTY_LIST {&kinder_widget_texlist, NULL, 0, 1}
struct kinder_list_desc account_edit_country_style = EMPTY_LIST;
struct kinder_list_desc account_edit_region_style = EMPTY_LIST;
struct kinder_list_desc account_edit_district_style = EMPTY_LIST;

struct account_find_request account_find_request;
struct kinder_widget *account_missing_window, *account_remove_window,
    *account_removed_window;
struct account_read_request account_read_request;
struct account_record account_read_record;
Sint32 account_edit_depth;
Uint32 account_edit_country_key;
Sint32 account_edit_region_key, account_edit_district_key;
struct kinder_widget *account_edit_window;
struct account_write_request account_write_request;
struct account_record account_write_record;
struct account_status_packet account_write_reply;
struct kinder_widget *account_isp_window, *account_disconnect_window,
    *account_failure_window;

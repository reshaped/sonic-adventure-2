#include <sg_xpt.h>
#include "chaostgkinder/message_data.h"
#include "chaostgkinder/messages.h"

static const char japanese[4] = "_j", english[4] = "_e", french[4] = "_f",
                  spanish[4] = "_s", german[4] = "_g";
#define TEN_DUMMIES "DUMMYDUMMYDUMMYDUMMYDUMMYDUMMYDUMMYDUMMYDUMMYDUMMY\n"
static const char placeholder[] = TEN_DUMMIES TEN_DUMMIES TEN_DUMMIES
    TEN_DUMMIES TEN_DUMMIES TEN_DUMMIES TEN_DUMMIES;

Sint32 *kinder_room_strings = NULL;
Sint32 *kinder_service_strings = NULL;
struct kinder_message_entry *kinder_room_script = NULL;
struct kinder_message_entry *kinder_service_script = NULL;
const char *kinder_message_suffixes[] = {japanese, english, french, spanish,
                                         german};
struct text_sprite *kinder_dialog_sprite = NULL;
struct task *kinder_dialog = NULL;
const char *kinder_placeholder_text = placeholder;
struct text_sprite *kinder_shared_sprite = NULL;

/* Each room message names a string in the loaded language file and the
 * next line or page command. */
struct kinder_message_entry kinder_default_script[147] = {
#include "chaostgkinder/message_data/kinder_default_script.inc"
};

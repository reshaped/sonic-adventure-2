#ifndef CHAOSTGKINDER_MESSAGE_DATA_H
#define CHAOSTGKINDER_MESSAGE_DATA_H
#include "chaostgkinder/messages.h"

struct text_sprite;
extern Sint32 *kinder_room_strings, *kinder_service_strings;
extern struct kinder_message_entry *kinder_room_script, *kinder_service_script;
extern const char *kinder_message_suffixes[];
extern struct text_sprite *kinder_dialog_sprite, *kinder_shared_sprite;
extern struct task *kinder_dialog;
extern const char *kinder_placeholder_text;
extern struct kinder_message_entry kinder_default_script[];
#endif

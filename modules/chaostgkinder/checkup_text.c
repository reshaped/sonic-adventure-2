#include "chaostgkinder/checkup_text.h"
#include "chaostgkinder/messages.h"

/* the table's entries, and its groups */
#define CHECKUP_ENTRIES 222
#define CHECKUP_GROUPS 23

struct kinder_message_entry checkup_messages[CHECKUP_ENTRIES] = {
#include "chaostgkinder/checkup_text/checkup_messages.inc"
};

static int last_entries[CHECKUP_GROUPS] = {
#include "chaostgkinder/checkup_text/last_entries.inc"
};

#ifdef NONMATCHING
Bool checkup_messages_ready(void)
{
    /* the disc's first reply is a single entry, so its next is
     * KINDER_MESSAGE_END */
    return checkup_messages[0].next == KINDER_MESSAGE_END;
}
#endif

int checkup_first_entry(int group)
{
    if (group == 0) {
        return 0;
    }
    return last_entries[group - 1] + 1;
}

int checkup_last_entry(int group)
{
    return last_entries[group];
}

int checkup_last_offset(int group)
{
    return checkup_last_entry(group) - checkup_first_entry(group);
}

int checkup_message_group(int entry)
{
    int group = 0;
#ifdef NONMATCHING
    /* this side returns -1 for an entry past the last group. it returns -1 too
     * where the disc's replies aren't there, as checkup_messages_ready says */
    int groups = (int)(sizeof(last_entries) / sizeof(last_entries[0]));
    if (!checkup_messages_ready() || entry > last_entries[groups - 1]) {
        return -1;
    }
#endif
    while (checkup_last_entry(group) < entry) {
        group++;
    }
    return group;
}

int checkup_message_variant(int group, int variant)
{
    int index = checkup_first_entry(group);
    struct kinder_message_entry *entry = &checkup_messages[index];
#ifdef NONMATCHING
    /* this side returns -1 where the disc's replies aren't there, as
     * checkup_messages_ready says */
    if (!checkup_messages_ready()) {
        return -1;
    }
#endif

    while (variant > 0) {
        while (entry->next != KINDER_MESSAGE_END) {
            entry++;
            index++;
        }
        variant--;
        index++;
        entry++;
    }
    return index;
}

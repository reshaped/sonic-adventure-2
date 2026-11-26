/* the checkup's replies, which kinder_checkup_create gives bank 1. the
 * table holds groups of replies. a reply is a run of entries that ends at
 * one whose next is KINDER_MESSAGE_END. groups, replies and entries count
 * from 0 */
#ifndef CHAOSTGKINDER_CHECKUP_TEXT_H
#define CHAOSTGKINDER_CHECKUP_TEXT_H
#include "chaostgkinder/messages.h"

/* the groups whose replies the checkup shows, by what picks the reply */
enum checkup_group {
    ILLNESS_GROUP = 1,
    HEADGEAR_GROUP = 3,
    FORM_GROUP = 4,
    AFFECTION_GROUP = 6,
    PERSONALITY_GROUP = 7,
    CONDITION_GROUP = 8,
    TASTE_GROUP = 9,
    SWIM_GROUP = 10,
    FLY_GROUP = 11,
    RUN_GROUP = 12,
    POWER_GROUP = 13,
    STAMINA_GROUP = 14,
    AGE_GROUP = 15,
    LIVES_GROUP = 16,
    SONIC_GROUP = 17,
    SHADOW_GROUP = 18,
    TAILS_GROUP = 19,
    EGGMAN_GROUP = 20,
    KNUCKLES_GROUP = 21,
    ROUGE_GROUP = 22
};

extern struct kinder_message_entry checkup_messages[];
#ifdef NONMATCHING
/* whether checkup_messages holds the disc's replies. a build without the
 * disc's data holds zeros there */
Bool checkup_messages_ready(void);
#endif
/* the index of group's first entry, and of its last */
int checkup_first_entry(int group);
int checkup_last_entry(int group);
/* how far group's last entry lies past its first, in entries */
int checkup_last_offset(int group);
/* the group that holds entry, which must lie in one */
int checkup_message_group(int entry);
/* the index of the first entry of the reply at variant in group */
int checkup_message_variant(int group, int variant);

#endif

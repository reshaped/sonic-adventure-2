/* the ACCOUNT service's packets. past the greeting and its ack, the kinds
 * pair each request with its reply */
#ifndef CHAOSTGKINDER_ACCOUNT_PACKET_H
#define CHAOSTGKINDER_ACCOUNT_PACKET_H
#include <sg_xpt.h>
#include "chaostgkinder/packet.h"

enum account_packet_kind {
    ACCOUNT_GREETING = 16,
    ACCOUNT_ACK,
    ACCOUNT_FIND_MAIL,
    ACCOUNT_MAIL_REPLY,
    ACCOUNT_CHECK_NAME,
    ACCOUNT_NAME_REPLY,
    ACCOUNT_CREATE_PROFILE,
    ACCOUNT_CREATE_REPLY,
    ACCOUNT_FIND_PROFILE,
    ACCOUNT_FIND_REPLY,
    ACCOUNT_READ_PROFILE,
    ACCOUNT_READ_REPLY,
    ACCOUNT_WRITE_PROFILE,
    ACCOUNT_WRITE_REPLY,
    ACCOUNT_STATUS,
    ACCOUNT_STATUS_REPLY
};

/* the bytes of each text field, its NUL included. the fields are a
 * greeting's text, an ISP mail address, a profile's name and its contact,
 * and the name a find request gives */
#define ACCOUNT_GREETING_BYTES 128
#define ACCOUNT_MAIL_BYTES 64
#define ACCOUNT_PROFILE_NAME_BYTES 17
#define ACCOUNT_CONTACT_BYTES 49
#define ACCOUNT_FIND_NAME_BYTES 23

struct account_greeting {
    struct kinder_packet header;
    char text[ACCOUNT_GREETING_BYTES];
};
struct account_status_packet {
    struct kinder_packet header;
    Sint32 status;
};
struct account_mail {
    struct kinder_packet header;
    char mail[ACCOUNT_MAIL_BYTES];
};
/* a user name's room in 16-bit codes, its terminator included */
#define ACCOUNT_NAME_CODES 17
struct account_mail_reply {
    struct kinder_packet header;
    Uint32 user_id;
    Uint16 name[ACCOUNT_NAME_CODES];
};
struct account_name {
    struct kinder_packet header;
    Uint16 name[ACCOUNT_NAME_CODES];
};
struct account_value_reply {
    struct kinder_packet header;
    Sint32 status;
    Uint32 value;
};
/* a profile. text is the contact address, and flag the visibility dialog's
 * answer. group is the country, and choice the region or district */
struct account_profile_fields {
    char name[ACCOUNT_PROFILE_NAME_BYTES], text[ACCOUNT_CONTACT_BYTES];
    Bool flag;
    Sint16 group, choice;
};
struct account_create {
    struct kinder_packet header;
    Sint32 reserved;
    struct account_profile_fields profile;
};
struct account_create_reply {
    struct kinder_packet header;
    Sint16 status;
    Uint32 value;
};
struct account_find {
    struct kinder_packet header;
    char name[ACCOUNT_FIND_NAME_BYTES];
};
struct account_record {
    struct kinder_packet header;
    struct account_profile_fields profile;
};

void account_greeting_init(struct account_greeting *message);
void account_mail_init(struct account_mail *message);
void account_name_init(struct account_name *message);
void account_create_init(struct account_create *message);
void account_find_init(struct account_find *message);
void account_read_init(struct kinder_packet *packet);
void account_write_init(struct account_record *message);
/* packet_decode for the ACCOUNT service. it also decodes ACCOUNT_GREETING,
 * ACCOUNT_ACK, ACCOUNT_FIND_MAIL and each kind of reply */
struct kinder_packet *account_packet_decode(Uint8 *frame);
#endif

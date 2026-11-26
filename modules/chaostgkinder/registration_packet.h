/* the REGISTER service's packets. the client sends REGISTER_IDENTITY and
 * REGISTER_LIST_REQUEST, and the server answers with REGISTER_STATUS and
 * REGISTER_LIST */
#ifndef CHAOSTGKINDER_REGISTRATION_PACKET_H
#define CHAOSTGKINDER_REGISTRATION_PACKET_H
#include "chaostgkinder/packet.h"

enum registration_kind {
    REGISTER_IDENTITY = 16,
    REGISTER_STATUS = 17,
    REGISTER_LIST_REQUEST = 20,
    REGISTER_LIST = 21
};

struct identity_packet {
    struct kinder_packet header;
    char console_id[9];
    Uint32 flags;
    char greeting[128];
};

struct registration_status {
    struct kinder_packet header;
    int status; /* a u16 on the wire */
};

/* a list's frame holds REGISTER_ENTRIES entries, whatever its count */
#define REGISTER_ENTRIES 16
struct registration_entry {
    Uint32 id;
    Uint16 value;
};

struct registration_list {
    struct kinder_packet header;
    Uint32 count;
    struct registration_entry entries[REGISTER_ENTRIES];
};

void identity_packet_init(struct identity_packet *identity);
void registration_list_request_init(struct kinder_packet *request);
struct kinder_packet *registration_packet_decode(Uint8 *frame);
#endif

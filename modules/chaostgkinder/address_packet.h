/* the ADDRESS service's packets. a request names a service, and its reply
 * gives the service's IPv4 address and port */
#ifndef CHAOSTGKINDER_ADDRESS_PACKET_H
#define CHAOSTGKINDER_ADDRESS_PACKET_H
#include "chaostgkinder/packet.h"

enum address_packet_kind {
    ADDRESS_GREETING = 16,
    ADDRESS_ACK,
    ADDRESS_REQUEST,
    ADDRESS_REPLY
};

/* the bytes of a greeting's text, its NUL included */
#define ADDRESS_GREETING_BYTES 128

struct address_greeting {
    struct kinder_packet header;
    char text[ADDRESS_GREETING_BYTES];
};

/* the ack and the request. a request's value is an enum address_service_id.
 * no code reads an ack's */
struct address_value {
    struct kinder_packet header;
    int value;
};

struct address_reply {
    struct kinder_packet header;
    Uint32 address;
    Uint16 port;
};

void address_greeting_init(struct address_greeting *greeting);
void address_request_init(struct address_value *request);
/* packet_decode for the ADDRESS service, which also takes its four kinds */
struct kinder_packet *address_packet_decode(Uint8 *frame);
#endif

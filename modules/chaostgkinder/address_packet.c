#include <sg_xpt.h>
#include <shinobi.h>
#include "chaostgkinder/packet.h"
#include "chaostgkinder/address_packet.h"

static Uint8 *read_address_greeting(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_greeting *greeting = (struct address_greeting *)packet;
    cursor = packet_read_header(packet, cursor);
    packet_read_text(&cursor, greeting->text, sizeof(greeting->text));
    return cursor;
}

static Uint8 *write_address_greeting(struct kinder_packet *packet,
                                     Uint8 *cursor)
{
    struct address_greeting *greeting = (struct address_greeting *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_text(&cursor, greeting->text, sizeof(greeting->text));
    return cursor;
}

static struct packet_codec greeting_codec;

void address_greeting_init(struct address_greeting *greeting)
{
    packet_init(&greeting->header);
    greeting->header.kind = ADDRESS_GREETING;
    greeting->header.size += sizeof(greeting->text);
    greeting->header.codec = &greeting_codec;
}

static Uint8 *read_address_ack(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_value *ack = (struct address_value *)packet;
    cursor = packet_read_header(packet, cursor);
    ack->value = packet_read_u16(&cursor);
    return cursor;
}

static Uint8 *write_address_ack(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_value *ack = (struct address_value *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, (Uint16)ack->value);
    return cursor;
}

static struct packet_codec ack_codec;

static void address_ack_init(struct address_value *ack)
{
    packet_init(&ack->header);
    ack->header.kind = ADDRESS_ACK;
    ack->header.size += sizeof(Uint16);
    ack->header.codec = &ack_codec;
}

static Uint8 *read_address_request(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_value *request = (struct address_value *)packet;
    cursor = packet_read_header(packet, cursor);
    request->value = packet_read_u16(&cursor);
    return cursor;
}

static Uint8 *write_address_request(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_value *request = (struct address_value *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, (Uint16)request->value);
    return cursor;
}
static struct packet_codec request_codec;

void address_request_init(struct address_value *request)
{
    packet_init(&request->header);
    request->header.kind = ADDRESS_REQUEST;
    request->header.size += sizeof(Uint16);
    request->header.codec = &request_codec;
}

static Uint8 *read_address_reply(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_reply *reply = (struct address_reply *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->address = packet_read_u32(&cursor);
    reply->port = packet_read_u16(&cursor);
    return cursor;
}

static Uint8 *write_address_reply(struct kinder_packet *packet, Uint8 *cursor)
{
    struct address_reply *reply = (struct address_reply *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u32(&cursor, reply->address);
    packet_write_u16(&cursor, reply->port);
    return cursor;
}

static struct packet_codec reply_codec;

static void address_reply_init(struct address_reply *reply)
{
    packet_init(&reply->header);
    reply->header.kind = ADDRESS_REPLY;
    reply->header.size += sizeof(Uint32) + sizeof(Uint16);
    reply->header.codec = &reply_codec;
}

struct kinder_packet *address_packet_decode(Uint8 *frame)
{
    struct kinder_packet *packet = NULL;
    /* @hack spelling: each case's assignment inside its test, where a
     * statement of its own would do */
    switch (PACKET_FRAME_KIND(frame)) {
    case ADDRESS_GREETING:
        if ((packet = syCalloc(1, sizeof(struct address_greeting))) != NULL) {
            address_greeting_init((struct address_greeting *)packet);
        }
        break;
    case ADDRESS_ACK:
        if ((packet = syCalloc(1, sizeof(struct address_value))) != NULL) {
            address_ack_init((struct address_value *)packet);
        }
        break;
    case ADDRESS_REQUEST:
        if ((packet = syCalloc(1, sizeof(struct address_value))) != NULL) {
            address_request_init((struct address_value *)packet);
        }
        break;
    case ADDRESS_REPLY:
        if ((packet = syCalloc(1, sizeof(struct address_reply))) != NULL) {
            address_reply_init((struct address_reply *)packet);
        }
        break;
    }
    if (packet) {
#ifdef NONMATCHING
        /* this side refuses a frame whose size isn't the packet's, which the
         * read would take past */
        if (PACKET_FRAME_SIZE(frame) != packet->size) {
            packet_free(packet);
            return NULL;
        }
#endif
        packet->codec->read(packet, frame);
    } else {
        packet = packet_decode(frame);
    }
    return packet;
}

#pragma section ATA
static struct packet_codec greeting_codec = {read_address_greeting,
                                             write_address_greeting};
static struct packet_codec ack_codec = {read_address_ack, write_address_ack};
static struct packet_codec request_codec = {read_address_request,
                                            write_address_request};
static struct packet_codec reply_codec = {read_address_reply,
                                          write_address_reply};

#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/packet.h"

/* a disconnect or notice, its code and its text */
struct notice_packet {
    struct kinder_packet header;
    Uint16 code;
    char text[PACKET_TEXT_SIZE];
};

/* @hack redundant: external linkage, which no other object needs, where
 * static would do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void empty_packet_init(struct kinder_packet *packet);
void disconnect_packet_init(struct notice_packet *notice);
void notice_packet_init(struct notice_packet *notice);
void ping_packet_init(struct ping_packet *ping);
void greeting_packet_init(struct greeting_packet *greeting);
/* NOLINTEND(misc-use-internal-linkage) */

Uint8 packet_read_u8(Uint8 **cursor)
{
    return *(*cursor)++;
}

Uint16 packet_read_u16(Uint8 **cursor)
{
    Uint16 value = *(*cursor)++;
    value |= *(*cursor)++ << 8;
    return value;
}

Uint32 packet_read_u32(Uint8 **cursor)
{
    Uint32 value = *(*cursor)++;
    value |= (Uint32) * (*cursor)++ << 8;
    value |= (Uint32) * (*cursor)++ << 16;
    value |= (Uint32) * (*cursor)++ << 24;
    return value;
}

void packet_read_bytes(Uint8 **cursor, void *data, Uint32 size)
{
    memcpy(data, *cursor, size);
    *cursor += size;
}

void packet_read_text(Uint8 **cursor, char *text, Uint32 size)
{
#ifdef NONMATCHING
    /* this side writes nothing for a size of 0, which the size - 1 below would
     * wrap */
    if (size == 0) {
        return;
    }
#endif
    memset(text, 0, size);
    strncpy(text, (const char *)*cursor, size - 1);
    *cursor += size;
}

void packet_write_u8(Uint8 **cursor, Uint8 value)
{
    *(*cursor)++ = value;
}

void packet_write_u16(Uint8 **cursor, Uint16 value)
{
    *(*cursor)++ = (Uint8)value;
    *(*cursor)++ = (Uint8)(value >> 8);
}

void packet_write_u32(Uint8 **cursor, Uint32 value)
{
    *(*cursor)++ = (Uint8)value;
    *(*cursor)++ = (Uint8)(value >> 8);
    *(*cursor)++ = (Uint8)(value >> 16);
    *(*cursor)++ = (Uint8)(value >> 24);
}

void packet_write_bytes(Uint8 **cursor, const void *data, Uint32 size)
{
    memcpy(*cursor, data, size);
    *cursor += size;
}

void packet_write_text(Uint8 **cursor, const char *text, Uint32 size)
{
#ifdef NONMATCHING
    /* this side writes nothing for a size of 0, which the size - 1 below would
     * wrap */
    if (size == 0) {
        return;
    }
#endif
    memset(*cursor, 0, size);
    strncpy((char *)*cursor, text, size - 1);
    *cursor += size;
}

Uint8 *packet_read_header(struct kinder_packet *packet, Uint8 *cursor)
{
    packet->size = packet_read_u16(&cursor);
    packet->kind = packet_read_u16(&cursor);
    return cursor;
}

Uint8 *packet_write_header(struct kinder_packet *packet, Uint8 *cursor)
{
    packet_write_u16(&cursor, packet->size);
    packet_write_u16(&cursor, (Uint16)packet->kind);
    return cursor;
}

static const struct packet_codec header_codec = {packet_read_header,
                                                 packet_write_header};

void packet_init(struct kinder_packet *packet)
{
    packet->kind = -1;
    packet->size = PACKET_HEADER_SIZE;
    packet->codec = &header_codec;
}

void empty_packet_init(struct kinder_packet *packet)
{
    packet_init(packet);
    packet->kind = PACKET_EMPTY;
}

static Uint8 *read_disconnect(struct kinder_packet *packet, Uint8 *cursor)
{
    struct notice_packet *notice = (struct notice_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    notice->code = packet_read_u16(&cursor);
    packet_read_text(&cursor, notice->text, sizeof(notice->text));
    return cursor;
}

static Uint8 *write_disconnect(struct kinder_packet *packet, Uint8 *cursor)
{
    struct notice_packet *notice = (struct notice_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, notice->code);
    packet_write_text(&cursor, notice->text, sizeof(notice->text));
    return cursor;
}

static const struct packet_codec disconnect_codec = {read_disconnect,
                                                     write_disconnect};

void disconnect_packet_init(struct notice_packet *notice)
{
    packet_init(&notice->header);
    notice->header.kind = PACKET_DISCONNECT;
    notice->header.size += sizeof(notice->code) + sizeof(notice->text);
    notice->header.codec = &disconnect_codec;
}

static Uint8 *read_notice(struct kinder_packet *packet, Uint8 *cursor)
{
    struct notice_packet *notice = (struct notice_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    notice->code = packet_read_u16(&cursor);
    packet_read_text(&cursor, notice->text, sizeof(notice->text));
    return cursor;
}

static Uint8 *write_notice(struct kinder_packet *packet, Uint8 *cursor)
{
    struct notice_packet *notice = (struct notice_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, notice->code);
    packet_write_text(&cursor, notice->text, sizeof(notice->text));
    return cursor;
}

static const struct packet_codec notice_codec = {read_notice, write_notice};

void notice_packet_init(struct notice_packet *notice)
{
    packet_init(&notice->header);
    notice->header.kind = PACKET_NOTICE;
    notice->header.size += sizeof(notice->code) + sizeof(notice->text);
    notice->header.codec = &notice_codec;
}

static Uint8 *read_ping(struct kinder_packet *packet, Uint8 *cursor)
{
    struct ping_packet *ping = (struct ping_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    ping->cookie = packet_read_u32(&cursor);
    return cursor;
}

static Uint8 *write_ping(struct kinder_packet *packet, Uint8 *cursor)
{
    struct ping_packet *ping = (struct ping_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u32(&cursor, ping->cookie);
    return cursor;
}

static const struct packet_codec ping_codec = {read_ping, write_ping};

void ping_packet_init(struct ping_packet *ping)
{
    packet_init(&ping->header);
    ping->header.kind = PACKET_PING;
    ping->header.size += sizeof(ping->cookie);
    ping->header.codec = &ping_codec;
}

static Uint8 *read_pong(struct kinder_packet *packet, Uint8 *cursor)
{
    struct ping_packet *pong = (struct ping_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    pong->cookie = packet_read_u32(&cursor);
    return cursor;
}

static Uint8 *write_pong(struct kinder_packet *packet, Uint8 *cursor)
{
    struct ping_packet *pong = (struct ping_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u32(&cursor, pong->cookie);
    return cursor;
}

static const struct packet_codec pong_codec = {read_pong, write_pong};

void pong_packet_init(struct ping_packet *pong)
{
    packet_init(&pong->header);
    pong->header.kind = PACKET_PONG;
    pong->header.size += sizeof(pong->cookie);
    pong->header.codec = &pong_codec;
}

static Uint8 *read_greeting(struct kinder_packet *packet, Uint8 *cursor)
{
    struct greeting_packet *greeting = (struct greeting_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    greeting->receive_seed = packet_read_u32(&cursor);
    greeting->send_seed = packet_read_u32(&cursor);
    packet_read_text(&cursor, greeting->text, sizeof(greeting->text));
    return cursor;
}

static Uint8 *write_greeting(struct kinder_packet *packet, Uint8 *cursor)
{
    struct greeting_packet *greeting = (struct greeting_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u32(&cursor, greeting->receive_seed);
    packet_write_u32(&cursor, greeting->send_seed);
    packet_write_text(&cursor, greeting->text, sizeof(greeting->text));
    return cursor;
}

static const struct packet_codec greeting_codec = {read_greeting,
                                                   write_greeting};

void greeting_packet_init(struct greeting_packet *greeting)
{
    packet_init(&greeting->header);
    greeting->header.kind = PACKET_GREETING;
    greeting->header.size += sizeof(greeting->receive_seed) +
                             sizeof(greeting->send_seed) +
                             sizeof(greeting->text);
    greeting->header.codec = &greeting_codec;
}

struct kinder_packet *packet_decode(Uint8 *frame)
{
    struct kinder_packet *packet = NULL;
    switch (PACKET_FRAME_KIND(frame)) {
    case PACKET_EMPTY:
        if ((packet = syCalloc(1, sizeof(*packet))) != NULL) {
            empty_packet_init(packet);
        }
        break;
    case PACKET_DISCONNECT:
        if ((packet = syCalloc(1, sizeof(struct notice_packet))) != NULL) {
            disconnect_packet_init((struct notice_packet *)packet);
        }
        break;
    case PACKET_NOTICE:
        if ((packet = syCalloc(1, sizeof(struct notice_packet))) != NULL) {
            notice_packet_init((struct notice_packet *)packet);
        }
        break;
    case PACKET_PING:
        if ((packet = syCalloc(1, sizeof(struct ping_packet))) != NULL) {
            ping_packet_init((struct ping_packet *)packet);
        }
        break;
    case PACKET_PONG:
        if ((packet = syCalloc(1, sizeof(struct ping_packet))) != NULL) {
            pong_packet_init((struct ping_packet *)packet);
        }
        break;
    case PACKET_GREETING:
        if ((packet = syCalloc(1, sizeof(struct greeting_packet))) != NULL) {
            greeting_packet_init((struct greeting_packet *)packet);
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
    }
    return packet;
}

void packet_free(struct kinder_packet *packet)
{
    syFree(packet);
}

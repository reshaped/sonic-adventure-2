#include <sg_xpt.h>
#include <shinobi.h>
#include "chaostgkinder/account_packet.h"
#include "chaostgkinder/packet.h"

/* each kind has a read and a write of its own, even where two kinds'
 * payloads are alike */
#define TEXT_CODEC(name, type, field)                                          \
    static Uint8 *read_##name(struct kinder_packet *packet, Uint8 *cursor)     \
    {                                                                          \
        struct type *message = (struct type *)packet;                          \
        cursor = packet_read_header(packet, cursor);                           \
        packet_read_text(&cursor, message->field, sizeof(message->field));     \
        return cursor;                                                         \
    }                                                                          \
    static Uint8 *write_##name(struct kinder_packet *packet, Uint8 *cursor)    \
    {                                                                          \
        struct type *message = (struct type *)packet;                          \
        cursor = packet_write_header(packet, cursor);                          \
        packet_write_text(&cursor, message->field, sizeof(message->field));    \
        return cursor;                                                         \
    }

#define STATUS_CODEC(name)                                                     \
    static Uint8 *read_##name(struct kinder_packet *packet, Uint8 *cursor)     \
    {                                                                          \
        cursor = packet_read_header(packet, cursor);                           \
        ((struct account_status_packet *)packet)->status =                     \
            packet_read_u16(&cursor);                                          \
        return cursor;                                                         \
    }                                                                          \
    static Uint8 *write_##name(struct kinder_packet *packet, Uint8 *cursor)    \
    {                                                                          \
        cursor = packet_write_header(packet, cursor);                          \
        packet_write_u16(                                                      \
            &cursor,                                                           \
            (Uint16)((struct account_status_packet *)packet)->status);         \
        return cursor;                                                         \
    }

/* MESSAGE_INIT and REPLY_INIT define a kind's constructor, name##_init, and
 * declare its codec, which the source's end defines. a reply's constructor is
 * static: only account_packet_decode calls it */
#define CONSTRUCTOR(name, type, kind_code, payload_size)                       \
    void name##_init(struct type *message)                                     \
    {                                                                          \
        packet_init(&message->header);                                         \
        message->header.kind = kind_code;                                      \
        message->header.size += (payload_size);                                \
        message->header.codec = &name##_codec;                                 \
    }
#define MESSAGE_INIT(name, type, kind_code, payload_size)                      \
    static struct packet_codec name##_codec;                                   \
    CONSTRUCTOR(name, type, kind_code, payload_size)
#define REPLY_INIT(name, type, kind_code, payload_size)                        \
    static struct packet_codec name##_codec;                                   \
    static CONSTRUCTOR(name, type, kind_code, payload_size)

#define EMPTY_CODEC(name, kind_code)                                           \
    static struct packet_codec name##_codec;                                   \
    static Uint8 *read_##name(struct kinder_packet *packet, Uint8 *cursor)     \
    {                                                                          \
        cursor = packet_read_header(packet, cursor);                           \
        return cursor;                                                         \
    }                                                                          \
    static Uint8 *write_##name(struct kinder_packet *packet, Uint8 *cursor)    \
    {                                                                          \
        cursor = packet_write_header(packet, cursor);                          \
        return cursor;                                                         \
    }                                                                          \
    void name##_init(struct kinder_packet *packet)                             \
    {                                                                          \
        packet_init(packet);                                                   \
        packet->kind = kind_code;                                              \
        packet->codec = &name##_codec;                                         \
    }

TEXT_CODEC(account_greeting, account_greeting, text)
MESSAGE_INIT(account_greeting, account_greeting, ACCOUNT_GREETING,
             sizeof(message->text))

STATUS_CODEC(account_ack)
REPLY_INIT(account_ack, account_status_packet, ACCOUNT_ACK, sizeof(Uint16))

TEXT_CODEC(account_mail, account_mail, mail)
MESSAGE_INIT(account_mail, account_mail, ACCOUNT_FIND_MAIL,
             sizeof(message->mail))

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side. the name's read and
 * write move all its codes' bytes */
#define MAIL_NAME_BYTES (2 * ACCOUNT_NAME_CODES)
#else
/* @bug logic: the name's read and write move 17 bytes, one for each of its
 * codes, of the 34 the packet's size counts. a reply keeps the name's first 8
 * codes and the low byte of its ninth, at 0x0c9dcd7e and 0x0c9dcdaa */
#define MAIL_NAME_BYTES ACCOUNT_NAME_CODES
#endif

static Uint8 *read_account_mail_reply(struct kinder_packet *packet,
                                      Uint8 *cursor)
{
    struct account_mail_reply *reply = (struct account_mail_reply *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->user_id = packet_read_u32(&cursor);
    packet_read_bytes(&cursor, reply->name, MAIL_NAME_BYTES);
    return cursor;
}
static Uint8 *write_account_mail_reply(struct kinder_packet *packet,
                                       Uint8 *cursor)
{
    struct account_mail_reply *reply = (struct account_mail_reply *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u32(&cursor, reply->user_id);
    packet_write_bytes(&cursor, reply->name, MAIL_NAME_BYTES);
    return cursor;
}
REPLY_INIT(account_mail_reply, account_mail_reply, ACCOUNT_MAIL_REPLY,
           sizeof(Uint32) + sizeof(message->name))

static Uint8 *read_account_name(struct kinder_packet *packet, Uint8 *cursor)
{
    struct account_name *name = (struct account_name *)packet;
    cursor = packet_read_header(packet, cursor);
    packet_read_bytes(&cursor, name->name, sizeof(name->name));
    return cursor;
}
static Uint8 *write_account_name(struct kinder_packet *packet, Uint8 *cursor)
{
    struct account_name *name = (struct account_name *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_bytes(&cursor, name->name, sizeof(name->name));
    return cursor;
}
MESSAGE_INIT(account_name, account_name, ACCOUNT_CHECK_NAME,
             sizeof(message->name))

static Uint8 *read_account_name_reply(struct kinder_packet *packet,
                                      Uint8 *cursor)
{
    struct account_value_reply *reply = (struct account_value_reply *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->status = packet_read_u8(&cursor) != 0;
    reply->value = packet_read_u32(&cursor);
    return cursor;
}
static Uint8 *write_account_name_reply(struct kinder_packet *packet,
                                       Uint8 *cursor)
{
    struct account_value_reply *reply = (struct account_value_reply *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u8(&cursor, reply->status ? 1 : 0);
    packet_write_u32(&cursor, reply->value);
    return cursor;
}
REPLY_INIT(account_name_reply, account_value_reply, ACCOUNT_NAME_REPLY,
           sizeof(Uint8) + sizeof(Uint32))

#define PROFILE_CODEC(codec_name, type)                                        \
    static Uint8 *read_##codec_name(struct kinder_packet *packet,              \
                                    Uint8 *cursor)                             \
    {                                                                          \
        struct type *message = (struct type *)packet;                          \
        cursor = packet_read_header(packet, cursor);                           \
        packet_read_text(&cursor, message->profile.name,                       \
                         sizeof(message->profile.name));                       \
        packet_read_text(&cursor, message->profile.text,                       \
                         sizeof(message->profile.text));                       \
        message->profile.flag = packet_read_u8(&cursor) != 0;                  \
        message->profile.group = (Sint16)packet_read_u16(&cursor);             \
        message->profile.choice = (Sint16)packet_read_u16(&cursor);            \
        return cursor;                                                         \
    }                                                                          \
    static Uint8 *write_##codec_name(struct kinder_packet *packet,             \
                                     Uint8 *cursor)                            \
    {                                                                          \
        struct type *message = (struct type *)packet;                          \
        cursor = packet_write_header(packet, cursor);                          \
        packet_write_text(&cursor, message->profile.name,                      \
                          sizeof(message->profile.name));                      \
        packet_write_text(&cursor, message->profile.text,                      \
                          sizeof(message->profile.text));                      \
        packet_write_u8(&cursor, (Uint8)message->profile.flag);                \
        packet_write_u16(&cursor, (Uint16)message->profile.group);             \
        packet_write_u16(&cursor, (Uint16)message->profile.choice);            \
        return cursor;                                                         \
    }

#define PROFILE_BYTES(profile)                                                 \
    (sizeof((profile).name) + sizeof((profile).text) + sizeof(Uint8) +         \
     sizeof(Uint16) + sizeof(Uint16))
PROFILE_CODEC(account_create, account_create)
MESSAGE_INIT(account_create, account_create, ACCOUNT_CREATE_PROFILE,
             PROFILE_BYTES(message->profile))

static Uint8 *read_account_create_reply(struct kinder_packet *packet,
                                        Uint8 *cursor)
{
    struct account_create_reply *reply = (struct account_create_reply *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->status = (Sint16)packet_read_u16(&cursor);
    reply->value = packet_read_u32(&cursor);
    return cursor;
}
static Uint8 *write_account_create_reply(struct kinder_packet *packet,
                                         Uint8 *cursor)
{
    struct account_create_reply *reply = (struct account_create_reply *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, (Uint16)reply->status);
    packet_write_u32(&cursor, reply->value);
    return cursor;
}
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: the size counts the
 * value too */
#define CREATE_REPLY_BYTES (sizeof(Uint16) + sizeof(Uint32))
#else
/* @bug logic: the packet's size counts the status and leaves out the value,
 * whose 4 bytes the reply's read and write move, at 0x0c9dd040 */
#define CREATE_REPLY_BYTES sizeof(Uint16)
#endif
REPLY_INIT(account_create_reply, account_create_reply, ACCOUNT_CREATE_REPLY,
           CREATE_REPLY_BYTES)
TEXT_CODEC(account_find, account_find, name)
MESSAGE_INIT(account_find, account_find, ACCOUNT_FIND_PROFILE,
             sizeof(message->name))

static Uint8 *read_account_find_reply(struct kinder_packet *packet,
                                      Uint8 *cursor)
{
    struct account_value_reply *reply = (struct account_value_reply *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->status = packet_read_u16(&cursor);
    reply->value = packet_read_u32(&cursor);
    return cursor;
}
static Uint8 *write_account_find_reply(struct kinder_packet *packet,
                                       Uint8 *cursor)
{
    struct account_value_reply *reply = (struct account_value_reply *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, (Uint16)reply->status);
    packet_write_u32(&cursor, reply->value);
    return cursor;
}
REPLY_INIT(account_find_reply, account_value_reply, ACCOUNT_FIND_REPLY,
           sizeof(Uint16) + sizeof(Uint32))
EMPTY_CODEC(account_read, ACCOUNT_READ_PROFILE)
PROFILE_CODEC(account_read_reply, account_record)
REPLY_INIT(account_read_reply, account_record, ACCOUNT_READ_REPLY,
           PROFILE_BYTES(message->profile))
PROFILE_CODEC(account_write, account_record)
MESSAGE_INIT(account_write, account_record, ACCOUNT_WRITE_PROFILE,
             PROFILE_BYTES(message->profile))
STATUS_CODEC(account_write_reply)
REPLY_INIT(account_write_reply, account_status_packet, ACCOUNT_WRITE_REPLY,
           sizeof(Uint16))
/* @hack type: external linkage for account_status_init, which no code calls,
 * where static would do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
EMPTY_CODEC(account_status, ACCOUNT_STATUS)
STATUS_CODEC(account_status_reply)
REPLY_INIT(account_status_reply, account_status_packet, ACCOUNT_STATUS_REPLY,
           sizeof(Uint16))

struct kinder_packet *account_packet_decode(Uint8 *frame)
{
    struct kinder_packet *packet = NULL;
    switch (PACKET_FRAME_KIND(frame)) {
/* @hack spelling: the assignment inside the test, where a statement of its
 * own would do */
#define DECODE_CASE(kind_code, type, init)                                     \
    case kind_code:                                                            \
        if ((packet = syCalloc(1, sizeof(struct type))) != NULL) {             \
            init((struct type *)packet);                                       \
        }                                                                      \
        break
        DECODE_CASE(ACCOUNT_GREETING, account_greeting, account_greeting_init);
        DECODE_CASE(ACCOUNT_ACK, account_status_packet, account_ack_init);
        DECODE_CASE(ACCOUNT_FIND_MAIL, account_mail, account_mail_init);
        DECODE_CASE(ACCOUNT_MAIL_REPLY, account_mail_reply,
                    account_mail_reply_init);
        DECODE_CASE(ACCOUNT_NAME_REPLY, account_value_reply,
                    account_name_reply_init);
        DECODE_CASE(ACCOUNT_CREATE_REPLY, account_create_reply,
                    account_create_reply_init);
        DECODE_CASE(ACCOUNT_FIND_REPLY, account_value_reply,
                    account_find_reply_init);
        DECODE_CASE(ACCOUNT_READ_REPLY, account_record,
                    account_read_reply_init);
        DECODE_CASE(ACCOUNT_WRITE_REPLY, account_status_packet,
                    account_write_reply_init);
        DECODE_CASE(ACCOUNT_STATUS_REPLY, account_status_packet,
                    account_status_reply_init);
#undef DECODE_CASE
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
#define CODEC(name)                                                            \
    static struct packet_codec name##_codec = {read_##name, write_##name}
CODEC(account_greeting);
CODEC(account_ack);
CODEC(account_mail);
CODEC(account_mail_reply);
CODEC(account_name);
CODEC(account_name_reply);
CODEC(account_create);
CODEC(account_create_reply);
CODEC(account_find);
CODEC(account_find_reply);
CODEC(account_read);
CODEC(account_read_reply);
CODEC(account_write);
CODEC(account_write_reply);
CODEC(account_status);
CODEC(account_status_reply);

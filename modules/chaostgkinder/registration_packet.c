#include <sg_xpt.h>
#include <shinobi.h>
#include "chaostgkinder/packet.h"
#include "chaostgkinder/registration_packet.h"

static Uint8 *read_identity(struct kinder_packet *packet, Uint8 *cursor)
{
    struct identity_packet *identity = (struct identity_packet *)packet;
    cursor = packet_read_header(packet, cursor);
    packet_read_text(&cursor, identity->console_id,
                     sizeof(identity->console_id));
    identity->flags = packet_read_u32(&cursor);
    packet_read_text(&cursor, identity->greeting, sizeof(identity->greeting));
    return cursor;
}

static Uint8 *write_identity(struct kinder_packet *packet, Uint8 *cursor)
{
    struct identity_packet *identity = (struct identity_packet *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_text(&cursor, identity->console_id,
                      sizeof(identity->console_id));
    packet_write_u32(&cursor, identity->flags);
    packet_write_text(&cursor, identity->greeting, sizeof(identity->greeting));
    return cursor;
}
static const struct packet_codec identity_codec = {read_identity,
                                                   write_identity};

void identity_packet_init(struct identity_packet *identity)
{
    packet_init(&identity->header);
    identity->header.kind = REGISTER_IDENTITY;
    identity->header.size += sizeof(identity->console_id) +
                             sizeof(identity->flags) +
                             sizeof(identity->greeting);
    identity->header.codec = &identity_codec;
}

static Uint8 *read_registration_status(struct kinder_packet *packet,
                                       Uint8 *cursor)
{
    struct registration_status *reply = (struct registration_status *)packet;
    cursor = packet_read_header(packet, cursor);
    reply->status = packet_read_u16(&cursor);
    return cursor;
}

static Uint8 *write_registration_status(struct kinder_packet *packet,
                                        Uint8 *cursor)
{
    struct registration_status *reply = (struct registration_status *)packet;
    cursor = packet_write_header(packet, cursor);
    packet_write_u16(&cursor, (Uint16)reply->status);
    return cursor;
}
static const struct packet_codec status_codec = {read_registration_status,
                                                 write_registration_status};

static void registration_status_init(struct registration_status *reply)
{
    packet_init(&reply->header);
    reply->header.kind = REGISTER_STATUS;
    reply->header.size += sizeof(Uint16);
    reply->header.codec = &status_codec;
}

void registration_list_request_init(struct kinder_packet *request)
{
    packet_init(request);
    request->kind = REGISTER_LIST_REQUEST;
}

static Uint8 *read_registration_list(struct kinder_packet *packet,
                                     Uint8 *cursor)
{
    struct registration_list *list = (struct registration_list *)packet;
    int i;
    cursor = packet_read_header(packet, cursor);
    list->count = packet_read_u16(&cursor);
    /* @hack type: i compared as unsigned, where an int compare would do */
    for (i = 0; (unsigned int)i < REGISTER_ENTRIES; i++) {
        list->entries[i].id = packet_read_u32(&cursor);
        list->entries[i].value = packet_read_u16(&cursor);
    }
    return cursor;
}

static Uint8 *write_registration_list(struct kinder_packet *packet,
                                      Uint8 *cursor)
{
    struct registration_list *list = (struct registration_list *)packet;
    int i;
    cursor = packet_write_header(packet, cursor);
    /* @bug logic: writes no count before the entries, where
     * read_registration_list reads one. registration_list_init's size leaves
     * the count out too (0x0c9c7b12, 0x0c9c7b76) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the write puts the count first */
    packet_write_u16(&cursor, (Uint16)list->count);
#endif
    /* @hack type: i compared as unsigned, where an int compare would do */
    for (i = 0; (unsigned int)i < REGISTER_ENTRIES; i++) {
        packet_write_u32(&cursor, list->entries[i].id);
        packet_write_u16(&cursor, list->entries[i].value);
    }
    return cursor;
}
/* @hack type: list_codec without the const the other codecs have, where const
 * would do */
static struct packet_codec list_codec = {read_registration_list,
                                         write_registration_list};

static void registration_list_init(struct registration_list *list)
{
    packet_init(&list->header);
    list->header.kind = REGISTER_LIST;
#ifdef NONMATCHING
    /* this side repairs the bug tagged at the list's write: the size counts the
     * count */
    list->header.size += sizeof(Uint16);
#endif
    list->header.size += REGISTER_ENTRIES * (sizeof(list->entries[0].id) +
                                             sizeof(list->entries[0].value));
    list->header.codec = &list_codec;
}

struct kinder_packet *registration_packet_decode(Uint8 *frame)
{
    struct kinder_packet *packet = NULL;
    switch (PACKET_FRAME_KIND(frame)) {
    case REGISTER_IDENTITY:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((packet = syCalloc(1, sizeof(struct identity_packet))) != NULL) {
            identity_packet_init((struct identity_packet *)packet);
        }
        break;
    case REGISTER_STATUS:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((packet = syCalloc(1, sizeof(struct registration_status))) !=
            NULL) {
            registration_status_init((struct registration_status *)packet);
        }
        break;
    case REGISTER_LIST_REQUEST:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((packet = syCalloc(1, sizeof(*packet))) != NULL) {
            registration_list_request_init(packet);
        }
        break;
    case REGISTER_LIST:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((packet = syCalloc(1, sizeof(struct registration_list))) != NULL) {
            registration_list_init((struct registration_list *)packet);
        }
        break;
    }
    if (packet) {
#ifdef NONMATCHING
        /* this side refuses a frame whose size isn't the packet's. it refuses a
         * list of more than REGISTER_ENTRIES too, which the read would take
         * past */
        if (PACKET_FRAME_SIZE(frame) != packet->size ||
            (packet->kind == REGISTER_LIST &&
             PACKET_FRAME_HALFWORD(frame, PACKET_HEADER_SIZE) >
                 REGISTER_ENTRIES)) {
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

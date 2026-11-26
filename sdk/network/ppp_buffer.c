#include <sg_xpt.h>
#include <string.h>
#include "ppp_packet.h"

/* the frames PP_alloc_mbuf_work hands out, by index */
#define WORK_FRAMES 2

#ifdef DREAMCAST
static Uint8 pull_word[4], pull_byte;
#endif

static struct ppp_frame {
    struct ppp_buffer packet;
    Uint8 headroom[PPP_HEADROOM];
    Uint8 payload[PPP_FRAME_DATA_BYTES];
} frames[WORK_FRAMES];
static struct {
    struct ppp_buffer packet;
    Uint8 headroom[PPP_HEADROOM];
    Uint8 payload[PPP_SHORT_FRAME_BYTES];
} short_frame;

struct ppp_buffer *PP_alloc_mbuf_work(Uint16 index)
{
    struct ppp_buffer *packet;
    /* @hack spelling: the test of 0 and 1, where index < WORK_FRAMES would
     * do */
    if (index == 0 || index == 1) {
        /* @hack type: the frame's address cast to its packet's, where
         * &frames[index].packet would do */
        packet = (struct ppp_buffer *)&frames[index];
        packet->data = frames[index].payload;
        packet->length = 0;
        return packet;
    }
    return NULL;
}
short PP_len_p(const struct ppp_buffer *packet)
{
    return (short)packet->length;
}
void PP_trim_mbuf(struct ppp_buffer **cursor, short length)
{
    struct ppp_buffer *packet;
    if (!cursor || !*cursor) {
        return;
    }
#ifdef NONMATCHING
    /* this side trims nothing for a negative length, which would leave the
     * packet a negative length */
    if (length <= 0) {
#else
    if (length == 0) {
#endif
        return;
    }
    packet = *cursor;
    if (packet->length >= length) {
        packet->length = length;
    }
}
short PP_pullup(struct ppp_buffer **cursor, Uint8 *destination, Uint16 length)
{
    struct ppp_buffer *packet;
    short amount;
    short copied;
    copied = 0;
    if (cursor == NULL) {
        return 0;
    }
    while (length != 0 && (packet = *cursor) != NULL) {
        amount = length < packet->length ? length : packet->length;
        if (destination != NULL) {
            if (amount == 1) {
                *destination = *packet->data;
            } else if (amount > 1) {
                memcpy(destination, packet->data, amount);
            }
            destination += amount;
        }
        copied += amount;
        length -= amount;
        packet->data += amount;
        packet->length -= amount;
        if (packet->length == 0) {
            length = 0;
        }
    }
    return copied;
}
struct ppp_buffer *PP_pushdown(struct ppp_buffer *packet, short length)
{
#ifdef NONMATCHING
    /* this side refuses a negative length, which the test below always
     * passes */
    if (length < 0) {
        return NULL;
    }
#endif
    if (packet && packet->data - (Uint8 *)(packet + 1) >= length) {
        packet->data -= length;
        packet->length += length;
        return packet;
    }
    return NULL;
}
struct ppp_buffer *PP_qdata(const Uint8 *source, Uint16 length)
{
    struct ppp_buffer *packet;
#ifdef NONMATCHING
    /* this side refuses a length past the payload's room, and a length with no
     * source. it finds the payload by its own name, the plain form the hack tag
     * below gives */
    if (length > sizeof(short_frame.payload) || (!source && length)) {
        return NULL;
    }
    packet = &short_frame.packet;
    packet->data = short_frame.payload;
    if (length) {
        memcpy(packet->data, source, length);
    }
#else
    Uint8 *data;
    /* @hack spelling: the payload's address stepped from the frame's, where
     * short_frame.payload would do */
    data = (Uint8 *)&short_frame;
    packet = (struct ppp_buffer *)data;
    data += sizeof(*packet) + PPP_HEADROOM;
    packet->data = data;
    memcpy(packet->data, source, length);
#endif
    packet->length = length;
    return packet;
}
Uint16 PP_dqdata(const struct ppp_buffer *packet, Uint8 *destination,
                 Uint16 length)
{
    Uint16 amount;
    if (!destination) {
        return 0;
    }
    amount = packet->length < length ? packet->length : length;
    memcpy(destination, packet->data, amount);
    return amount;
}

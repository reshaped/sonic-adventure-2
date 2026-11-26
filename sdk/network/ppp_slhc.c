#include <sg_xpt.h>
#include <string.h>
#include "ppp_framing.h"
#include "ppp_ip.h"
#include "ppp_packet.h"

enum {
    NO_SLOT = 255,
    TCP_PROTOCOL = 6,
    NEW_URGENT = 1,
    NEW_WINDOW = 2,
    NEW_ACK = 4,
    NEW_SEQUENCE = 8,
    PUSH = 16,
    NEW_ID = 32,
    NEW_SLOT = 64,
    INTERACTIVE = NEW_SEQUENCE | NEW_WINDOW | NEW_URGENT,
    ONE_WAY = NEW_SEQUENCE | NEW_ACK | NEW_WINDOW | NEW_URGENT,
    CHANGE_FIELDS = ONE_WAY,
    DISCARD_IMPLICIT_SLOT = 1,
    /* the change mask and the checksum that start every compressed header */
    COMPRESSED_HEADER_BYTES = 3,
    /* the slot number after them, where NEW_SLOT is set */
    SLOT_NUMBER_BYTES = 1,
    ENCODED_DELTA_BYTES = 16
};

struct connection_slot {
    Uint8 number;
    /* for a send slot, the next older one. the oldest's is the newest */
    struct connection_slot *next;
    struct ppp_ip ip;
    struct ppp_tcp tcp;
};
struct ppp_compression {
    struct connection_slot *send, *receive;
    Uint8 last_send_slot, last_receive_slot;
    Uint8 oldest, current_send, current_receive, flags;
    Uint32 sent_other, sent_tcp, sent_full, sent_delta;
    Uint32 search_steps, misses;
    Uint32 received_full, received_delta, errors, discarded;
};

static Uint8 pull_word[4], pull_byte;
static struct ppp_compression compression_state;
static struct connection_slot receive_slots[VJ_SLOTS], send_slots[VJ_SLOTS];
/* the deltas of the header being compressed, up to five. a delta from 1 to
 * 255 takes one byte. any other takes a zero byte and a 16-bit word */
static Uint8 encoded_deltas[ENCODED_DELTA_BYTES];

#ifdef NONMATCHING
/* this side pulls every octet through PP_pullup, as PPP_PULL_BYTE does.
 * ppp_packet.h's matching pull_octet takes one straight off the cursor while
 * more than one remains */
static int pull_octet(struct ppp_buffer **packet)
{
    return PP_pullup(packet, &pull_byte, 1) == 1 ? pull_byte : -1;
}
#endif

static Uint8 *encode_delta(Uint8 *out, Uint16 value)
{
    if (value >= 256 || value == 0) {
        *out++ = 0;
        out = PP_put16(out, value);
    } else {
        *out++ = value;
    }
    return out;
}

static Sint32 decode_delta(struct ppp_buffer **packet)
{
    int value = pull_octet(packet);
    if (value == 0) {
        value = PPP_PULL16(packet);
    }
    return value;
}

struct ppp_compression *PP_slhc_init(short receive_count, short send_count)
{
    short i;
    struct connection_slot *slots;
    struct ppp_compression *state = &compression_state;
#ifdef NONMATCHING
    /* this side refuses a count past VJ_SLOTS, and starts the state and every
     * slot cleared */
    if (receive_count < 0 || receive_count > VJ_SLOTS || send_count < 0 ||
        send_count > VJ_SLOTS) {
        return NULL;
    }
    memset(state, 0, sizeof(*state));
    memset(receive_slots, 0, sizeof(receive_slots));
    memset(send_slots, 0, sizeof(send_slots));
#endif
    if (receive_count > 0 && receive_count <= VJ_SLOTS) {
        state->receive = receive_slots;
        state->last_receive_slot = receive_count - 1;
    }
    if (send_count > 0 && send_count <= VJ_SLOTS) {
        state->send = send_slots;
        state->last_send_slot = send_count - 1;
    }
    state->oldest = 0;
    state->current_send = NO_SLOT;
    state->current_receive = NO_SLOT;
    if (send_count > 0) {
        slots = state->send;
        for (i = state->last_send_slot; i > 0; i--) {
            slots[i].number = i;
            slots[i].next = &slots[i - 1];
        }
        slots[0].next = &slots[state->last_send_slot];
        slots[0].number = 0;
    }
    return state;
}

void PP_slhc_free(struct ppp_compression *state)
{
#ifdef NONMATCHING
    /* this side does nothing, the plain form the hack tag below gives */
    (void)state;
#else
    /* @hack redundant: a test of state, after which it returns either way */
    if (state == NULL) {
        return;
    }
#endif
}

short PP_slhc_compress(struct ppp_compression *state,
                       struct ppp_buffer **packet, short compress_slot)
{
    struct connection_slot *oldest;
    struct connection_slot *previous;
    struct connection_slot *slot;
    short header_length;
    struct ppp_tcp *last;
    Uint32 sequence_delta;
    Uint32 ack_delta;
    short changes;
    Uint8 *out;
    struct ppp_buffer *compressed;
    struct ppp_tcp tcp;
    struct ppp_ip ip;

#ifdef NONMATCHING
    /* this side refuses a missing state or packet, sends plain IP while the
     * state has no send slots, and starts both headers cleared for the reads
     * below */
    Uint16 tcp_length;
    if (!state || !packet || !*packet || !(*packet)->data) {
        return -1;
    }
    if (!state->send) {
        return VJ_PLAIN_IP;
    }
    memset(&ip, 0, sizeof(ip));
    memset(&tcp, 0, sizeof(tcp));
#endif
    oldest = &state->send[state->oldest];
    previous = oldest;
    slot = previous->next;
    changes = 0;
    out = encoded_deltas;
    header_length = PP_ntohip(&ip, packet);
#ifdef NONMATCHING
    /* this side refuses an IP header PP_ntohip can't read, one that isn't
     * version 4, and one whose length disagrees with the packet's */
    if (header_length < 0 || ip.version != 4 ||
        ip.length != header_length + (*packet)->length) {
        return -1;
    }
#endif
    if (ip.protocol != TCP_PROTOCOL || ip.fragment_offset != 0 ||
        ip.more_fragments) {
        if (ip.protocol != TCP_PROTOCOL) {
            state->sent_other++;
        } else {
            state->sent_tcp++;
        }
        *packet = PP_htonip(&ip, *packet, 1);
        return VJ_PLAIN_IP;
    }
#ifdef NONMATCHING
    /* this side refuses a TCP header PP_ntohtcp can't read */
    tcp_length = PP_ntohtcp(&tcp, packet);
    if (tcp_length == (Uint16)-1) {
        return -1;
    }
    header_length += tcp_length;
#else
    header_length += PP_ntohtcp(&tcp, packet);
#endif
    if (tcp.flags.syn || tcp.flags.fin || tcp.flags.reset || !tcp.flags.ack) {
        state->sent_tcp++;
        *packet = PP_htontcp(&tcp, *packet, NULL);
        *packet = PP_htonip(&ip, *packet, 1);
        return VJ_PLAIN_IP;
    }

    for (;;) {
        if (ip.source == slot->ip.source &&
            ip.destination == slot->ip.destination &&
            tcp.source == slot->tcp.source &&
            tcp.destination == slot->tcp.destination) {
            goto found;
        }
        if (slot == oldest) {
            break;
        }
        previous = slot;
        slot = slot->next;
        state->search_steps++;
    }
    state->misses++;
    state->oldest = previous->number;
    goto full_header;

found:
    if (previous != oldest) {
        if (slot == oldest) {
            state->oldest = previous->number;
        } else {
            previous->next = slot->next;
            slot->next = oldest->next;
            oldest->next = slot;
        }
    }
    last = &slot->tcp;
    if (ip.version != slot->ip.version ||
        ip.option_length != slot->ip.option_length ||
        ip.service != slot->ip.service ||
        ip.dont_fragment != slot->ip.dont_fragment || ip.ttl != slot->ip.ttl ||
        tcp.option_length != slot->tcp.option_length ||
        (ip.option_length > 0 &&
         memcmp(ip.options, slot->ip.options, ip.option_length) != 0) ||
        (tcp.option_length > 0 &&
         memcmp(tcp.options, slot->tcp.options, tcp.option_length) != 0)) {
        goto full_header;
    }

    if (tcp.flags.urgent) {
        sequence_delta = tcp.urgent_pointer;
        out = encode_delta(out, sequence_delta);
        changes |= NEW_URGENT;
    } else if (tcp.urgent_pointer != last->urgent_pointer) {
        goto full_header;
    }
    if ((sequence_delta = tcp.window - last->window) != 0) {
        out = encode_delta(out, sequence_delta);
        changes |= NEW_WINDOW;
    }
    if ((ack_delta = tcp.acknowledgement - last->acknowledgement) != 0) {
        if (ack_delta > 0xffff) {
            goto full_header;
        }
        out = encode_delta(out, ack_delta);
        changes |= NEW_ACK;
    }
    if ((sequence_delta = tcp.sequence - last->sequence) != 0) {
        if (sequence_delta > 0xffff) {
            goto full_header;
        }
        out = encode_delta(out, sequence_delta);
        changes |= NEW_SEQUENCE;
    }

    switch (changes) {
    case 0:
        /* no field changed. a segment that follows one without data, and
         * differs from it in length, goes compressed. any other may resend
         * one the peer missed, so it goes whole */
        if (ip.length != slot->ip.length && slot->ip.length == header_length) {
            break;
        }
        goto full_header;
    case INTERACTIVE:
    case ONE_WAY:
        /* a change mask of exactly INTERACTIVE or ONE_WAY would read as that
         * special case. such a header goes whole */
        goto full_header;
    case NEW_SEQUENCE | NEW_ACK:
        if (sequence_delta == ack_delta &&
            sequence_delta == (Uint32)(slot->ip.length - header_length)) {
            changes = INTERACTIVE;
            out = encoded_deltas;
        }
        break;
    case NEW_SEQUENCE:
        if (sequence_delta == (Uint32)(slot->ip.length - header_length)) {
            changes = ONE_WAY;
            out = encoded_deltas;
        }
        break;
    }
    sequence_delta = ip.identification - slot->ip.identification;
    if (sequence_delta != 1) {
        out = encode_delta(out, sequence_delta);
        changes |= NEW_ID;
    }
    if (tcp.flags.push) {
        changes |= PUSH;
    }
    /* @hack spelling: ack_delta holding the checksum, and sequence_delta the
     * deltas' length below, where locals of their own would do */
    ack_delta = tcp.checksum;
    slot->ip = ip;
    slot->tcp = tcp;
    sequence_delta = out - encoded_deltas;
    if (!compress_slot || state->current_send != slot->number) {
        compressed = *packet =
            PP_pushdown(*packet, sequence_delta + (COMPRESSED_HEADER_BYTES +
                                                   SLOT_NUMBER_BYTES));
        out = compressed->data;
        *out++ = changes | NEW_SLOT;
        *out++ = slot->number;
        state->current_send = slot->number;
    } else {
        compressed = *packet =
            PP_pushdown(*packet, sequence_delta + COMPRESSED_HEADER_BYTES);
        out = compressed->data;
        *out++ = changes;
    }
    out = PP_put16(out, (Uint16)ack_delta);
    /* @hack type: the length narrowed to a short, where its Uint32 would do */
    memcpy(out, encoded_deltas, (short)sequence_delta);
    state->sent_delta++;
    return VJ_DELTA_HEADER;

full_header:
    /* an uncompressed segment carries its slot's number in IP's protocol
     * field */
    ip.protocol = slot->number;
    slot->ip = ip;
    slot->tcp = tcp;
    state->current_send = slot->number;
    state->sent_full++;
    *packet = PP_htontcp(&tcp, *packet, NULL);
    *packet = PP_htonip(&ip, *packet, 1);
    return VJ_FULL_HEADER;
}

short PP_slhc_uncompress(struct ppp_compression *state,
                         struct ppp_buffer **packet)
{
    short changes;
    Sint32 value;
    struct ppp_tcp *tcp;
    struct connection_slot *slot;
    short length;
#ifdef NONMATCHING
    /* this side refuses a missing state, receive slots or packet. updated is
     * the copy of the slot it changes, and saved the slot itself */
    struct connection_slot updated;
    struct connection_slot *saved;
    if (!state || !state->receive || !packet || !*packet || !(*packet)->data) {
        return 0;
    }
#endif
    state->received_delta++;
    if (PP_len_p(*packet) < COMPRESSED_HEADER_BYTES) {
        state->errors++;
#ifdef NONMATCHING
        /* this side tosses the packet, as the errors at bad do */
        return PP_slhc_toss(state);
#else
        return 0;
#endif
    }
    changes = pull_octet(packet);
    if (changes & NEW_SLOT) {
        value = pull_octet(packet);
        if (value < 0 || value > state->last_receive_slot) {
            goto bad;
        }
        state->flags &= ~DISCARD_IMPLICIT_SLOT;
        state->current_receive = value;
    } else if (state->flags & DISCARD_IMPLICIT_SLOT) {
        state->discarded++;
        return 0;
    }
    if (state->current_receive > state->last_receive_slot) {
        state->discarded++;
        return 0;
    }
    slot = &state->receive[state->current_receive];
#ifdef NONMATCHING
    if (slot->ip.version != 4) {
        goto bad;
    }
    /* the slot changes only once every delta is read and both headers are
     * rebuilt */
    saved = slot;
    updated = *slot;
    slot = &updated;
#endif
    tcp = &slot->tcp;
    value = PPP_PULL16(packet);
    if (value == -1) {
        goto bad;
    }
    tcp->checksum = value;
    tcp->flags.push = (changes & PUSH) ? 1 : 0;
#ifdef NONMATCHING
    /* neither special case carries an urgent pointer */
    tcp->flags.urgent = 0;
#endif
    switch (changes & CHANGE_FIELDS) {
    case INTERACTIVE: {
        short payload = slot->ip.length;
        payload -=
            slot->ip.option_length + PPP_IP_HEADER_BYTES + PPP_TCP_HEADER_BYTES;
#ifdef NONMATCHING
        /* this side takes the TCP options out of the payload too, which the
         * slot's IP length counts */
        payload -= tcp->option_length;
#endif
        tcp->acknowledgement += payload;
        tcp->sequence += payload;
        break;
    }
    case ONE_WAY:
        tcp->sequence +=
            slot->ip.length - (slot->ip.option_length + PPP_IP_HEADER_BYTES +
                               PPP_TCP_HEADER_BYTES);
#ifdef NONMATCHING
        /* this side takes the TCP options out of the payload too, which the
         * slot's IP length counts */
        tcp->sequence -= tcp->option_length;
#endif
        break;
    default:
        if (changes & NEW_URGENT) {
            tcp->flags.urgent = 1;
            if ((value = decode_delta(packet)) == -1) {
                goto bad;
            }
            tcp->urgent_pointer = value;
        } else {
            tcp->flags.urgent = 0;
        }
        if (changes & NEW_WINDOW) {
            if ((value = decode_delta(packet)) == -1) {
                goto bad;
            }
            tcp->window += value;
        }
        if (changes & NEW_ACK) {
            if ((value = decode_delta(packet)) == -1) {
                goto bad;
            }
            tcp->acknowledgement += value;
        }
        if (changes & NEW_SEQUENCE) {
            if ((value = decode_delta(packet)) == -1) {
                goto bad;
            }
            tcp->sequence += value;
        }
        break;
    }
    if (changes & NEW_ID) {
        if ((value = decode_delta(packet)) == -1) {
            goto bad;
        }
        slot->ip.identification += value;
    } else {
        slot->ip.identification++;
    }

    length = PP_len_p(*packet) + PPP_IP_HEADER_BYTES + PPP_TCP_HEADER_BYTES +
             slot->ip.option_length;
#ifdef NONMATCHING
    /* this side counts the TCP options in the length, and drops a length
     * outside 1 to PPP_FRAME_DATA_BYTES */
    length += tcp->option_length;
    if (length <= 0 || length > PPP_FRAME_DATA_BYTES) {
        goto bad;
    }
#endif
    slot->ip.length = length;
    *packet = PP_htontcp(tcp, *packet, NULL);
    *packet = PP_htonip(&slot->ip, *packet, 0);
#ifdef NONMATCHING
    /* this side drops a packet whose headers couldn't be rebuilt, and only then
     * stores the updated slot */
    if (!*packet) {
        goto bad;
    }
    *saved = updated;
#endif
    return length;

bad:
    state->errors++;
    return PP_slhc_toss(state);
}

short PP_slhc_remember(struct ppp_compression *state,
                       struct ppp_buffer **packet)
{
    struct connection_slot *slot;
    struct ppp_ip ip;
    struct ppp_tcp tcp;
#ifdef NONMATCHING
    /* this side refuses a missing state, receive slots or packet. it tosses a
     * packet whose IP or TCP header can't be read, isn't version 4, or
     * disagrees with its length */
    if (!state || !state->receive || !packet || !*packet || !(*packet)->data) {
        return 0;
    }
    memset(&ip, 0, sizeof(ip));
    memset(&tcp, 0, sizeof(tcp));
    if (PP_ntohip(&ip, packet) == (Uint16)-1 || ip.version != 4 ||
        ip.length !=
            PPP_IP_HEADER_BYTES + ip.option_length + (*packet)->length ||
        PP_ntohtcp(&tcp, packet) == (Uint16)-1) {
        state->errors++;
        return PP_slhc_toss(state);
    }
#else
    PP_ntohip(&ip, packet);
    PP_ntohtcp(&tcp, packet);
#endif
    if (ip.protocol > state->last_receive_slot) {
        state->errors++;
        return PP_slhc_toss(state);
    }
    slot = &state->receive[state->current_receive = ip.protocol];
    state->flags &= ~DISCARD_IMPLICIT_SLOT;
    ip.protocol = TCP_PROTOCOL;
    slot->ip = ip;
    slot->tcp = tcp;
    *packet = PP_htontcp(&tcp, *packet, NULL);
    *packet = PP_htonip(&ip, *packet, 1);
    state->received_full++;
    return PP_len_p(*packet);
}

short PP_slhc_toss(struct ppp_compression *state)
{
    if (state != NULL) {
        state->flags |= DISCARD_IMPLICIT_SLOT;
    }
    return 0;
}

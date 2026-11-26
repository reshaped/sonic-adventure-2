#include <sg_xpt.h>
#include <string.h>
#include "ppp_ip.h"
#include "ppp_packet.h"

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RESET 0x04
#define TCP_FLAG_PUSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URGENT 0x20
#define TCP_OPTION_END 0
#define TCP_OPTION_NOP 1
#define TCP_OPTION_MSS 2
#define TCP_MSS_BYTES 4
#define TCP_ERROR ((Uint16) - 1)
/* offset holds the header's length in 32-bit words in its high four bits.
 * TCP_OFFSET makes it from a length in bytes, and TCP_LENGTH reads one back */
#define TCP_OFFSET(bytes) ((bytes) << 2)
#define TCP_LENGTH(offset) (((offset) & 0xf0) >> 2)

struct tcp_wire {
    Uint16 source, destination;
    Uint32 sequence, acknowledgement;
    Uint8 offset, flags;
    Uint16 window, checksum, urgent_pointer;
    Uint8 options[PPP_OPTION_BYTES];
};

struct ppp_buffer *PP_htontcp(const struct ppp_tcp *tcp,
                              struct ppp_buffer *buffer,
                              const struct ppp_pseudo_header *pseudo)
{
    Uint16 length = 0;
    struct ppp_buffer *packet;
    Uint8 *data;
    Uint8 flags;
    struct tcp_wire header;
    struct tcp_wire *wire;

    if (tcp->option_length > 0 && tcp->option_length <= PPP_OPTION_BYTES) {
        length = tcp->option_length;
    } else if (tcp->maximum_segment) {
        length = TCP_MSS_BYTES;
    }
#ifdef NONMATCHING
    /* this side refuses options that don't fill whole words, and an
     * option_length past PPP_OPTION_BYTES */
    if ((length & 3) || tcp->option_length > PPP_OPTION_BYTES) {
        return NULL;
    }
#endif
    length += PPP_TCP_HEADER_BYTES;
    packet = PP_pushdown(buffer, length);
    if (packet == NULL) {
        return NULL;
    }
    /* @hack spelling: the header written through wire, where header itself
     * would do */
    wire = &header;
    data = packet->data;
    wire->source = Htons(tcp->source);
    wire->destination = Htons(tcp->destination);
    wire->sequence = Htonl(tcp->sequence);
    wire->acknowledgement = Htonl(tcp->acknowledgement);
    wire->offset = TCP_OFFSET(length);
    flags = 0;
    if (tcp->flags.urgent) {
        flags |= TCP_FLAG_URGENT;
    }
    if (tcp->flags.ack) {
        flags |= TCP_FLAG_ACK;
    }
    if (tcp->flags.push) {
        flags |= TCP_FLAG_PUSH;
    }
    if (tcp->flags.reset) {
        flags |= TCP_FLAG_RESET;
    }
    if (tcp->flags.syn) {
        flags |= TCP_FLAG_SYN;
    }
    if (tcp->flags.fin) {
        flags |= TCP_FLAG_FIN;
    }
    wire->flags = flags;
    wire->window = Htons(tcp->window);
    if (pseudo == NULL) {
        wire->checksum = Htons(tcp->checksum);
    } else {
        wire->checksum = 0;
    }
    wire->urgent_pointer = Htons(tcp->urgent_pointer);
    if (length > PPP_TCP_HEADER_BYTES) {
        if (tcp->maximum_segment) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: the bytes past the MSS
             * are TCP_OPTION_END */
            memset(wire->options, TCP_OPTION_END,
                   length - PPP_TCP_HEADER_BYTES);
#endif
            /* @bug uninitialized: where tcp holds other options too, the
             * header sent carries the MSS and then bytes nothing wrote
             * (0x0c9ef908, 0x0c9ef940) */
            wire->options[0] = TCP_OPTION_MSS;
            wire->options[1] = TCP_MSS_BYTES;
            wire->options[2] = tcp->maximum_segment >> 8;
            wire->options[3] = tcp->maximum_segment;
        } else {
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: it copies the options to
             * the start of the header's */
            memcpy(wire->options, tcp->options, tcp->option_length);
#else
            /* @bug memory: copies the options two bytes in. the header sent
             * starts them with two bytes nothing wrote and drops their last
             * two, and a list of 39 or 40 bytes runs one or two past header
             * (0x0c9ef936, 0x0c9ef940) */
            memcpy(wire->options + 2, tcp->options, tcp->option_length);
#endif
        }
    }
    memcpy(data, wire, length);
    if (pseudo) {
        wire->checksum = Htons(PP_cksum(pseudo, packet, pseudo->length));
    }
    memcpy(data, wire, length);
    return packet;
}

Uint16 PP_ntohtcp(struct ppp_tcp *tcp, struct ppp_buffer **buffer)
{
    Uint16 remaining;
    Uint16 length;
    Uint16 kind;
    Uint16 option_length;
    int flags;
    Uint8 *option;
#ifdef NONMATCHING
    /* this side reads the header into the whole struct, the plain form the hack
     * tag below gives */
    struct tcp_wire header;
    struct tcp_wire *wire = &header;
#else
    /* @hack type: a header of PPP_TCP_HEADER_BYTES read through struct
     * tcp_wire, where the struct, options and all, would do */
    Uint8 header[PPP_TCP_HEADER_BYTES];
    struct tcp_wire *wire = (struct tcp_wire *)header;
#endif

    remaining = PP_pullup(buffer, (Uint8 *)&header, PPP_TCP_HEADER_BYTES);
#ifdef NONMATCHING
    /* this side fails a header shorter than PPP_TCP_HEADER_BYTES */
    if (remaining < PPP_TCP_HEADER_BYTES) {
        return TCP_ERROR;
    }
#endif
    tcp->source = Htons(wire->source);
    tcp->destination = Htons(wire->destination);
    tcp->sequence = Htonl(wire->sequence);
    tcp->acknowledgement = Htonl(wire->acknowledgement);
    length = TCP_LENGTH(wire->offset);
    flags = wire->flags;
    tcp->flags.urgent = flags & TCP_FLAG_URGENT;
    tcp->flags.ack = flags & TCP_FLAG_ACK;
    tcp->flags.push = flags & TCP_FLAG_PUSH;
    tcp->flags.reset = flags & TCP_FLAG_RESET;
    tcp->flags.syn = flags & TCP_FLAG_SYN;
    tcp->flags.fin = flags & TCP_FLAG_FIN;
    tcp->window = Htons(wire->window);
    tcp->checksum = Htons(wire->checksum);
    tcp->urgent_pointer = Htons(wire->urgent_pointer);
    tcp->maximum_segment = 0;
    tcp->option_length = length - PPP_TCP_HEADER_BYTES;

    if (remaining < PPP_TCP_HEADER_BYTES || length < PPP_TCP_HEADER_BYTES) {
        return TCP_ERROR;
    }
    if (tcp->option_length == 0) {
        return length;
    }
    if (tcp->option_length > PP_len_p(*buffer)) {
        return TCP_ERROR;
    }
    PP_pullup(buffer, tcp->options, tcp->option_length);
    for (option = tcp->options, remaining = tcp->option_length; remaining > 0;
         remaining -= option_length, option += option_length) {
        kind = option[0];
        if (kind == TCP_OPTION_END) {
            break;
        }
        if (kind == TCP_OPTION_NOP) {
            option_length = 1;
        } else {
#ifdef NONMATCHING
            /* this side fails an option whose length byte lies past the
             * header */
            if (remaining < 2) {
                return TCP_ERROR;
            }
#endif
            option_length = option[1];
#ifdef NONMATCHING
            /* this side fails an option shorter than its kind and length bytes,
             * or longer than the header holds */
            if (option_length < 2 || option_length > remaining) {
                return TCP_ERROR;
            }
#else
            if (option_length <= 0) {
                break;
            }
#endif
        }
        if (kind == TCP_OPTION_MSS && option_length == TCP_MSS_BYTES) {
            tcp->maximum_segment = (option[2] << 8) | option[3];
            /* @hack redundant: a continue at the end of the loop's body */
            continue;
        }
    }
    return length;
}

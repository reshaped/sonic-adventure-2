/* the NONMATCHING side's reading and writing of LCP's and IPCP's options,
 * which ppp_lcp.c and ppp_ipcp.c each compile. an option is a type byte, a
 * length byte and its value. the length counts the whole option */
#ifndef NETWORK_PPP_OPTIONS_IMPL_H
#define NETWORK_PPP_OPTIONS_IMPL_H
#include <sg_xpt.h>
#include <string.h>
#include "ppp_packet.h"

struct ppp_option {
    const Uint8 *bytes;
    Uint8 type, length;
};

/* take the next option off packet. the result is 1 with option set and
 * packet past the option, or 0 at packet's end. it is -1 where the option's
 * length is missing, under 2 or past packet's end. 0 and -1 leave packet as
 * it was */
static int next_option(struct ppp_buffer *packet, struct ppp_option *option)
{
    Uint8 length;
    if (packet->length == 0) {
        return 0;
    }
    if (packet->length < 2) {
        return -1;
    }
    length = packet->data[1];
    if (length < 2 || length > packet->length) {
        return -1;
    }
    option->type = packet->data[0];
    option->length = length;
    option->bytes = packet->data;
    packet->data += length;
    packet->length -= length;
    return 1;
}

/* a work frame for the reply to input, which the reply mustn't overlap. it is
 * frame 0, or frame 1 where input lies in frame 0's data */
static struct ppp_buffer *option_workspace(const struct ppp_buffer *input)
{
    struct ppp_buffer *output = PP_alloc_mbuf_work(0);
    if (output && (unsigned long)input->data - (unsigned long)output->data <
                      PPP_FRAME_DATA_BYTES) {
        output = PP_alloc_mbuf_work(1);
    }
    return output;
}

/* copy length bytes of data to the end of packet, a work frame. the result
 * is 0, or -1 where they would overflow its PPP_FRAME_DATA_BYTES */
static int append_option(struct ppp_buffer *packet, const Uint8 *data,
                         unsigned int length)
{
    if (length > PPP_FRAME_DATA_BYTES ||
        packet->length > PPP_FRAME_DATA_BYTES - length) {
        return -1;
    }
    memcpy(packet->data + packet->length, data, length);
    packet->length += length;
    return 0;
}
#endif

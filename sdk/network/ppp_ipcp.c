#include <sg_xpt.h>

#ifdef NONMATCHING
#else
/* above the includes, so that SHC places it ahead of cont_ipcp_s, which
 * ppp_link.h declares */
static Uint8 pull_word[4], pull_byte;
#endif

#include "ppp_framing.h"
#include "ppp_fsm.h"
#include "ppp_ipcp.h"
#include "ppp_link.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_system.h"
#ifdef NONMATCHING
/* the option reader and writer this side's IPCP code below takes from
 * ppp_options_impl.h, and its memcmp */
#include <string.h>
#include "ppp_options_impl.h"
#else
static ppp_event ipcp_release, ipcp_reset, ipcp_starting, ipcp_opening,
    ipcp_closing, ipcp_stopping;
static ppp_check ipcp_request, ipcp_ack, ipcp_nak, ipcp_reject;
static struct ppp_buffer *ipcp_make_request(struct ppp_fsm *fsm);
enum { IPCP_OPTION_HEADER_BYTES = 2, IPCP_OPTION_LIMIT = 32 };
#endif

/* the address the PP_doipcp_ calls take for none */
#define NO_ADDRESS (-1)

#pragma section ATA
static char ipcp_name[5];
struct ppp_protocol ipcp_protocol = {
#include "network/ppp_ipcp/ipcp_protocol.inc"
};
static char ipcp_name[5] = {
#include "network/ppp_ipcp/ipcp_name.inc"
};
#pragma section
struct ipcp_options ipcp_default_options = {
#include "network/ppp_ipcp/ipcp_default_options.inc"
};
Sint16 ipcp_default_negotiation = {
#include "network/ppp_ipcp/ipcp_default_negotiation.inc"
};
Uint8 ipcp_option_lengths[IPCP_OPTION_BITS] = {
#include "network/ppp_ipcp/ipcp_option_lengths.inc"
};
struct ppp_ipcp cont_ipcp_s;

#ifdef NONMATCHING
/* this side's emitter writes only the types it knows, each at its own length.
 * *packet becomes NULL for any other type, or where the option would pass the
 * frame's room */
void ipcp_emit_option(struct ppp_buffer **packet,
                      const struct ipcp_options *settings, Uint8 type,
                      Uint8 length, struct ppp_buffer **parameters)
{
    struct ppp_buffer *output;
    Uint8 *bytes;
    if (!packet) {
        return;
    }
    if (!settings) {
        *packet = NULL;
        return;
    }
    output = *packet;
    if (!output) {
        output = PP_alloc_mbuf_work(0);
        if (!output) {
            return;
        }
        *packet = output;
    }
    if (!output->data || output->length > PPP_FRAME_DATA_BYTES - length ||
        length != (type == IPCP_ADDRESSES ? IPCP_ADDRESSES_LENGTH
                                          : IPCP_OPTION_LENGTH)) {
        *packet = NULL;
        return;
    }
    bytes = output->data + output->length;
    switch (type) {
    case IPCP_ADDRESSES:
        PP_put32(bytes + 2, settings->address);
        PP_put32(bytes + 6, settings->peer_address);
        break;
    case IPCP_COMPRESS:
        PP_put16(bytes + 2, settings->compression);
        if (settings->compression == PPP_VJ_COMPRESSED) {
            bytes[4] = settings->slots - 1;
            bytes[5] = settings->slot_compression;
        } else {
            bytes[4] = 0xff;
            bytes[5] = 0xff;
            if (parameters) {
                PP_pullup(parameters, bytes + 4, 2);
            }
        }
        break;
    case IPCP_ADDRESS:
        PP_put32(bytes + 2, settings->address);
        break;
    case IPCP_DNS1:
        PP_put32(bytes + 2, settings->dns1);
        break;
    case IPCP_DNS2:
        PP_put32(bytes + 2, settings->dns2);
        break;
    default:
        *packet = NULL;
        return;
    }
    bytes[0] = type;
    bytes[1] = length;
    output->length += length;
}

void ipcp_write_options(struct ppp_buffer **packet,
                        const struct ipcp_options *settings, Sint16 mask)
{
    int index;
    if (!*packet) {
        *packet = PP_alloc_mbuf_work(0);
        if (!*packet) {
            return;
        }
    }
    for (index = 1; index < (int)sizeof(ipcp_option_lengths); index++) {
        Uint8 length = ipcp_option_lengths[index];
        if (length && (mask & (1U << index))) {
            ipcp_emit_option(packet, settings,
                             index < IPCP_OWN_TYPES
                                 ? index
                                 : index + (IPCP_HIGH_TYPES - IPCP_OWN_TYPES),
                             length, NULL);
            if (!*packet) {
                return;
            }
        }
    }
}
#else
/* append the option type, length bytes long, to *packet. where *packet is
 * NULL, it takes work frame 0. the types this knows take their values from
 * settings. the rest of length's bytes come from parameters, which may be
 * NULL. a byte parameters can't give is 0xff */
static void ipcp_emit_option(struct ppp_buffer **packet,
                             const struct ipcp_options *settings, Uint8 type,
                             Uint8 length, struct ppp_buffer **parameters)
{
    struct ppp_buffer *output;
    Uint8 *bytes;
    short remaining = length - IPCP_OPTION_HEADER_BYTES;

    if (*packet != NULL) {
        output = *packet;
    } else {
        output = PP_alloc_mbuf_work(0);
        if (output == NULL) {
            return;
        }
        *packet = output;
    }
    bytes = output->data + output->length;
    *bytes++ = type;
    *bytes++ = length;

    switch (type) {
    case IPCP_ADDRESSES:
        bytes = PP_put32(bytes, settings->address);
        bytes = PP_put32(bytes, settings->peer_address);
        remaining -= 8;
        break;
    case IPCP_COMPRESS:
        bytes = PP_put16(bytes, settings->compression);
        remaining -= 2;
        if (settings->compression == PPP_VJ_COMPRESSED) {
            *bytes++ = settings->slots - 1;
            *bytes++ = settings->slot_compression;
            remaining -= 2;
        }
        break;
    case IPCP_ADDRESS:
        bytes = PP_put32(bytes, settings->address);
        remaining -= 4;
        break;
    case IPCP_DNS1:
        bytes = PP_put32(bytes, settings->dns1);
        remaining -= 4;
        break;
    case IPCP_DNS2:
        bytes = PP_put32(bytes, settings->dns2);
        remaining -= 4;
        break;
    default:
        break;
    }
    while (remaining-- > 0) {
        *bytes++ = PPP_PULL_BYTE(parameters);
    }
    output->length += length;
}

static void ipcp_write_options(struct ppp_buffer **packet,
                               const struct ipcp_options *settings, short mask)
{
    short index;

    if (*packet == NULL) {
        *packet = PP_alloc_mbuf_work(0);
        if (*packet == NULL) {
            return;
        }
    }
    for (index = 1; index < (short)IPCP_OPTION_LIMIT; index++) {
        if (mask & (1 << index)) {
            /* @hack redundant: the arm that gives type 0 from
             * IPCP_OPTION_LIMIT on, which the loop never reaches */
            /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
            ipcp_emit_option(packet, settings,
                             index < IPCP_OWN_TYPES ? index
                             : index < IPCP_OPTION_LIMIT
                                 ? index + (IPCP_HIGH_TYPES - IPCP_OWN_TYPES)
                                 : 0,
                             ipcp_option_lengths[index], 0);
            /* NOLINTEND(readability-avoid-nested-conditional-operator) */
        }
    }
}
#endif
struct ppp_buffer *ipcp_make_request(struct ppp_fsm *fsm)
{
    struct ppp_ipcp *ipcp = fsm->context;
    /* @hack redundant: packet's NULL, which the allocation overwrites */
    struct ppp_buffer *packet = NULL;
    packet = PP_alloc_mbuf_work(0);
    if (!packet) {
        return NULL;
    }
    ipcp_write_options(&packet, &ipcp->local.work, ipcp->local.work.negotiate);
    return packet;
}

#ifdef NONMATCHING
/* this side reads each option within the packet's bounds, and changes the IPCP
 * state only once a whole packet checks. it repairs the three bugs tagged on
 * the matching side: the secondary server's test reads dns2, a nak uses up a
 * retry only once it is sent, and a reject carries the peer's own options */
static int ipcp_option_index(Uint8 type)
{
    switch (type) {
    case IPCP_ADDRESSES:
    case IPCP_COMPRESS:
    case IPCP_ADDRESS:
        return type;
    case IPCP_DNS1:
    case IPCP_DNS2:
        return type - (IPCP_HIGH_TYPES - IPCP_OWN_TYPES);
    default:
        return 0;
    }
}

static int ipcp_write_option(struct ppp_buffer *packet, Uint8 type,
                             const struct ipcp_options *settings)
{
    int index = ipcp_option_index(type);
    ipcp_emit_option(&packet, settings, type, ipcp_option_lengths[index], NULL);
    return packet ? 0 : -1;
}

static int ipcp_address_pair(struct ppp_ipcp *ipcp, struct ipcp_side *side,
                             const Uint8 *value, int request)
{
    int result = PPP_CONFIG_ACK;
    side->work.address = PP_get32(value);
    side->work.peer_address = PP_get32(value + 4);
    if (!request) {
        if (ipcp->remote.want.address) {
            side->work.peer_address = ipcp->remote.want.address;
        }
        if (side->want.address) {
            side->work.address = side->want.address;
        }
        return result;
    }
    if (!side->work.address && !side->want.address) {
        result = PPP_CONFIG_REJ;
    } else if (!side->want.address) {
        ipcp->local.work.peer_address = side->work.address;
    } else if (side->work.address != side->want.address) {
        side->work.address = side->want.address;
        result = PPP_CONFIG_NAK;
    }
    if (!side->work.peer_address && !ipcp->local.want.address) {
        return PPP_CONFIG_REJ;
    }
    if (!ipcp->local.want.address) {
        ipcp->local.work.address = side->work.peer_address;
    } else if (side->work.peer_address != ipcp->local.want.address) {
        side->work.peer_address = ipcp->local.want.address;
        if (result != PPP_CONFIG_REJ) {
            result = PPP_CONFIG_NAK;
        }
    }
    return result;
}

static int ipcp_check_option(struct ppp_ipcp *ipcp, struct ipcp_side *side,
                             const struct ppp_option *option, int request)
{
    int index = ipcp_option_index(option->type);
    const Uint8 *value = option->bytes + 2;
    Sint32 *work;
    Sint32 *want;
    Sint32 *peer;
    int result = PPP_CONFIG_ACK;
    if (!index || !(side->will_negotiate & (1U << index))) {
        return PPP_CONFIG_REJ;
    }
    if (option->type == IPCP_COMPRESS) {
        if (option->length < 4) {
            return -1;
        }
        if (PP_get16(value) != PPP_VJ_COMPRESSED) {
            side->work.compression = PPP_VJ_COMPRESSED;
            side->work.slots = (side->want.negotiate & IPCP_COMPRESS_BIT)
                                   ? side->want.slots
                                   : VJ_SLOTS;
            side->work.slot_compression =
                (side->want.negotiate & IPCP_COMPRESS_BIT)
                    ? side->want.slot_compression
                    : 1;
            return PPP_CONFIG_NAK;
        }
        if (option->length != 6) {
            return -1;
        }
        side->work.compression = PPP_VJ_COMPRESSED;
        side->work.slots = value[2] + 1;
        side->work.slot_compression = value[3];
        if (side->work.slots > VJ_SLOTS) {
            side->work.slots = VJ_SLOTS;
            result = PPP_CONFIG_NAK;
        }
        if (side->work.slot_compression > 1) {
            side->work.slot_compression = 1;
            result = PPP_CONFIG_NAK;
        }
        return result;
    }
    if (option->length != ipcp_option_lengths[index]) {
        return -1;
    }
    switch (option->type) {
    case IPCP_ADDRESSES:
        return ipcp_address_pair(ipcp, side, value, request);
    case IPCP_ADDRESS:
        work = &side->work.address;
        want = &side->want.address;
        peer = request ? &ipcp->local.want.peer_address
                       : &ipcp->remote.want.peer_address;
        break;
    case IPCP_DNS1:
        work = &side->work.dns1;
        want = &side->want.dns1;
        peer = request ? &ipcp->local.want.peer_dns1
                       : &ipcp->remote.want.peer_dns1;
        break;
    case IPCP_DNS2:
        work = &side->work.dns2;
        want = &side->want.dns2;
        peer = request ? &ipcp->local.want.peer_dns2
                       : &ipcp->remote.want.peer_dns2;
        break;
    default:
        return PPP_CONFIG_REJ;
    }
    *work = PP_get32(value);
    if (request && !*work) {
        *work = *want;
        return PPP_CONFIG_NAK;
    }
    *peer = *want = *work;
    return PPP_CONFIG_ACK;
}

short ipcp_request(struct ppp_fsm *fsm, struct ppp_control_header *header,
                   struct ppp_buffer *packet)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ppp_ipcp state;
    struct ppp_option option;
    struct ppp_buffer input;
    struct ppp_buffer *reply;
    int reply_code = PPP_CONFIG_ACK;
    int status;
    int code;
    input = *packet;
    reply = option_workspace(&input);
    if (!reply || input.length != header->length) {
        return -1;
    }
    state = *ipcp;
    state.remote.work.negotiate = 0;
    while ((status = next_option(&input, &option)) > 0) {
        code = ipcp_check_option(&state, &state.remote, &option, 1);
        if (code < 0) {
            return -1;
        }
        if (code == PPP_CONFIG_NAK && !fsm->nak_retries) {
            code = PPP_CONFIG_REJ;
        }
        if (code < reply_code) {
            continue;
        }
        if (code > reply_code) {
            reply->length = 0;
            reply_code = code;
        }
        if (code != PPP_CONFIG_REJ) {
            state.remote.work.negotiate |= 1U << ipcp_option_index(option.type);
        }
        if (code == PPP_CONFIG_NAK) {
            if (ipcp_write_option(reply, option.type, &state.remote.work) < 0) {
                return -1;
            }
        } else if (append_option(reply, option.bytes, option.length) < 0) {
            return -1;
        }
    }
    if (status < 0) {
        return -1;
    }
    if (reply_code == PPP_CONFIG_NAK) {
        ipcp_write_options(&reply, &state.remote.work,
                           state.remote.want.negotiate &
                               ~state.remote.work.negotiate);
        if (!reply) {
            return -1;
        }
    }
    if (PP_fsm_send(fsm, reply_code, header->id, reply) < 0) {
        return -1;
    }
    if (reply_code == PPP_CONFIG_NAK) {
        fsm->nak_retries--;
    }
    *ipcp = state;
    return reply_code != PPP_CONFIG_ACK;
}
short ipcp_ack(struct ppp_fsm *fsm, struct ppp_control_header *header,
               struct ppp_buffer *packet)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ppp_buffer input;
    struct ppp_buffer *request;
    if (header->id != fsm->last_id || header->length != packet->length) {
        return -1;
    }
    input = *packet;
    request = option_workspace(&input);
    if (!request) {
        return -1;
    }
    ipcp_write_options(&request, &ipcp->local.work, ipcp->local.work.negotiate);
    if (!request || request->length != input.length ||
        memcmp(request->data, input.data, input.length)) {
        return -1;
    }
    fsm->ack_count++;
    return 0;
}
short ipcp_nak(struct ppp_fsm *fsm, struct ppp_control_header *header,
               struct ppp_buffer *packet)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ppp_ipcp state;
    struct ppp_buffer input;
    struct ppp_option option;
    int status;
    int index;
    if (header->id != fsm->last_id || header->length != packet->length) {
        return -1;
    }
    state = *ipcp;
    input = *packet;
    while ((status = next_option(&input, &option)) > 0) {
        index = ipcp_option_index(option.type);
        if (index) {
            int code = ipcp_check_option(&state, &state.local, &option, 0);
            if (code < 0) {
                return -1;
            }
            if (code == PPP_CONFIG_REJ) {
                state.local.work.negotiate &= ~(1U << index);
            } else {
                state.local.work.negotiate |= 1U << index;
            }
        }
    }
    if (status < 0) {
        return -1;
    }
    *ipcp = state;
    return 0;
}
short ipcp_reject(struct ppp_fsm *fsm, struct ppp_control_header *header,
                  struct ppp_buffer *packet)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ppp_buffer input;
    struct ppp_option option;
    Uint16 negotiate = ipcp->local.work.negotiate;
    int index;
    int status;
    if (header->id != fsm->last_id || header->length != packet->length) {
        return -1;
    }
    input = *packet;
    while ((status = next_option(&input, &option)) > 0) {
        index = ipcp_option_index(option.type);
        if (!index || !(negotiate & (1U << index))) {
            return -1;
        }
        negotiate &= ~(1U << index);
    }
    if (status < 0) {
        return -1;
    }
    ipcp->local.work.negotiate = negotiate;
    return 0;
}
#else
/* check option, whose value is at *packet, reading its values into
 * side->work. the result is PPP_CONFIG_ACK, PPP_CONFIG_NAK with side->work
 * holding the values to ask for, or PPP_CONFIG_REJ. it is -1 where the length
 * is shorter than the values or the bytes run out. request is 1 for a peer's
 * request. the option's bytes past its values then stay at *packet, for
 * ipcp_emit_option to copy. request is 0 for a nak, and the check skips those
 * bytes */
static short ipcp_check(struct ppp_buffer **packet, struct ppp_ipcp *ipcp,
                        struct ipcp_side *side,
                        struct ppp_option_header *option, short request)
{
    short remaining = option->length - IPCP_OPTION_HEADER_BYTES;
    int option_result = PPP_CONFIG_ACK;
    short value;

    switch (option->type) {
    case IPCP_ADDRESSES:
        side->work.address = PPP_PULL32(packet);
        side->work.peer_address = PPP_PULL32(packet);
        remaining -= 8;
        if (!request) {
            if (ipcp->remote.want.address != 0) {
                ipcp->local.work.peer_address = ipcp->remote.want.address;
            }
            if (ipcp->local.want.address != 0) {
                ipcp->local.work.address = ipcp->local.want.address;
            }
            break;
        }
        if (ipcp->remote.work.address == ipcp->remote.want.address) {
            if (ipcp->remote.want.address == 0) {
                option_result = PPP_CONFIG_REJ;
            }
        } else if (ipcp->remote.want.address == 0) {
            ipcp->local.work.peer_address = ipcp->remote.work.address;
        } else {
            ipcp->remote.work.address = ipcp->remote.want.address;
            option_result = PPP_CONFIG_NAK;
        }
        if (ipcp->remote.work.peer_address == ipcp->local.want.address) {
            if (ipcp->local.want.address == 0) {
                option_result = PPP_CONFIG_REJ;
            }
        } else if (ipcp->local.want.address == 0) {
            ipcp->local.work.address = ipcp->remote.work.peer_address;
        } else {
            option_result = PPP_CONFIG_NAK;
            ipcp->remote.work.peer_address = ipcp->local.want.address;
        }
        break;
    case IPCP_COMPRESS:
        side->work.compression = PPP_PULL16(packet);
        remaining -= 2;
        switch (side->work.compression) {
        case PPP_VJ_COMPRESSED:
            value = PPP_PULL_BYTE(packet);
            if (value == -1) {
                return -1;
            }
            if ((side->work.slots = value + 1) < 1) {
                side->work.slots = 1;
                option_result = PPP_CONFIG_NAK;
            } else if (side->work.slots > VJ_SLOTS) {
                side->work.slots = VJ_SLOTS;
                option_result = PPP_CONFIG_NAK;
            }
            value = PPP_PULL_BYTE(packet);
            if (value == -1) {
                return -1;
            }
            if ((side->work.slot_compression = value) > 1) {
                side->work.slot_compression = 1;
                option_result = PPP_CONFIG_NAK;
            }
            remaining -= 2;
            break;
        default:
            if (side->want.negotiate & IPCP_COMPRESS_BIT) {
                side->work.compression = side->want.compression;
                side->work.slots = side->want.slots;
                side->work.slot_compression = side->want.slot_compression;
            } else {
                side->work.compression = PPP_VJ_COMPRESSED;
                side->work.slots = VJ_SLOTS;
                side->work.slot_compression = 1;
            }
            option_result = PPP_CONFIG_NAK;
            break;
        }
        break;
    case IPCP_ADDRESS:
        side->work.address = PPP_PULL32(packet);
        remaining -= 4;
        if (!request) {
            ipcp->local.want.address = side->work.address;
            ipcp->remote.want.peer_address = side->work.address;
            break;
        }
        if (side->work.address != 0) {
            ipcp->remote.want.address = side->work.address;
            ipcp->local.want.peer_address = side->work.address;
            break;
        }
        ipcp->remote.work.address = ipcp->remote.want.address;
        option_result = PPP_CONFIG_NAK;
        break;
    case IPCP_DNS1:
        side->work.dns1 = PPP_PULL32(packet);
        remaining -= 4;
        if (!request) {
            ipcp->local.want.dns1 = side->work.dns1;
            ipcp->remote.want.peer_dns1 = side->work.dns1;
            break;
        }
        if (side->work.dns1 != 0) {
            ipcp->remote.want.dns1 = side->work.dns1;
            ipcp->local.want.peer_dns1 = side->work.dns1;
            break;
        }
        ipcp->remote.work.dns1 = ipcp->remote.want.dns1;
        option_result = PPP_CONFIG_NAK;
        break;
    case IPCP_DNS2:
        side->work.dns2 = PPP_PULL32(packet);
        remaining -= 4;
        if (!request) {
            ipcp->local.want.dns2 = side->work.dns2;
            ipcp->remote.want.peer_dns2 = side->work.dns2;
            break;
        }
        /* @bug logic: takes a secondary server of 0 where the primary isn't
         * 0, and naks any secondary where the primary is 0. the test reads
         * dns1 where dns2 is meant (0x0c9e563c) */
        if (side->work.dns1 != 0) {
            ipcp->remote.want.dns2 = side->work.dns2;
            ipcp->local.want.peer_dns2 = side->work.dns2;
            break;
        }
        ipcp->remote.work.dns2 = ipcp->remote.want.dns2;
        option_result = PPP_CONFIG_NAK;
        break;
    default:
        option_result = PPP_CONFIG_REJ;
        break;
    }

    if (!(side->will_negotiate & IPCP_MASK(option->type))) {
        option_result = PPP_CONFIG_REJ;
    }
    if (remaining < 0) {
        return -1;
    }
    if (!request && remaining > 0) {
        while (remaining-- > 0) {
            if (PPP_PULL_BYTE(packet) == -1) {
                return -1;
            }
        }
    }
    return option_result;
}

short ipcp_request(struct ppp_fsm *fsm, struct ppp_control_header *header,
                   struct ppp_buffer *data)
{
    struct ppp_ipcp *ipcp = fsm->context;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_buffer *reply = NULL;
    short reply_result = PPP_CONFIG_ACK;
    short suggested;
    struct ppp_option_header option;
    short option_result;

    ipcp->remote.work.negotiate = 0;
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        if ((left -= option.length) < 0) {
            return -1;
        }
        option_result = ipcp_check(&data, ipcp, &ipcp->remote, &option, 1);
        if (option_result == -1) {
            return -1;
        }
        /* a reply holds only the options of its worst result, so a worse
         * result starts it over */
        if (option_result < reply_result) {
            continue;
        } else if (option_result > reply_result) {
            reply = NULL;
            reply_result = option_result;
        }
        if (option_result != PPP_CONFIG_REJ) {
            ipcp->remote.work.negotiate |= IPCP_MASK(option.type);
        }
        ipcp_emit_option(&reply, &ipcp->remote.work, option.type, option.length,
                         &data);
    }

    suggested = ipcp->remote.want.negotiate & ~ipcp->remote.work.negotiate;
    if (reply_result == PPP_CONFIG_NAK) {
        if (fsm->nak_retries > 0) {
            ipcp_write_options(&reply, &ipcp->remote.work, suggested);
            /* @bug logic: takes one of nak_retries though the send fails
             * (0x0c9e57e8, 0x0c9e57fc) */
            fsm->nak_retries--;
        } else {
            /* @bug logic: rejects with this end's values (0x0c9e57ee) */
            reply_result = PPP_CONFIG_REJ;
        }
    }
    PP_fsm_send(fsm, reply_result, header->id, reply);
    return reply_result != PPP_CONFIG_ACK;
}

short ipcp_ack(struct ppp_fsm *fsm, struct ppp_control_header *header,
               struct ppp_buffer *data)
{
    struct ppp_buffer *request_packet;
    short error = 0;

    if (header->id != fsm->last_id) {
        return -1;
    }
    request_packet = ipcp_make_request(fsm);
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    if ((short)header->length != PP_len_p(request_packet)) {
        error = 1;
    } else {
        short requested_byte;
        short acknowledged_byte;

        while ((requested_byte = PPP_PULL_BYTE(&request_packet)) != -1) {
            acknowledged_byte = PPP_PULL_BYTE(&data);
            if (acknowledged_byte == -1 ||
                acknowledged_byte != requested_byte) {
                error = 1;
                break;
            }
        }
    }
    if (error) {
        return -1;
    }
    fsm->ack_count++;
    return 0;
}

short ipcp_nak(struct ppp_fsm *fsm, struct ppp_control_header *header,
               struct ppp_buffer *data)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ipcp_side *local = &ipcp->local;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_option_header option;
    short result;

    if (header->id != fsm->last_id) {
        return -1;
    }
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        if ((left -= option.length) < 0) {
            return -1;
        }
        /* @hack redundant: the test of the bit, which the |= sets either way */
        if (!(local->work.negotiate & IPCP_MASK(option.type))) {
            local->work.negotiate |= IPCP_MASK(option.type);
        }
        if ((result = ipcp_check(&data, ipcp, local, &option, 0)) == -1) {
            return -1;
        }

        if (result == PPP_CONFIG_REJ) {
            local->work.negotiate &= ~IPCP_MASK(option.type);
        }
    }
    return 0;
}

short ipcp_reject(struct ppp_fsm *fsm, struct ppp_control_header *header,
                  struct ppp_buffer *data)
{
    struct ppp_ipcp *ipcp = fsm->context;
    struct ipcp_side *local = &ipcp->local;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_option_header option;

    if (header->id != fsm->last_id) {
        return -1;
    }
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        short remaining;

        if ((left -= option.length) < 0) {
            return -1;
        }
        if (!(local->work.negotiate & IPCP_MASK(option.type))) {
            return -1;
        }
        for (remaining = option.length - IPCP_OPTION_HEADER_BYTES;
             remaining-- > 0;) {
            if (PPP_PULL_BYTE(&data) == -1) {
                return -1;
            }
        }
        local->work.negotiate &= ~IPCP_MASK(option.type);
    }
    return 0;
}
#endif
void ipcp_reset(struct ppp_fsm *fsm)
{
    struct ppp_ipcp *ipcp = fsm->context;
    ipcp->local.work = ipcp->local.want;
    ipcp->local.work.peer_address = ipcp->remote.want.address;
    ipcp->local.will_negotiate |= ipcp->local.want.negotiate;
    ipcp->remote.work.negotiate = 0;
    ipcp->remote.will_negotiate |= ipcp->remote.want.negotiate;
}
void ipcp_stopping(struct ppp_fsm *fsm)
{
    PP_stop_timer(&fsm->timer);
}
void ipcp_closing(struct ppp_fsm *fsm)
{
    struct ppp_ipcp *ipcp = fsm->context;
    PP_slhc_free(ipcp->compression);
    ipcp->compression = NULL;
}
void ipcp_opening(struct ppp_fsm *fsm)
{
    struct ppp_ipcp *ipcp = fsm->context;
    short receive_slots = 0;
    short send_slots = 0;
    PP_slhc_free(ipcp->compression);
    ipcp->compression = NULL;
    if (ipcp->local.work.negotiate & IPCP_COMPRESS_BIT) {
        receive_slots = ipcp->local.work.slots;
    }
    if (ipcp->remote.work.negotiate & IPCP_COMPRESS_BIT) {
        send_slots = ipcp->remote.work.slots;
    }
    if (receive_slots != 0 || send_slots != 0) {
        ipcp->compression = PP_slhc_init(receive_slots, send_slots);
    }
    PP_stop_timer(&fsm->timer);
    fsm->state = PPP_OPENED;
}
void ipcp_starting(struct ppp_fsm *fsm)
{
    fsm->reserved = 0;
    fsm->ack_count = 0;
}
void ipcp_release(struct ppp_fsm *fsm)
{
    struct ppp_ipcp *ipcp = fsm->context;
    PP_slhc_free(ipcp->compression);
}
short PP_doipcp_open(struct ppp_fsm *fsm)
{
    PP_doppp_active(fsm);
    if (fsm->session->phase == PPP_READY) {
        PP_fsm_start(fsm);
    }
    return 0;
}
short PP_doipcp_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.address = 0;
        side->want.negotiate &= ~IPCP_ADDRESS_BIT;
    } else {
        side->want.address = address;
        side->want.negotiate |= IPCP_ADDRESS_BIT;
    }
    return 0;
}
short PP_doipcp_remote_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.address = 0;
        side->want.negotiate &= ~IPCP_ADDRESS_BIT;
    } else {
        side->want.address = address;
        side->want.negotiate |= IPCP_ADDRESS_BIT;
    }
    return 0;
}
short PP_doipcp_dns1_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.dns1 = 0;
        side->want.negotiate &= ~IPCP_DNS1_BIT;
    } else {
        side->want.dns1 = address;
        side->want.negotiate |= IPCP_DNS1_BIT;
    }
    return 0;
}
short PP_doipcp_dns2_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.dns2 = 0;
        side->want.negotiate &= ~IPCP_DNS2_BIT;
    } else {
        side->want.dns2 = address;
        side->want.negotiate |= IPCP_DNS2_BIT;
    }
    return 0;
}
short PP_doipcp_remote_dns1_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.dns1 = 0;
        side->want.negotiate &= ~IPCP_DNS1_BIT;
    } else {
        side->want.dns1 = address;
        side->want.negotiate |= IPCP_DNS1_BIT;
    }
    return 0;
}
short PP_doipcp_remote_dns2_address(struct ipcp_side *side, long address)
{
    if (address == NO_ADDRESS) {
        side->want.dns2 = 0;
        side->want.negotiate &= ~IPCP_DNS2_BIT;
    } else {
        side->want.dns2 = address;
        side->want.negotiate |= IPCP_DNS2_BIT;
    }
    return 0;
}
short PP_doipcp_compress(short disable, struct ipcp_side *side)
{
    /* @hack redundant: the else's test of disable != 0, which the if has
     * settled */
    if (disable == 0) {
        side->want.compression = PPP_VJ_COMPRESSED;
        side->want.slots = VJ_SLOTS;
        side->want.slot_compression = 1;
        side->want.negotiate |= IPCP_COMPRESS_BIT;
    } else if (disable != 0) {
        side->want.negotiate &= ~IPCP_COMPRESS_BIT;
    }
    return 0;
}
void PP_ipcp_init(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    struct ppp_ipcp *ipcp;
    fsm->session = session;
    fsm->protocol = &ipcp_protocol;
    fsm->context = ipcp = &cont_ipcp_s;
    ipcp->local.want = ipcp_default_options;
    ipcp->local.will_negotiate = ipcp_default_negotiation;
    ipcp->remote.want = ipcp_default_options;
    ipcp->remote.work = ipcp_default_options;
    ipcp->remote.will_negotiate = ipcp_default_negotiation;
    PP_fsm_init(fsm);
}
short PP_ipcp_monitor(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    fsm->fault = PPP_FAULT_NONE;
    while (fsm->state != PPP_OPENED) {
        if (ppp_is_abort() || fsm->fault != PPP_FAULT_NONE ||
            session->phase == PPP_TERMINATING) {
            fsm->fault = PPP_FAULT_NONE;
            return -1;
        }
        PP_OS_CriticalUnLock();
        PP_OS_Yield();
        PP_timerproc();
        PP_OS_CriticalLock();
    }
    fsm->fault = PPP_FAULT_NONE;
    return 0;
}

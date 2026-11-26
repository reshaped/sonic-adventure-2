#include <sg_xpt.h>
#include "ppp_framing.h"
#include "ppp_fsm.h"
#include "ppp_lcp.h"
#include "ppp_link.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_serial.h"
#include "ppp_system.h"
#ifdef NONMATCHING
/* the option reader and writer this side's LCP code below takes from
 * ppp_options_impl.h, and its memcmp */
#include <string.h>
#include "ppp_options_impl.h"
#endif

static ppp_event lcp_release, lcp_reset, lcp_starting;
static ppp_check lcp_request, lcp_ack, lcp_nak, lcp_reject;
#ifdef NONMATCHING
#else
static short lcp_set_accm(struct ppp_lcp_side *side, long map);
static void lcp_set_address_compression(struct ppp_lcp_side *side,
                                        short disable);
static short lcp_set_authentication(struct ppp_lcp_side *side, short method);
static short lcp_set_magic(struct ppp_lcp_side *side, long magic_number);
static short lcp_set_mru(struct ppp_lcp_side *side, short mru);
static short lcp_set_protocol_compression(struct ppp_lcp_side *side,
                                          short disable);
static void lcp_closing(struct ppp_fsm *fsm);
static void lcp_stopping(struct ppp_fsm *fsm);
static struct ppp_buffer *lcp_make_request(struct ppp_fsm *fsm);
static Uint8 pull_word[4], pull_byte;
/* @hack type: a signed 1, where LCP_MASK's 1U would do */
#define LCP_BIT(option) (1 << (option))
enum { LCP_OPTION_HEADER_BYTES = 2 };
#endif
#pragma section ATA
static char lcp_name[4];
static struct ppp_protocol lcp_protocol = {
#include "network/ppp_lcp/lcp_protocol.inc"
};
static char lcp_name[4] = {
#include "network/ppp_lcp/lcp_name.inc"
};
#pragma section
static struct ppp_lcp_options lcp_default = {
#include "network/ppp_lcp/lcp_default.inc"
};
static Uint16 default_negotiation = {
#include "network/ppp_lcp/default_negotiation.inc"
};
Uint8 lcp_option_lengths[LCP_OPTION_TYPES] = {
#include "network/ppp_lcp/lcp_option_lengths.inc"
};
static struct ppp_lcp lcp_state;

short lcp_set_accm(struct ppp_lcp_side *side, long map)
{
    side->want.accm = map;
    if (map != LCP_NO_ACCM) {
        side->want.negotiate |= LCP_MASK(LCP_ACCM);
        side->will_negotiate |= LCP_MASK(LCP_ACCM);
    } else {
        side->want.negotiate &= ~LCP_MASK(LCP_ACCM);
        side->will_negotiate &= ~LCP_MASK(LCP_ACCM);
    }
    return 0;
}
void lcp_set_address_compression(struct ppp_lcp_side *side, short disable)
{
    if (disable == 0) {
        side->will_negotiate |= LCP_MASK(LCP_ADDRESS_COMPRESSION);
    } else {
        side->will_negotiate &= ~LCP_MASK(LCP_ADDRESS_COMPRESSION);
    }
}
short lcp_set_authentication(struct ppp_lcp_side *side, short method)
{
    if (method == PPP_AUTH_PAP) {
        side->want.negotiate |= LCP_MASK(LCP_AUTHENTICATION);
        side->want.authentication = PPP_PAP_PROTOCOL;
    } else if (method == PPP_AUTH_NONE) {
        side->want.negotiate &= ~LCP_MASK(LCP_AUTHENTICATION);
        side->want.chap_any = 0;
    } else if (method == PPP_AUTH_CHAP) {
        side->want.negotiate |= LCP_MASK(LCP_AUTHENTICATION);
        side->want.authentication = PPP_CHAP_PROTOCOL;
        side->want.chap_algorithm = CHAP_MD5;
        side->want.chap_any = 0;
    } else if (method == PPP_AUTH_AUTO) {
        side->want.negotiate |= LCP_MASK(LCP_AUTHENTICATION);
        side->want.authentication = PPP_CHAP_PROTOCOL;
        side->want.chap_algorithm = CHAP_MD5;
        side->want.chap_any = 1;
    } else {
        return 1;
    }
    return 0;
}
short lcp_set_magic(struct ppp_lcp_side *side, long magic_number)
{
    if (magic_number != 0) {
        side->want.negotiate |= LCP_MASK(LCP_MAGIC);
    } else {
        side->want.negotiate &= ~LCP_MASK(LCP_MAGIC);
    }
    side->want.magic_number = magic_number;
    return 0;
}
short lcp_set_mru(struct ppp_lcp_side *side, short mru)
{
    if (mru < LCP_MIN_MRU || mru > LCP_MAX_MRU) {
        return -1;
    }
    side->want.negotiate |= LCP_MASK(LCP_MRU);
    side->want.mru = mru;
    return 0;
}
short lcp_set_protocol_compression(struct ppp_lcp_side *side, short disable)
{
    if (disable == 0) {
        side->will_negotiate |= LCP_MASK(LCP_PROTOCOL_COMPRESSION);
    } else {
        side->will_negotiate &= ~LCP_MASK(LCP_PROTOCOL_COMPRESSION);
    }
    return 0;
}
#ifdef NONMATCHING
/* this side's emitter writes only the types it knows, each at the length
 * lcp_option_lengths gives, and takes no bytes from parameters. *packet becomes
 * NULL for any other type, or where the option would pass the frame's room */
void lcp_emit_option(struct ppp_buffer **packet,
                     const struct ppp_lcp_options *settings, Uint8 type,
                     Uint8 length, struct ppp_buffer **parameters)
{
    struct ppp_buffer *output;
    Uint8 *bytes;
    (void)parameters;
    if (!packet || !settings) {
        return;
    }
    if (type < LCP_MRU || type > LCP_ADDRESS_COMPRESSION ||
        length != lcp_option_lengths[type]) {
        *packet = NULL;
        return;
    }
    if (type == LCP_AUTHENTICATION) {
        if (settings->authentication == PPP_CHAP_PROTOCOL) {
            length++;
        } else if (settings->authentication != PPP_PAP_PROTOCOL) {
            *packet = NULL;
            return;
        }
    }
    output = *packet;
    if (!output) {
        output = PP_alloc_mbuf_work(1);
        if (!output) {
            return;
        }
        *packet = output;
    }
    if (!output->data || output->length > PPP_FRAME_DATA_BYTES - length) {
        *packet = NULL;
        return;
    }
    bytes = output->data + output->length;
    switch (type) {
    case LCP_MRU:
        PP_put16(bytes + 2, settings->mru);
        break;
    case LCP_ACCM:
        PP_put32(bytes + 2, settings->accm);
        break;
    case LCP_AUTHENTICATION:
        PP_put16(bytes + 2, settings->authentication);
        if (length == LCP_CHAP_OPTION_BYTES) {
            bytes[4] = settings->chap_algorithm;
        }
        break;
    case LCP_MAGIC:
        PP_put32(bytes + 2, settings->magic_number);
        break;
    case LCP_PROTOCOL_COMPRESSION:
    case LCP_ADDRESS_COMPRESSION:
        break;
    default:
        *packet = NULL;
        return;
    }
    bytes[0] = type;
    bytes[1] = length;
    output->length += length;
}

void lcp_write_options(struct ppp_buffer **packet,
                       const struct ppp_lcp_options *settings, Uint16 mask)
{
    unsigned int type;
    unsigned int requested = mask;
    for (type = LCP_MRU; type <= LCP_ADDRESS_COMPRESSION; type++) {
        requested >>= 1;
        if (requested & 1U) {
            lcp_emit_option(packet, settings, type, lcp_option_lengths[type],
                            NULL);
            if (!*packet) {
                return;
            }
        }
    }
}
#else
/* append the option type, length bytes long, to *packet. where *packet is
 * NULL, it takes work frame 1. the types this knows take their values from
 * settings. an authentication option takes LCP_PAP_OPTION_BYTES or
 * LCP_CHAP_OPTION_BYTES by its protocol, where length is either. the rest of
 * length's bytes come from parameters, which may be NULL. a byte parameters
 * can't give is 0xff */
static void lcp_emit_option(struct ppp_buffer **packet,
                            const struct ppp_lcp_options *settings, Uint8 type,
                            Uint8 length, struct ppp_buffer **parameters)
{
    struct ppp_buffer *output;
    Uint8 *bytes;
    Uint8 *length_byte;
    short remaining = length - LCP_OPTION_HEADER_BYTES;

    if (*packet == NULL) {
        *packet = PP_alloc_mbuf_work(1);
        if (*packet == NULL) {
            return;
        }
    }
    output = *packet;
    bytes = output->data + output->length;
    *bytes++ = type;
    length_byte = bytes;
    *bytes++ = length;

    switch (type) {
    case LCP_MRU:
        bytes = PP_put16(bytes, settings->mru);
        remaining -= 2;
        break;
    case LCP_ACCM:
        bytes = PP_put32(bytes, settings->accm);
        remaining -= 4;
        break;
    case LCP_AUTHENTICATION:
        bytes = PP_put16(bytes, settings->authentication);
        remaining -= 2;
        if (settings->authentication == PPP_CHAP_PROTOCOL) {
            *length_byte = LCP_CHAP_OPTION_BYTES;
            *bytes++ = settings->chap_algorithm;
            if (length == LCP_PAP_OPTION_BYTES) {
                length++;
            }
            remaining = 0;
        }
        if (settings->authentication == PPP_PAP_PROTOCOL) {
            *length_byte = LCP_PAP_OPTION_BYTES;
            if (length == LCP_CHAP_OPTION_BYTES) {
                length--;
            }
            remaining = 0;
        }
        break;
    case LCP_MAGIC:
        bytes = PP_put32(bytes, settings->magic_number);
        remaining -= 4;
        break;
    /* @hack redundant: the cases that fall to the default */
    case LCP_PROTOCOL_COMPRESSION:
    case LCP_ADDRESS_COMPRESSION:
    case LCP_ENCRYPTION:
    case LCP_QUALITY:
    default:
        break;
    }
    while (remaining-- > 0) {
        *bytes++ = PPP_PULL_BYTE(parameters);
    }
    output->length += length;
}

static void lcp_write_options(struct ppp_buffer **packet,
                              const struct ppp_lcp_options *settings,
                              short mask)
{
    short type;

    for (type = LCP_MRU; type <= (short)LCP_ADDRESS_COMPRESSION; type++) {
        if (mask & LCP_BIT(type)) {
            lcp_emit_option(packet, settings, type, lcp_option_lengths[type],
                            NULL);
        }
    }
}
#endif
struct ppp_buffer *lcp_make_request(struct ppp_fsm *fsm)
{
    struct ppp_lcp *lcp = fsm->context;
    /* @hack redundant: packet's NULL, which the allocation overwrites */
    struct ppp_buffer *packet = NULL;
    packet = PP_alloc_mbuf_work(0);
    if (!packet) {
        return NULL;
    }
    lcp_write_options(&packet, &lcp->local.work, lcp->local.work.negotiate);
    return packet;
}
#ifdef NONMATCHING
/* this side reads each option within the packet's bounds, and changes the LCP
 * state only once a whole packet checks. it repairs both bugs tagged in the
 * matching lcp_request: a nak uses up a retry only once it is sent, and a
 * reject carries the peer's own options. an ack must repeat the request this
 * end sent */
static int lcp_write_option(struct ppp_buffer *packet, Uint8 type,
                            const struct ppp_lcp_options *settings)
{
    if (type < LCP_MRU || type > LCP_ADDRESS_COMPRESSION) {
        return -1;
    }
    lcp_emit_option(&packet, settings, type, lcp_option_lengths[type], NULL);
    return packet ? 0 : -1;
}
static int lcp_append_options(struct ppp_buffer *packet,
                              const struct ppp_lcp_options *settings,
                              Uint16 mask)
{
    lcp_write_options(&packet, settings, mask);
    return packet ? 0 : -1;
}
static int lcp_authentication_option(struct ppp_lcp_side *side,
                                     const struct ppp_option *option)
{
    Uint16 protocol;
    Uint8 algorithm = 0;
    if (!(side->want.negotiate & LCP_MASK(LCP_AUTHENTICATION))) {
        return PPP_CONFIG_REJ;
    }
    if (option->length < LCP_PAP_OPTION_BYTES) {
        return -1;
    }
    protocol = PP_get16(option->bytes + 2);
    if (protocol == PPP_CHAP_PROTOCOL) {
        if (option->length != LCP_CHAP_OPTION_BYTES) {
            return -1;
        }
        algorithm = option->bytes[4];
    } else if (protocol == PPP_PAP_PROTOCOL &&
               option->length != LCP_PAP_OPTION_BYTES) {
        return -1;
    }
    if (side->want.chap_any) {
        if (protocol == PPP_PAP_PROTOCOL ||
            (protocol == PPP_CHAP_PROTOCOL && algorithm == CHAP_MD5)) {
            side->want.authentication = protocol;
            side->want.chap_algorithm = algorithm;
        } else {
            side->work.authentication = PPP_PAP_PROTOCOL;
            side->work.chap_algorithm = 0;
            return PPP_CONFIG_NAK;
        }
    }
    side->work.authentication = side->want.authentication;
    side->work.chap_algorithm = side->want.chap_algorithm;
    if (protocol != side->work.authentication ||
        (protocol == PPP_CHAP_PROTOCOL &&
         algorithm != side->work.chap_algorithm)) {
        return PPP_CONFIG_NAK;
    }
    return PPP_CONFIG_ACK;
}
static int lcp_check_option(struct ppp_lcp *lcp, struct ppp_lcp_side *side,
                            const struct ppp_option *option, int request)
{
    const Uint8 *value = option->bytes + 2;
    Uint16 mru;
    if (option->type > LCP_ADDRESS_COMPRESSION ||
        !(side->will_negotiate & LCP_MASK(option->type))) {
        return PPP_CONFIG_REJ;
    }
    if (option->type == LCP_AUTHENTICATION) {
        return lcp_authentication_option(side, option);
    }
    if (option->length != lcp_option_lengths[option->type]) {
        return -1;
    }
    switch (option->type) {
    case LCP_MRU:
        mru = PP_get16(value);
        if (mru < LCP_MIN_MRU) {
            side->work.mru = LCP_MIN_MRU;
        } else if (mru > LCP_MAX_MRU) {
            side->work.mru = LCP_MAX_MRU;
        } else {
            side->work.mru = mru;
        }
        if (request && (side->want.negotiate & LCP_MASK(LCP_MRU)) &&
            side->work.mru > side->want.mru) {
            side->work.mru = side->want.mru;
        }
        return mru == side->work.mru ? PPP_CONFIG_ACK : PPP_CONFIG_NAK;
    case LCP_ACCM:
        side->work.accm = PP_get32(value);
        if ((!request || (side->want.negotiate & LCP_MASK(LCP_ACCM))) &&
            (side->work.accm & side->want.accm) != side->want.accm) {
            side->work.accm |= side->want.accm;
            return PPP_CONFIG_NAK;
        }
        break;
    case LCP_MAGIC:
        side->work.magic_number = PP_get32(value);
        if (!side->work.magic_number ||
            lcp->remote.work.magic_number == lcp->local.work.magic_number) {
            side->work.magic_number += PP_Clock();
            return PPP_CONFIG_NAK;
        }
        break;
    case LCP_PROTOCOL_COMPRESSION:
    case LCP_ADDRESS_COMPRESSION:
        break;
    default:
        return PPP_CONFIG_REJ;
    }
    return PPP_CONFIG_ACK;
}
static short lcp_request(struct ppp_fsm *fsm, struct ppp_control_header *header,
                         struct ppp_buffer *packet)
{
    struct ppp_lcp *lcp = fsm->context;
    struct ppp_lcp state;
    struct ppp_buffer input;
    struct ppp_buffer *reply;
    struct ppp_option option;
    int reply_code = PPP_CONFIG_ACK;
    int code;
    int status;
    state = *lcp;
    input = *packet;
    reply = option_workspace(&input);
    if (!reply || header->length != input.length) {
        return -1;
    }
    state.remote.work.negotiate = 0;
    while ((status = next_option(&input, &option)) > 0) {
        code = lcp_check_option(&state, &state.remote, &option, 1);
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
            reply_code = code;
            reply->length = 0;
        }
        if (code != PPP_CONFIG_REJ) {
            state.remote.work.negotiate |= LCP_MASK(option.type);
        }
        if (code == PPP_CONFIG_NAK) {
            if (lcp_write_option(reply, option.type, &state.remote.work) < 0) {
                return -1;
            }
        } else if (append_option(reply, option.bytes, option.length) < 0) {
            return -1;
        }
    }
    if (status < 0) {
        return -1;
    }
    if (reply_code == PPP_CONFIG_NAK && fsm->nak_retries) {
        if (lcp_append_options(reply, &state.remote.want,
                               state.remote.want.negotiate &
                                   ~state.remote.work.negotiate) < 0) {
            return -1;
        }
    }
    if (PP_fsm_send(fsm, reply_code, header->id, reply) < 0) {
        return -1;
    }
    if (reply_code == PPP_CONFIG_NAK && fsm->nak_retries) {
        fsm->nak_retries--;
    }
    *lcp = state;
    return reply_code != PPP_CONFIG_ACK;
}
static short lcp_ack(struct ppp_fsm *fsm, struct ppp_control_header *header,
                     struct ppp_buffer *packet)
{
    struct ppp_lcp *lcp = fsm->context;
    struct ppp_buffer input;
    struct ppp_buffer *request;
    input = *packet;
    if (header->id != fsm->last_id || header->length != input.length) {
        return -1;
    }
    request = option_workspace(&input);
    if (!request ||
        lcp_append_options(request, &lcp->local.work,
                           lcp->local.work.negotiate) < 0 ||
        request->length != input.length ||
        memcmp(request->data, input.data, input.length)) {
        return -1;
    }
    return 0;
}
static short lcp_adjust_options(struct ppp_fsm *fsm,
                                struct ppp_control_header *header,
                                struct ppp_buffer *packet, int reject)
{
    struct ppp_lcp *lcp = fsm->context;
    struct ppp_lcp state;
    struct ppp_buffer input;
    struct ppp_option option;
    int status;
    int last_type = 0;
    state = *lcp;
    input = *packet;
    if (header->id != fsm->last_id || header->length != input.length) {
        return -1;
    }
    while ((status = next_option(&input, &option)) > 0) {
        Uint16 mask;
        int result;
        if (option.type > LCP_ADDRESS_COMPRESSION) {
            continue;
        }
        mask = LCP_MASK(option.type);
        if (reject) {
            if (option.type < last_type ||
                !(state.local.work.negotiate & mask)) {
                return -1;
            }
            last_type = option.type;
            result = PPP_CONFIG_REJ;
        } else {
            if (!(state.local.work.negotiate & mask)) {
                last_type = LCP_OPTION_TYPES;
                state.local.work.negotiate |= mask;
            } else if (option.type < last_type) {
                return -1;
            } else {
                last_type = option.type;
            }
            result = lcp_check_option(&state, &state.local, &option, 0);
            if (result < 0) {
                return -1;
            }
        }
        if (result == PPP_CONFIG_REJ &&
            (option.type != LCP_AUTHENTICATION ||
             !(state.local.want.negotiate & mask))) {
            state.local.work.negotiate &= ~mask;
        }
    }
    if (status < 0) {
        return -1;
    }
    *lcp = state;
    return 0;
}
static short lcp_nak(struct ppp_fsm *fsm, struct ppp_control_header *header,
                     struct ppp_buffer *packet)
{
    return lcp_adjust_options(fsm, header, packet, 0);
}
static short lcp_reject(struct ppp_fsm *fsm, struct ppp_control_header *header,
                        struct ppp_buffer *packet)
{
    return lcp_adjust_options(fsm, header, packet, 1);
}
#else
/* check option, whose value is at *packet, reading its values into
 * side->work. the result is PPP_CONFIG_ACK, PPP_CONFIG_NAK with side->work
 * holding the values to ask for, or PPP_CONFIG_REJ. it is -1 where the length
 * is shorter than the values or the bytes run out. request is 1 for a peer's
 * request. the option's bytes past its values then stay at *packet, for
 * lcp_emit_option to copy. request is 0 for a nak, and the check skips those
 * bytes */
static short lcp_check(struct ppp_buffer **packet, struct ppp_lcp *lcp,
                       struct ppp_lcp_side *side,
                       struct ppp_option_header *option, short request)
{
    short remaining = option->length - LCP_OPTION_HEADER_BYTES;
    int option_result = PPP_CONFIG_ACK;

    switch (option->type) {
    case LCP_MRU:
        side->work.mru = PPP_PULL16(packet);
        remaining -= 2;
        if (side->work.mru < LCP_MIN_MRU) {
            side->work.mru = LCP_MIN_MRU;
            option_result = PPP_CONFIG_NAK;
        } else if (side->work.mru > LCP_MAX_MRU) {
            side->work.mru = LCP_MAX_MRU;
            option_result = PPP_CONFIG_NAK;
        }
        if (!request) {
            break;
        }
        if ((side->want.negotiate & LCP_MASK(LCP_MRU)) &&
            side->work.mru > side->want.mru) {
            side->work.mru = side->want.mru;
            option_result = PPP_CONFIG_NAK;
        }
        break;
    case LCP_ACCM:
        side->work.accm = PPP_PULL32(packet);
        remaining -= 4;
        if ((!request || (side->want.negotiate & LCP_MASK(LCP_ACCM))) &&
            side->work.accm != (side->work.accm | side->want.accm)) {
            side->work.accm |= side->want.accm;
            option_result = PPP_CONFIG_NAK;
        }
        break;
    case LCP_AUTHENTICATION:
        side->work.authentication = PPP_PULL16(packet);
        remaining -= 2;
        if (side->work.authentication == PPP_CHAP_PROTOCOL) {
            side->work.chap_algorithm = PPP_PULL_BYTE(packet);
            remaining -= 1;
        }
        if (side->want.chap_any) {
            /* @hack type: the algorithm compared as a signed char, where its
             * Uint8 would do */
            if (side->work.authentication == PPP_PAP_PROTOCOL ||
                (side->work.authentication == PPP_CHAP_PROTOCOL &&
                 (Sint8)side->work.chap_algorithm == CHAP_MD5)) {
                side->want.authentication = side->work.authentication;
                side->want.chap_algorithm = side->work.chap_algorithm;
                break;
            }
            side->work.authentication = PPP_PAP_PROTOCOL;
            side->work.chap_algorithm = 0;
            option_result = PPP_CONFIG_NAK;
            option->length = LCP_PAP_OPTION_BYTES;
            while (remaining > 0) {
                remaining--;
                if (PPP_PULL_BYTE(packet) == -1) {
                    return -1;
                }
            }
            break;
        }
        if (side->work.authentication != side->want.authentication ||
            (side->work.authentication == PPP_CHAP_PROTOCOL &&
             side->work.chap_algorithm != side->want.chap_algorithm)) {
            side->work.authentication = side->want.authentication;
            option_result = PPP_CONFIG_NAK;
            side->work.chap_algorithm = side->want.chap_algorithm;
        }
        break;
    case LCP_MAGIC:
        side->work.magic_number = PPP_PULL32(packet);
        remaining -= 4;
        if (side->work.magic_number == 0 ||
            lcp->remote.work.magic_number == lcp->local.work.magic_number) {
            side->work.magic_number += PP_Clock();
            option_result = PPP_CONFIG_NAK;
        }
        break;
    case LCP_PROTOCOL_COMPRESSION:
    case LCP_ADDRESS_COMPRESSION:
        break;
    /* @hack redundant: the cases that fall to the default */
    case 0:
    case LCP_ENCRYPTION:
    case LCP_QUALITY:
    default:
        option_result = PPP_CONFIG_REJ;
        break;
    }

    if (option->type > LCP_ADDRESS_COMPRESSION ||
        !(side->will_negotiate & LCP_BIT(option->type))) {
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
    /* @hack redundant: an empty switch on the result */
    switch (option_result) {
    }
    return option_result;
}

static short lcp_request(struct ppp_fsm *fsm, struct ppp_control_header *header,
                         struct ppp_buffer *data)
{
    struct ppp_lcp *lcp = fsm->context;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_buffer *reply = NULL;
    short reply_result = PPP_CONFIG_ACK;
    short suggested;
    struct ppp_option_header option;
    short option_result;

    lcp->remote.work.negotiate = 0;
    reply = PP_alloc_mbuf_work(1);
    if (lcp->remote.want.chap_any) {
        lcp->remote.work.authentication = 0;
        lcp->remote.work.negotiate |= LCP_MASK(LCP_AUTHENTICATION);
    }
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        if ((left -= option.length) < 0) {
            return -1;
        }
        option_result = lcp_check(&data, lcp, &lcp->remote, &option, 1);
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
        if (option_result != PPP_CONFIG_REJ &&
            option.type <= LCP_ADDRESS_COMPRESSION) {
            lcp->remote.work.negotiate |= LCP_BIT(option.type);
        }
        lcp_emit_option(&reply, &lcp->remote.work, option.type, option.length,
                        &data);
    }

    suggested = lcp->remote.want.negotiate & ~lcp->remote.work.negotiate;
    if (reply_result == PPP_CONFIG_NAK) {
        if (fsm->nak_retries > 0) {
            lcp_write_options(&reply, &lcp->remote.work, suggested);
            /* @bug logic: takes one of nak_retries though the send fails
             * (0x0c9e64bc, 0x0c9e64d0) */
            fsm->nak_retries--;
        } else {
            /* @bug logic: rejects with this end's values (0x0c9e64c2) */
            reply_result = PPP_CONFIG_REJ;
        }
    }
    PP_fsm_send(fsm, reply_result, header->id, reply);
    return reply_result != PPP_CONFIG_ACK;
}

static short lcp_ack(struct ppp_fsm *fsm, struct ppp_control_header *header,
                     struct ppp_buffer *data)
{
    struct ppp_buffer *request_packet;
    short error = 0;

    if (header->id != fsm->last_id) {
        return -1;
    }
    request_packet = lcp_make_request(fsm);
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    if ((short)header->length != PP_len_p(request_packet)) {
        error = 1;
    } else {
        short requested_byte;
        short acknowledged_byte;

        while ((requested_byte = PPP_PULL_BYTE(&request_packet)) != -1) {
            if ((acknowledged_byte = PPP_PULL_BYTE(&data)) == -1 ||
                acknowledged_byte != requested_byte) {
                error = 1;
                break;
            }
        }
    }
    if (error) {
        return -1;
    }
    return 0;
}

static short lcp_nak(struct ppp_fsm *fsm, struct ppp_control_header *header,
                     struct ppp_buffer *data)
{
    struct ppp_lcp *lcp = fsm->context;
    struct ppp_lcp_side *local = &lcp->local;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_option_header option;
    short last_option = 0;
    short result;

    if (header->id != fsm->last_id) {
        return -1;
    }
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        if ((left -= option.length) < 0) {
            return -1;
        }
        /* a nak lists the requested options it won't take, in the request's
         * type order, then those it suggests. last_option is the last
         * requested type read. a suggestion sets it past every type, so each
         * option after one is a suggestion too */
        if (option.type <= LCP_ADDRESS_COMPRESSION) {
            if (option.type < last_option ||
                !(local->work.negotiate & LCP_BIT(option.type))) {
                if (local->work.negotiate & LCP_BIT(option.type)) {
                    return -1;
                }
                local->work.negotiate |= LCP_BIT(option.type);
                last_option = LCP_OPTION_TYPES;
            } else {
                last_option = option.type;
            }
        }
        result = lcp_check(&data, lcp, local, &option, 0);
        if (result == -1) {
            return -1;
        }
        /* every option a nak lists is now in the next request. one the check
         * rejects leaves it, but authentication. this end asks for that until
         * its retries run out */
        if (result == PPP_CONFIG_REJ &&
            option.type <= LCP_ADDRESS_COMPRESSION) {
            /* @hack redundant: the test of the option's bit, which the lines
             * above have set */
            if (option.type != LCP_AUTHENTICATION ||
                !(local->work.negotiate & LCP_BIT(option.type))) {
                local->work.negotiate &= ~LCP_BIT(option.type);
            }
        }
    }
    return 0;
}

static short lcp_reject(struct ppp_fsm *fsm, struct ppp_control_header *header,
                        struct ppp_buffer *data)
{
    struct ppp_lcp *lcp = fsm->context;
    struct ppp_lcp_side *local = &lcp->local;
    /* @hack type: the length read as a short, where its Uint16 would do
     * below 0x8000 */
    long left = (short)header->length;
    struct ppp_option_header option;
    short last_option = 0;

    if (header->id != fsm->last_id) {
        return -1;
    }
    while (left > 0 && PP_ntohopt(&option, &data) != -1) {
        short remaining;

        if ((left -= option.length) < 0) {
            return -1;
        }
        /* a reject holds requested options alone, in the request's type
         * order */
        if (option.type <= LCP_ADDRESS_COMPRESSION) {
            if (option.type < last_option ||
                !(local->work.negotiate & LCP_BIT(option.type))) {
                return -1;
            }
        }
        for (remaining = option.length - LCP_OPTION_HEADER_BYTES;
             remaining-- > 0;) {
            if (PPP_PULL_BYTE(&data) == -1) {
                return -1;
            }
        }
        last_option = option.type;
        /* each rejected option leaves the next request, but authentication.
         * this end asks for that until its retries run out */
        if (option.type <= LCP_ADDRESS_COMPRESSION) {
            /* @hack redundant: the test of the option's bit, which the check
             * above found set */
            if (option.type != LCP_AUTHENTICATION ||
                !(local->work.negotiate & LCP_BIT(option.type))) {
                local->work.negotiate &= ~LCP_BIT(option.type);
            }
        }
    }
    return 0;
}
#endif
static void lcp_reset(struct ppp_fsm *fsm)
{
    struct ppp_lcp *lcp = fsm->context;
    if (lcp->local.want.negotiate & LCP_MASK(LCP_MAGIC)) {
        lcp->local.want.magic_number += PP_Clock();
    }
    lcp->local.work = lcp->local.want;
    lcp->local.will_negotiate |= lcp->local.want.negotiate;
    lcp->remote.work.negotiate = 0;
    lcp->remote.will_negotiate |= lcp->remote.want.negotiate;
}
static void lcp_starting(struct ppp_fsm *fsm)
{
    fsm->session->phase = PPP_LINK;
}
void lcp_stopping(struct ppp_fsm *fsm)
{
    ppp_iostatus(PPP_SERIAL_HANG_UP);
}

void lcp_closing(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    session->phase = PPP_TERMINATING;
    PP_fsm_down(&session->fsm[PPP_IPCP]);
    PP_pap_down(&session->fsm[PPP_PAP]);
}

void lcp_opening(struct ppp_fsm *fsm)
{
    struct ppp_lcp *lcp = fsm->context;

    fsm->session->phase = PPP_AUTH;
    fsm->session->flags &= ~(PPP_AUTH_LOCAL | PPP_AUTH_REMOTE);
    fsm->session->peer_name = NULL;

    if (lcp->local.work.negotiate & LCP_MASK(LCP_AUTHENTICATION)) {
        switch (lcp->local.work.authentication) {
        case PPP_PAP_PROTOCOL:
            PP_pap_local(fsm->session);
            break;
        case PPP_CHAP_PROTOCOL:
            PP_chap_local(fsm->session);
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: it starts CHAP for this
             * end alone */
#else
            /* @bug logic: starts CHAP for the peer to authenticate this end
             * too, which the peer may not have negotiated, at 0x0c9e683c */
            PP_chap_remote(fsm->session);
#endif
            break;
#ifdef NONMATCHING
        /* this side closes the link where the negotiated authentication is
         * neither PAP nor CHAP. the matching side goes on to ppp_ready */
        default:
            PP_fsm_close(fsm);
            return;
#endif
        }
    }
    if (lcp->remote.work.negotiate & LCP_MASK(LCP_AUTHENTICATION)) {
        switch (lcp->remote.work.authentication) {
        case PPP_PAP_PROTOCOL:
            PP_pap_remote(fsm->session);
            break;
        case PPP_CHAP_PROTOCOL:
            PP_chap_remote(fsm->session);
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: it starts CHAP for the
             * peer alone */
#else
            /* @bug logic: starts CHAP for this end to authenticate the peer
             * too, which this end may not have negotiated, at 0x0c9e6870 */
            PP_chap_local(fsm->session);
#endif
            break;
#ifdef NONMATCHING
        /* this side closes the link where the negotiated authentication is
         * neither PAP nor CHAP. the matching side goes on to ppp_ready */
        default:
            PP_fsm_close(fsm);
            return;
#endif
        }
    }
    ppp_ready(fsm->session);
}
static void lcp_release(struct ppp_fsm *fsm)
{
}
#ifdef NONMATCHING
/* this side fails the open for a method lcp_set_authentication doesn't know,
 * which the matching side ignores, and drops the matching side's test of the
 * phase lcp_starting has just set */
short PP_dolcp_open(struct ppp_fsm *fsm, struct ppp_connection *configuration)
{
    struct ppp_lcp *lcp = fsm->context;
    int method = configuration->authentication;
    PP_fsm_reset(fsm);
    PP_doppp_active(fsm);
    lcp_starting(fsm);
    lcp_set_mru(&lcp->local, configuration->mru);
    lcp_set_accm(&lcp->local, configuration->character_map);
    if ((method & PPP_AUTH_PEER) || method == PPP_AUTH_AUTO) {
        if (lcp_set_authentication(&lcp->remote, method & PPP_AUTH_METHOD)) {
            return -1;
        }
    } else if (lcp_set_authentication(&lcp->remote, method) ||
               lcp_set_authentication(&lcp->local, method)) {
        return -1;
    }
    lcp_set_magic(&lcp->local, configuration->magic_number);
    lcp_set_protocol_compression(&lcp->local,
                                 configuration->protocol_compression);
    lcp_set_address_compression(&lcp->local,
                                configuration->address_compression);
    PP_fsm_start(fsm);
    return 0;
}
#else
short PP_dolcp_open(struct ppp_fsm *fsm, struct ppp_connection *configuration)
{
    /* @hack spelling: fsm copied to link_fsm, where fsm itself would do */
    struct ppp_fsm *link_fsm = fsm;
    struct ppp_lcp *lcp;

    PP_fsm_reset(link_fsm);
    PP_doppp_active(link_fsm);
    lcp_starting(link_fsm);
    /* @hack redundant: the test of phase, which lcp_starting has just set to
     * PPP_LINK */
    if (link_fsm->session->phase >= PPP_LINK) {
        lcp = link_fsm->context;
        lcp_set_mru(&lcp->local, configuration->mru);
        lcp_set_accm(&lcp->local, configuration->character_map);
        if ((configuration->authentication & PPP_AUTH_PEER) ||
            configuration->authentication == PPP_AUTH_AUTO) {
            lcp_set_authentication(&lcp->remote, configuration->authentication &
                                                     PPP_AUTH_METHOD);
        } else {
            lcp_set_authentication(&lcp->remote, configuration->authentication);
            lcp_set_authentication(&lcp->local, configuration->authentication);
        }
        lcp_set_magic(&lcp->local, configuration->magic_number);
        lcp_set_protocol_compression(&lcp->local,
                                     configuration->protocol_compression);
        lcp_set_address_compression(&lcp->local,
                                    configuration->address_compression);
        PP_fsm_start(link_fsm);
    }
    return 0;
}
#endif
void ppp_ready(struct ppp_session *session)
{
#ifdef NONMATCHING
    /* this side readies the link only from PPP_AUTH, so a late call starts no
     * IPCP on a link past it */
    if (session->phase != PPP_AUTH) {
        return;
    }
#endif
    if (!(session->flags & (PPP_AUTH_LOCAL | PPP_AUTH_REMOTE))) {
        session->phase = PPP_READY;
        session->up_since = PP_secclock();
        PP_fsm_start(&session->fsm[PPP_IPCP]);
    }
}
void PP_lcp_init(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_LCP];
    struct ppp_lcp *lcp;
    fsm->session = session;
    fsm->protocol = &lcp_protocol;
    fsm->context = lcp = &lcp_state;
    lcp->local.want = lcp_default;
    lcp->local.will_negotiate = default_negotiation;
    lcp->remote.want = lcp_default;
    lcp->remote.work = lcp_default;
    lcp->remote.will_negotiate = default_negotiation;
    PP_fsm_init(fsm);
}
short PP_lcp_monitor(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    fsm->fault = PPP_FAULT_NONE;
    while (fsm->state != PPP_OPENED) {
        if (ppp_is_abort() || fsm->fault != PPP_FAULT_NONE ||
            session->phase == PPP_TERMINATING) {
            PP_fsm_close(fsm);
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

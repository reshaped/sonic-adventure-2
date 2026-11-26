#include <sg_xpt.h>
#include "net_debug.h"
#include "ppp_fsm.h"
#include "ppp_packet.h"
#include "ppp_system.h"

#define OPTION_HEADER_BYTES 2
#define EMPTY_FRAME_BYTES 128

#pragma section ATA
static char config_req_name[12] = "CONFIG_REQ";
static char config_ack_name[12] = "CONFIG_ACK";
static char config_nak_name[12] = "CONFIG_NAK";
static char config_rej_name[12] = "CONFIG_REJ";
static char term_req_name[12] = "TERM_REQ";
static char term_ack_name[12] = "TERM_ACK";
static char code_rej_name[12] = "CODE_REJ";
static char protocol_rej_name[12] = "PROT_REJ";
static char echo_req_name[12] = "ECHO_REQ";
static char echo_reply_name[12] = "ECHO_REPLY";
static char discard_req_name[12] = "DISCARD_REQ";
static char quality_name[16] = "QUALITY_REPORT";
static char unknown_name[4] = "???";
static char closed_name[8] = "CLOSED";
static char listen_name[8] = "LISTEN";
static char request_sent_name[12] = "REQ_Sent";
static char ack_received_name[12] = "ACK_Rcvd";
static char ack_sent_name[12] = "ACK_Sent";
static char opened_name[8] = "OPENED";
static char terminate_sent_name[12] = "TERM_Sent";
static char receive_format[] = "DEBUG: ppp/lcp: recv %s on %s\n";
#pragma section

struct ppp_buffer *PP_htoncnf(const struct ppp_control_header *header,
                              struct ppp_buffer *packet)
{
    Uint8 *data;
    packet = PP_pushdown(packet, CONTROL_HEADER_BYTES);
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it returns the NULL, which
     * PP_fsm_send tests for */
    if (!packet) {
        return NULL;
    }
#endif
    /* @bug memory: reads through the NULL PP_pushdown returns where packet
     * has no room. PP_fsm_send tests for that NULL (0x0c9e4874, 0x0c9e49e6) */
    data = packet->data;
    *data++ = header->code;
    *data++ = header->id;
    PP_put16(data, header->length);
    return packet;
}
short PP_ntohcnf(struct ppp_control_header *header, struct ppp_buffer **packet)
{
    Uint8 data[CONTROL_HEADER_BYTES];
    if (header == NULL) {
        return -1;
    }
    if (PP_pullup(packet, data, CONTROL_HEADER_BYTES) < CONTROL_HEADER_BYTES) {
        return -1;
    }
    header->code = data[0];
    header->id = data[1];
    header->length = PP_get16(&data[2]);
    return 0;
}
short PP_ntohopt(struct ppp_option_header *option, struct ppp_buffer **packet)
{
    Uint8 data[OPTION_HEADER_BYTES];
    if (option == NULL) {
        return -1;
    }
    if (PP_pullup(packet, data, OPTION_HEADER_BYTES) < OPTION_HEADER_BYTES) {
        return -1;
    }
    option->type = data[0];
    option->length = data[1];
    return 0;
}
void PP_fsm_no_action(struct ppp_fsm *fsm)
{
}
short PP_fsm_no_check(struct ppp_fsm *fsm, struct ppp_control_header *header,
                      struct ppp_buffer *packet)
{
    return 0;
}
void PP_fsm_timer(struct ppp_fsm *fsm)
{
    PP_start_timer(&fsm->timer);
}
short PP_fsm_send(struct ppp_fsm *fsm, Uint8 code, Uint8 id,
                  struct ppp_buffer *data)
{
    struct ppp_session *session = fsm->session;
    struct {
        struct ppp_buffer packet;
        Uint8 headroom[PPP_HEADROOM];
        Uint8 payload[EMPTY_FRAME_BYTES];
    } empty_frame;
    struct ppp_control_header header;
    struct ppp_buffer *packet;
    struct ppp_lcp *lcp;

    if (data == NULL) {
        packet = &empty_frame.packet;
        data = packet;
        /* @hack spelling: the payload's address stepped from the frame's,
         * where empty_frame.payload would do */
        data->data = (Uint8 *)packet + sizeof(*packet) + PPP_HEADROOM;
        data->length = 0;
    }
    header.code = code;
    switch (code) {
    case PPP_CONFIG_REQ:
    case PPP_TERM_REQ:
    case PPP_ECHO_REQ:
        fsm->last_id = session->id;
        /* fall through */
    case PPP_PROTOCOL_REJ:
    case PPP_DISCARD_REQ:
        header.id = session->id++;
        break;
    case PPP_CONFIG_ACK:
    case PPP_CONFIG_NAK:
    case PPP_CONFIG_REJ:
    case PPP_TERM_ACK:
    case PPP_CODE_REJ:
    case PPP_ECHO_REPLY:
        header.id = id;
        break;
    default:
        return -1;
    }
    switch (code) {
    case PPP_ECHO_REQ:
    case PPP_ECHO_REPLY:
    case PPP_DISCARD_REQ:
        lcp = fsm->context;
        PP_put32(data->data, lcp->local.work.magic_number);
    }
    header.length = PP_len_p(data) + CONTROL_HEADER_BYTES;
    packet = PP_htoncnf(&header, data);
    if (packet == NULL) {
        return -1;
    }
    session->control_sent[fsm->protocol->index]++;
    return ppp_output(NULL, NULL, fsm->protocol->protocol, packet);
}
short PP_fsm_sendreq(struct ppp_fsm *fsm)
{
    struct ppp_buffer *packet;
    if (fsm->retries <= 0) {
        return -1;
    }
    fsm->retries--;
    PP_fsm_timer(fsm);
    packet = fsm->protocol->make_request(fsm);
    return PP_fsm_send(fsm, PPP_CONFIG_REQ, 0, packet);
}
static short send_terminate_request(struct ppp_fsm *fsm)
{
    if (fsm->retries <= 0) {
        return -1;
    }
    fsm->retries--;
    PP_fsm_timer(fsm);
    return PP_fsm_send(fsm, PPP_TERM_REQ, 0, NULL);
}
static short send_terminate_ack(struct ppp_fsm *fsm, Uint8 id)
{
    return PP_fsm_send(fsm, PPP_TERM_ACK, id, NULL);
}
void PP_fsm_reset(struct ppp_fsm *fsm)
{
    fsm->state =
        fsm->flags & (PPP_ACTIVE | PPP_PASSIVE) ? PPP_LISTEN : PPP_CLOSED;
    fsm->retries = fsm->max_requests;
    fsm->nak_retries = fsm->max_naks;
    fsm->protocol->reset(fsm);
}
static void open_protocol(struct ppp_fsm *fsm)
{
    PP_stop_timer(&fsm->timer);
    fsm->protocol->opening(fsm);
    fsm->state = PPP_OPENED;
}
static const char *code_name(int code)
{
    switch (code) {
    case PPP_CONFIG_REQ:
        return config_req_name;
    case PPP_CONFIG_ACK:
        return config_ack_name;
    case PPP_CONFIG_NAK:
        return config_nak_name;
    case PPP_CONFIG_REJ:
        return config_rej_name;
    case PPP_TERM_REQ:
        return term_req_name;
    case PPP_TERM_ACK:
        return term_ack_name;
    case PPP_CODE_REJ:
        return code_rej_name;
    case PPP_PROTOCOL_REJ:
        return protocol_rej_name;
    case PPP_ECHO_REQ:
        return echo_req_name;
    case PPP_ECHO_REPLY:
        return echo_reply_name;
    case PPP_DISCARD_REQ:
        return discard_req_name;
    case PPP_QUALITY_REPORT:
        return quality_name;
    }
    return unknown_name;
}
static const char *state_name(int state)
{
    switch (state) {
    case PPP_CLOSED:
        return closed_name;
    case PPP_LISTEN:
        return listen_name;
    case PPP_REQUEST_SENT:
        return request_sent_name;
    case PPP_ACK_RECEIVED:
        return ack_received_name;
    case PPP_ACK_SENT:
        return ack_sent_name;
    case PPP_OPENED:
        return opened_name;
    case PPP_TERMINATE_SENT:
        return terminate_sent_name;
    }
    return unknown_name;
}

/* the states in which a PPP_CONFIG_ACK, PPP_CONFIG_NAK or PPP_CONFIG_REJ
 * comes with no request awaiting it. an opened fsm closes and starts over.
 * it then sends its request again, as one in PPP_ACK_RECEIVED does. in
 * PPP_CLOSED and PPP_LISTEN the fsm answers with a PPP_TERM_ACK, and in
 * PPP_TERMINATE_SENT it ignores the reply */
#define UNAWAITED_REPLY_CASES(fsm, id)                                         \
    case PPP_OPENED:                                                           \
        (fsm)->protocol->closing(fsm);                                         \
        (fsm)->protocol->starting(fsm);                                        \
        PP_fsm_reset(fsm);                                                     \
        /* fall through */                                                     \
    case PPP_ACK_RECEIVED:                                                     \
        PP_fsm_sendreq(fsm);                                                   \
        (fsm)->state = PPP_REQUEST_SENT;                                       \
        break;                                                                 \
    case PPP_CLOSED:                                                           \
    case PPP_LISTEN:                                                           \
        send_terminate_ack((fsm), (id));                                       \
        /* fall through */                                                     \
    case PPP_TERMINATE_SENT:                                                   \
        break

void PP_fsm_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet)
{
    struct ppp_control_header header;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it drops a packet whose header
     * PP_ntohcnf can't read, or whose length disagrees with the packet's */
    if (PP_ntohcnf(&header, &packet) != 0 ||
        header.length < CONTROL_HEADER_BYTES ||
        header.length - CONTROL_HEADER_BYTES > packet->length) {
        return;
    }
#else
    /* @bug uninitialized: reads header unset where the packet holds fewer
     * than four bytes. nothing tests PP_ntohcnf's result (0x0c9e4c32) */
    PP_ntohcnf(&header, &packet);
#endif
    header.length -= CONTROL_HEADER_BYTES;
    PP_trim_mbuf(&packet, header.length);
    dbPrint(DB_DEBUG_PPP, receive_format, code_name(header.code),
            state_name(fsm->state));
    switch (header.code) {
    case PPP_CONFIG_REQ:
        switch (fsm->state) {
        case PPP_OPENED:
            fsm->protocol->closing(fsm);
            PP_fsm_reset(fsm);
            /* fall through */
        case PPP_LISTEN:
            fsm->protocol->starting(fsm);
            PP_fsm_sendreq(fsm);
            /* fall through */
        case PPP_REQUEST_SENT:
        case PPP_ACK_SENT:
            if (fsm->protocol->request(fsm, &header, packet) == 0) {
                fsm->state = PPP_ACK_SENT;
            } else {
                fsm->state = PPP_REQUEST_SENT;
            }
            break;
        case PPP_ACK_RECEIVED:
            if (fsm->protocol->request(fsm, &header, packet) == 0) {
                open_protocol(fsm);
            } else {
                PP_fsm_timer(fsm);
            }
            break;
        case PPP_CLOSED:
            send_terminate_ack(fsm, header.id);
            /* fall through */
        case PPP_TERMINATE_SENT:
            break;
        }
        break;
    case PPP_CONFIG_ACK:
        switch (fsm->state) {
        case PPP_REQUEST_SENT:
            if (fsm->protocol->ack(fsm, &header, packet) == 0) {
                fsm->state = PPP_ACK_RECEIVED;
            }
            break;
        case PPP_ACK_SENT:
            if (fsm->protocol->ack(fsm, &header, packet) == 0) {
                open_protocol(fsm);
            }
            break;
            UNAWAITED_REPLY_CASES(fsm, header.id);
        }
        break;
    case PPP_CONFIG_NAK:
        switch (fsm->state) {
        case PPP_REQUEST_SENT:
        case PPP_ACK_SENT:
            if (fsm->protocol->nak(fsm, &header, packet) == 0) {
                PP_fsm_sendreq(fsm);
            }
            break;
            UNAWAITED_REPLY_CASES(fsm, header.id);
        }
        break;
    case PPP_CONFIG_REJ:
        switch (fsm->state) {
        case PPP_REQUEST_SENT:
        case PPP_ACK_SENT:
            if (fsm->protocol->reject(fsm, &header, packet) == 0) {
                PP_fsm_sendreq(fsm);
            }
            break;
            UNAWAITED_REPLY_CASES(fsm, header.id);
        }
        break;
    case PPP_TERM_REQ:
        switch (fsm->state) {
        case PPP_OPENED:
            send_terminate_ack(fsm, header.id);
            fsm->protocol->closing(fsm);
            fsm->protocol->stopping(fsm);
            PP_fsm_reset(fsm);
            break;
        case PPP_ACK_RECEIVED:
        case PPP_ACK_SENT:
            fsm->state = PPP_REQUEST_SENT;
            /* fall through */
        case PPP_REQUEST_SENT:
        case PPP_TERMINATE_SENT:
        case PPP_CLOSED:
        case PPP_LISTEN:
            send_terminate_ack(fsm, header.id);
            break;
        }
        break;
    case PPP_TERM_ACK:
        switch (fsm->state) {
        case PPP_TERMINATE_SENT:
            PP_stop_timer(&fsm->timer);
            fsm->protocol->stopping(fsm);
            PP_fsm_reset(fsm);
            break;
        case PPP_OPENED:
            fsm->protocol->closing(fsm);
            PP_fsm_reset(fsm);
            if (PP_fsm_sendreq(fsm) == 0) {
                fsm->state = PPP_REQUEST_SENT;
            }
            break;
        case PPP_ACK_SENT:
        case PPP_ACK_RECEIVED:
            fsm->state = PPP_REQUEST_SENT;
            break;
        /* @hack redundant: the empty cases and the default, which leave the
         * state as a switch without them does */
        case PPP_REQUEST_SENT:
            break;
        case PPP_CLOSED:
            break;
        case PPP_LISTEN:
            break;
        default:
            break;
        }
        break;
    case PPP_CODE_REJ:
        fsm->protocol->stopping(fsm);
        PP_fsm_reset(fsm);
        break;
    case PPP_PROTOCOL_REJ:
        break;
    case PPP_ECHO_REQ:
        switch (fsm->state) {
        case PPP_OPENED:
            PP_fsm_send(fsm, PPP_ECHO_REPLY, header.id, packet);
            break;
        case PPP_CLOSED:
        case PPP_LISTEN:
            send_terminate_ack(fsm, header.id);
            break;
        case PPP_REQUEST_SENT:
        case PPP_ACK_RECEIVED:
        case PPP_ACK_SENT:
        case PPP_TERMINATE_SENT:
            break;
        }
        break;
    case PPP_ECHO_REPLY:
        break;
    case PPP_DISCARD_REQ:
        break;
    case PPP_QUALITY_REPORT:
        break;
    default:
        header.length += CONTROL_HEADER_BYTES;
        packet = PP_htoncnf(&header, packet);
        PP_fsm_send(fsm, PPP_CODE_REJ, header.id, packet);
        switch (fsm->state) {
        case PPP_REQUEST_SENT:
        case PPP_ACK_RECEIVED:
        case PPP_ACK_SENT:
        case PPP_OPENED:
            fsm->state = PPP_LISTEN;
            break;
        case PPP_CLOSED:
        case PPP_LISTEN:
        case PPP_TERMINATE_SENT:
            break;
        }
        break;
    }
}
static void fsm_timeout(void *argument)
{
    struct ppp_fsm *fsm = argument;
    switch (fsm->state) {
    case PPP_REQUEST_SENT:
    case PPP_ACK_RECEIVED:
    case PPP_ACK_SENT:
        if (fsm->retries > 0) {
            PP_fsm_sendreq(fsm);
            if (fsm->state == PPP_ACK_RECEIVED) {
                fsm->state = PPP_REQUEST_SENT;
            }
        } else {
            PP_fsm_reset(fsm);
            fsm->fault = PPP_FAULT_FAILED;
        }
        break;
    case PPP_TERMINATE_SENT:
        if (fsm->retries > 0) {
            send_terminate_request(fsm);
        } else {
            fsm->protocol->stopping(fsm);
            PP_fsm_reset(fsm);
            fsm->fault = PPP_FAULT_FAILED;
        }
        break;
    case PPP_CLOSED:
    case PPP_LISTEN:
    case PPP_OPENED:
        break;
    }
}
void PP_fsm_start(struct ppp_fsm *fsm)
{
    if (fsm->context == NULL) {
        return;
    }
    if (!(fsm->flags & (PPP_ACTIVE | PPP_PASSIVE))) {
        return;
    }
    switch (fsm->state) {
    case PPP_CLOSED:
    case PPP_LISTEN:
    case PPP_TERMINATE_SENT:
        fsm->protocol->starting(fsm);
        PP_fsm_reset(fsm);
        if (fsm->flags & PPP_ACTIVE) {
            PP_fsm_sendreq(fsm);
            fsm->state = PPP_REQUEST_SENT;
        }
        break;
    default:
        break;
    }
}
void PP_fsm_down(struct ppp_fsm *fsm)
{
    if (fsm->context == NULL) {
        return;
    }
    switch (fsm->state) {
    case PPP_REQUEST_SENT:
    case PPP_ACK_RECEIVED:
    case PPP_ACK_SENT:
        PP_stop_timer(&fsm->timer);
        PP_fsm_reset(fsm);
        break;
    case PPP_OPENED:
        fsm->protocol->closing(fsm);
        /* fall through */
    case PPP_TERMINATE_SENT:
        PP_fsm_reset(fsm);
        break;
    case PPP_CLOSED:
    case PPP_LISTEN:
        break;
    }
}
void PP_fsm_close(struct ppp_fsm *fsm)
{
    if (fsm->context == NULL) {
        return;
    }
    switch (fsm->state) {
    case PPP_OPENED:
        fsm->protocol->closing(fsm);
        /* fall through */
    case PPP_ACK_SENT:
        fsm->retries = fsm->max_terminate;
        send_terminate_request(fsm);
        fsm->state = PPP_TERMINATE_SENT;
        break;
    case PPP_REQUEST_SENT:
    case PPP_ACK_RECEIVED:
        fsm->retries = 0;
        fsm->state = PPP_TERMINATE_SENT;
        break;
    case PPP_LISTEN:
        fsm->state = PPP_CLOSED;
        break;
    case PPP_TERMINATE_SENT:
    case PPP_CLOSED:
        break;
    }
}
void PP_fsm_init(struct ppp_fsm *fsm)
{
    struct ppp_timer *timer = &fsm->timer;
    fsm->max_requests = fsm->protocol->max_requests;
    fsm->max_naks = fsm->protocol->max_naks;
    fsm->max_terminate = fsm->protocol->max_terminate;
    PP_fsm_reset(fsm);
    timer->callback = fsm_timeout;
    timer->argument = fsm;
    PP_set_timer(timer, fsm->protocol->timeout);
}
void PP_fsm_free(struct ppp_fsm *fsm)
{
    if (fsm->context != NULL) {
        fsm->protocol->release(fsm);
        fsm->context = NULL;
    }
}

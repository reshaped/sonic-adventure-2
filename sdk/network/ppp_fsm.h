/* RFC 1661's option negotiation automaton, which LCP and IPCP run. the
 * session holds an fsm for each protocol, PAP's and CHAP's included. a
 * protocol's struct ppp_protocol gives its numbers and its calls */
#ifndef NETWORK_PPP_FSM_H
#define NETWORK_PPP_FSM_H
#include <sg_xpt.h>
#include "ppp_packet.h"
#include "ppp_system.h"

enum ppp_fsm_state {
    PPP_CLOSED,
    PPP_LISTEN,
    PPP_REQUEST_SENT,
    PPP_ACK_RECEIVED,
    PPP_ACK_SENT,
    PPP_OPENED,
    PPP_TERMINATE_SENT
};
enum ppp_code {
    PPP_CONFIG_REQ = 1,
    PPP_CONFIG_ACK,
    PPP_CONFIG_NAK,
    PPP_CONFIG_REJ,
    PPP_TERM_REQ,
    PPP_TERM_ACK,
    PPP_CODE_REJ,
    PPP_PROTOCOL_REJ,
    PPP_ECHO_REQ,
    PPP_ECHO_REPLY,
    PPP_DISCARD_REQ,
    PPP_QUALITY_REPORT
};
enum ppp_protocol_index { PPP_LCP, PPP_PAP, PPP_IPCP, PPP_CHAP, PPP_PROTOCOLS };
enum ppp_phase { PPP_DEAD, PPP_LINK, PPP_AUTH, PPP_READY, PPP_TERMINATING };
/* an fsm's flags, and the session's for the first two. PPP_AUTH_LOCAL holds
 * while this end has yet to authenticate the peer. PPP_AUTH_REMOTE holds
 * while the peer has yet to authenticate this end. an fsm with PPP_ACTIVE
 * sends its request as it starts. one with PPP_PASSIVE alone waits for the
 * peer's */
enum {
    PPP_AUTH_LOCAL = 0x10,
    PPP_AUTH_REMOTE = 0x20,
    PPP_PASSIVE = 0x40,
    PPP_ACTIVE = 0x80
};
/* the fault an fsm records as it gives up. PPP_FAULT_FAILED marks retries
 * run out or a failed send. PPP_FAULT_REFUSED marks authentication refused */
enum { PPP_FAULT_NONE, PPP_FAULT_FAILED, PPP_FAULT_REFUSED };

struct ppp_session;
struct ppp_fsm;
/* length counts the whole packet, the header's CONTROL_HEADER_BYTES
 * included */
#define CONTROL_HEADER_BYTES 4
struct ppp_control_header {
    Uint8 code, id;
    Uint16 length;
};
struct ppp_option_header {
    Uint8 type, length;
};
typedef void ppp_event(struct ppp_fsm *);
/* a protocol's check of a packet received, which returns 0 to accept it */
typedef short ppp_check(struct ppp_fsm *, struct ppp_control_header *,
                        struct ppp_buffer *);
/* a protocol as its fsm runs it. the fsm calls starting as it sets out to
 * negotiate, and opening once it opens. it calls closing as it leaves
 * PPP_OPENED, and stopping as it terminates. reset runs within PP_fsm_reset,
 * and release frees context */
struct ppp_protocol {
    const char *name;
    Uint16 protocol, receive_mask;
    Uint8 index, max_requests, max_naks, max_terminate;
    Uint32 timeout;
    ppp_event *release, *reset, *starting, *opening, *closing, *stopping;
    struct ppp_buffer *(*make_request)(struct ppp_fsm *);
    ppp_check *request, *ack, *nak, *reject;
};
/* last_id is the id of the last request sent. retries counts the requests
 * left to send before the fsm gives up. nak_retries counts the naks left to
 * send before it rejects instead. IPCP's and LCP's request checks take one
 * even where the send fails. once none is left, the reject they send holds
 * this end's values. RFC 1661 has a reject hold the peer's unchanged */
struct ppp_fsm {
    Uint8 state, last_id, flags, retries;
    Uint8 max_requests, max_terminate, nak_retries, max_naks;
    Sint16 fault;
    Uint8 reserved, ack_count;
    struct ppp_session *session;
    struct ppp_timer timer;
    const struct ppp_protocol *protocol;
    void *context; /* the protocol's own state, such as a struct ppp_lcp */
};
/* id is the next id the session hands out */
struct ppp_session {
    Uint8 phase, id, flags, trace;
    struct ppp_fsm fsm[PPP_PROTOCOLS];
    Sint32 up_since;
    char *peer_name;
    Sint32 octets_sent, packets_sent;
    Sint16 control_sent[PPP_PROTOCOLS];
    Sint16 send_status, receive_status;
    Sint32 octets_received, packets_received;
    union {
        struct {
            Uint16 lcp, pap, ipcp, chap;
        } protocol;
        Uint16 count[PPP_PROTOCOLS];
    } received;
    Uint16 unknown_protocol, discarded, overflow, receive_errors,
        allocation_errors;
};
struct ppp_lcp_options {
    Sint16 negotiate, mru;
    Uint32 accm;
    Uint16 authentication, encryption;
    Uint32 magic_number, report_period;
    Uint8 chap_algorithm, chap_any;
};
struct ppp_lcp_side {
    Sint16 will_negotiate;
    struct ppp_lcp_options want, work;
};
struct ppp_lcp {
    struct ppp_lcp_side local, remote;
};

/* put header's four bytes ahead of packet's data, which must have room for
 * them, and return packet */
struct ppp_buffer *PP_htoncnf(const struct ppp_control_header *header,
                              struct ppp_buffer *packet);
/* take a control header, or an option's, off the front of *packet. each
 * returns 0, or -1 where too few bytes are left */
short PP_ntohcnf(struct ppp_control_header *header, struct ppp_buffer **packet);
short PP_ntohopt(struct ppp_option_header *option, struct ppp_buffer **packet);
/* the event that does nothing, and the check that accepts everything */
void PP_fsm_no_action(struct ppp_fsm *fsm);
short PP_fsm_no_check(struct ppp_fsm *fsm, struct ppp_control_header *header,
                      struct ppp_buffer *packet);
void PP_fsm_timer(struct ppp_fsm *fsm);
/* send a packet of code holding data, or empty where data is NULL. the
 * result is ppp_output's. it is -1 for a code not named below, or where data
 * has no room for the header.
 *
 * PPP_CONFIG_REQ, PPP_TERM_REQ and PPP_ECHO_REQ take the session's next id
 * and record it in last_id. PPP_PROTOCOL_REJ and PPP_DISCARD_REQ take the
 * next id alone. PPP_CONFIG_ACK, PPP_CONFIG_NAK, PPP_CONFIG_REJ,
 * PPP_TERM_ACK, PPP_CODE_REJ and PPP_ECHO_REPLY take id.
 *
 * PPP_ECHO_REQ, PPP_ECHO_REPLY and PPP_DISCARD_REQ need LCP's fsm, whose
 * context is a struct ppp_lcp. they write its local magic number over the
 * first four bytes at data's data. an empty packet goes without it */
short PP_fsm_send(struct ppp_fsm *fsm, Uint8 code, Uint8 id,
                  struct ppp_buffer *data);
/* where a retry is left, take it, restart the timer and send the protocol's
 * request. the result is PP_fsm_send's, or -1 with none left */
short PP_fsm_sendreq(struct ppp_fsm *fsm);
/* refill retries and nak_retries, and run the protocol's reset. an fsm with
 * PPP_ACTIVE or PPP_PASSIVE goes to PPP_LISTEN, and any other to
 * PPP_CLOSED */
void PP_fsm_reset(struct ppp_fsm *fsm);
/* run the automaton on a control packet received */
void PP_fsm_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet);
/* PP_fsm_start, PP_fsm_down and PP_fsm_close do nothing where context is
 * NULL. PP_fsm_start also does nothing to an fsm with neither PPP_ACTIVE nor
 * PPP_PASSIVE. nor does it to one that has sent its request or opened. any
 * other calls starting and resets, and with PPP_ACTIVE sends its request */
void PP_fsm_start(struct ppp_fsm *fsm);
/* the link is down: an fsm past PPP_LISTEN resets, calling closing first
 * where it has opened. one that awaits a reply to its request stops its
 * timer too */
void PP_fsm_down(struct ppp_fsm *fsm);
/* an opened fsm calls closing. it and one in PPP_ACK_SENT send a terminate
 * request, with max_terminate retries. one in PPP_REQUEST_SENT or
 * PPP_ACK_RECEIVED sends none, and has no retry left when its timer runs
 * out. each goes to PPP_TERMINATE_SENT, and one in PPP_LISTEN to
 * PPP_CLOSED */
void PP_fsm_close(struct ppp_fsm *fsm);
/* take the fsm's limits and timeout from its protocol, and reset it */
void PP_fsm_init(struct ppp_fsm *fsm);
/* run the protocol's release and set context NULL, where it isn't */
void PP_fsm_free(struct ppp_fsm *fsm);
/* clear PPP_ACTIVE and PPP_PASSIVE, and close the fsm. the result is 0 */
short PP_doppp_close(struct ppp_fsm *fsm);
/* make the fsm PPP_ACTIVE alone, and move it from PPP_CLOSED to PPP_LISTEN.
 * the result is 0 */
short PP_doppp_active(struct ppp_fsm *fsm);
/* send packet in a frame of protocol. interface and destination are unread.
 * the result is 0, or -1 */
short ppp_output(void *interface, void *destination, Uint16 protocol,
                 struct ppp_buffer *packet);
#endif

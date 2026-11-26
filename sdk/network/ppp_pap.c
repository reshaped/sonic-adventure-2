#include <sg_xpt.h>
#include <string.h>
#include "ppp_fsm.h"
#include "ppp_link.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_system.h"

/* PAP's codes. past the control header, a request holds the login's length,
 * the login, the password's length and the password. an ack or a nak holds
 * its message's length and the message */
enum { PAP_REQUEST = 1, PAP_ACK, PAP_NAK };
/* the credentials PP_dopap_user sets, and the status text. pap_timeout sets
 * message to pap_request_timeout or pap_retries_exceeded, and a reply to the
 * peer's message */
struct pap_context {
    char *login, *password, *message;
};

static ppp_event pap_release, pap_close_link;
/* end the role flag marks, PPP_AUTH_LOCAL or PPP_AUTH_REMOTE, in PAP's fsm and
 * the session. the fsm opens once it holds neither. ppp_ready then starts
 * IPCP where the session holds neither */
static void pap_authenticated(struct ppp_fsm *fsm, Uint8 flag);
#ifdef NONMATCHING
/* the longest login, password or message, which a field's one-byte length
 * counts */
enum { PAP_FIELD_LIMIT = 255 };
#else
enum { PEER_NAME_BYTES = 33, PEER_MESSAGE_BYTES = 64 };
static ppp_check pap_request, pap_reply;
static Uint8 pull_word[4], pull_byte;
#endif
static struct pap_context pap_state;
#ifdef NONMATCHING
/* the login of the peer this end accepted, and the message of the peer's
 * reply, PAP_FIELD_LIMIT bytes and a NUL each. the matching side's three hold
 * less than a field can */
static char peer_login[PAP_FIELD_LIMIT + 1], peer_message[PAP_FIELD_LIMIT + 1];
#else
static char peer_login[PEER_NAME_BYTES], peer_password[PEER_NAME_BYTES],
    peer_message[PEER_MESSAGE_BYTES];
#endif

#pragma section ATA
static char pap_name[4];
static struct ppp_protocol pap_protocol = {
#include "network/ppp_pap/pap_protocol.inc"
};
/* the welcome and the refusal go to the peer. each one's leading space
 * carries the length of the text after it */
static char pap_welcome[12] = " Welcome";
static char pap_invalid_credentials[32] = " Invalid username or password";
static char pap_request_timeout[16] = "Request timeout";
static char pap_retries_exceeded[24] = "Request retry exceeded";
static char pap_name[4] = {
#include "network/ppp_pap/pap_name.inc"
};
#pragma section

#ifdef NONMATCHING
/* credential's length, or -1 where it is NULL or longer than PAP_FIELD_LIMIT.
 * this side's PAP code refuses such a credential, where the matching side
 * takes any */
static int pap_credential_length(const char *credential)
{
    int length;
    if (!credential) {
        return -1;
    }
    for (length = 0; length <= PAP_FIELD_LIMIT; length++) {
        if (credential[length] == '\0') {
            return length;
        }
    }
    return -1;
}
#endif

short PP_dopap_user(char *login, char *password, struct ppp_fsm *fsm)
{
    struct pap_context *pap = fsm->context;
#ifdef NONMATCHING
    /* this side refuses a login or password that is missing, empty or longer
     * than PAP_FIELD_LIMIT */
    if (pap_credential_length(login) <= 0 ||
        pap_credential_length(password) <= 0) {
        pap->login = NULL;
        pap->password = NULL;
        return -1;
    }
#endif
    pap->login = login;
    pap->password = password;
    return 0;
}

short PP_pap_monitor(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    fsm->fault = PPP_FAULT_NONE;
    while (fsm->state != PPP_OPENED) {
        if (ppp_is_abort() || fsm->fault != PPP_FAULT_NONE ||
            session->phase == PPP_TERMINATING) {
            PP_fsm_close(fsm);
            pap_close_link(fsm);
            if (fsm->fault == PPP_FAULT_REFUSED) {
                fsm->fault = PPP_FAULT_NONE;
                return -1;
            }
            fsm->fault = PPP_FAULT_NONE;
            return -2;
        }
        PP_OS_CriticalUnLock();
        PP_OS_Yield();
        PP_timerproc();
        PP_OS_CriticalLock();
    }
    fsm->fault = PPP_FAULT_NONE;
    return 0;
}

#ifdef NONMATCHING
/* this side sends no request without a login and a password of 1 to
 * PAP_FIELD_LIMIT bytes, and sets the packet's length where the matching side
 * adds to it */
struct ppp_buffer *PP_pap_makereq(struct ppp_fsm *fsm)
{
    struct pap_context *pap = fsm->context;
    struct ppp_buffer *packet;
    int login_length = pap_credential_length(pap->login);
    int password_length = pap_credential_length(pap->password);
    Uint8 *output;
    if (login_length <= 0 || password_length <= 0) {
        return NULL;
    }
    packet = PP_alloc_mbuf_work(0);
    if (!packet) {
        return NULL;
    }
    output = packet->data;
    *output++ = login_length;
    memcpy(output, pap->login, login_length);
    output += login_length;
    *output++ = password_length;
    memcpy(output, pap->password, password_length);
    packet->length = login_length + password_length + 2;
    return packet;
}
#else
/* the peer's login and password as this end checks them: it accepts every
 * pair, with 0 */
static short pap_check_credentials(char *login, char *password)
{
    return 0;
}

struct ppp_buffer *PP_pap_makereq(struct ppp_fsm *fsm)
{
    struct pap_context *pap = fsm->context;
    struct ppp_buffer *packet;
    Uint8 *output;
    int login_length;
    int password_length;
    if (!pap->login || !pap->password) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((packet = PP_alloc_mbuf_work(0)) == NULL) {
        return NULL;
    }
    output = packet->data;
    login_length = strlen(pap->login);
    *output++ = login_length;
    memcpy(output, pap->login, login_length);
    output += login_length;
    password_length = strlen(pap->password);
    *output++ = password_length;
    memcpy(output, pap->password, password_length);
    packet->length += login_length + password_length + 2;
    return packet;
}
#endif

void pap_close_link(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    PP_fsm_close(&session->fsm[PPP_LCP]);
}

void pap_authenticated(struct ppp_fsm *fsm, Uint8 flag)
{
    struct ppp_session *session = fsm->session;
    fsm->flags &= ~flag;
    if (!(fsm->flags & (PPP_AUTH_LOCAL | PPP_AUTH_REMOTE))) {
        fsm->state = PPP_OPENED;
    }
#ifdef NONMATCHING
    /* this side leaves the timer running while PPP_AUTH_REMOTE holds, since it
     * sends this end's request again until the peer answers. the matching side
     * stops it whichever role ends */
    if (!(fsm->flags & PPP_AUTH_REMOTE)) {
        PP_stop_timer(&fsm->timer);
    }
#else
    PP_stop_timer(&fsm->timer);
#endif
    session->flags &= ~flag;
    ppp_ready(session);
}

#ifdef NONMATCHING
/* this side repairs the bugs tagged on the matching side below: it reads a
 * packet within its bounds, into buffers that hold any field. it also checks
 * the peer's login and password against this end's, where the matching side
 * accepts every pair. it answers a request again after it has accepted the
 * peer, and takes a reply only while the peer has yet to authenticate this
 * end */
static int credential_matches(const char *expected, const Uint8 *bytes,
                              int length)
{
    int index;
    unsigned int difference = 0;
    if (length == 0 || pap_credential_length(expected) != length) {
        return 0;
    }
    for (index = 0; index < length; index++) {
        difference |= (Uint8)expected[index] ^ bytes[index];
    }
    return difference == 0;
}

static void pap_authenticate(struct ppp_fsm *fsm, Uint8 id,
                             const struct ppp_buffer *packet)
{
    struct pap_context *pap = fsm->context;
    const Uint8 *data = packet->data;
    struct ppp_buffer *reply;
    const char *message;
    Uint8 login_length;
    Uint8 password_length;
    int accepted;
    int reply_length;
    int awaiting_peer = fsm->flags & PPP_AUTH_LOCAL;
    /* a request after this end accepted the peer repeats one whose ack was
     * lost. it is checked and answered again, and changes nothing */
    if ((!awaiting_peer && (fsm->session->peer_name != peer_login ||
                            (fsm->session->phase != PPP_AUTH &&
                             fsm->session->phase != PPP_READY))) ||
        packet->length < 2) {
        return;
    }
    login_length = data[0];
    if (login_length + 2 > packet->length) {
        return;
    }
    password_length = data[login_length + 1];
    if (login_length + password_length + 2 != packet->length) {
        return;
    }
    accepted = credential_matches(pap->login, data + 1, login_length) &
               credential_matches(pap->password, data + login_length + 2,
                                  password_length);
    message = accepted ? pap_welcome : pap_invalid_credentials;
    reply_length = strlen(message);
    if (accepted && awaiting_peer) {
        memcpy(peer_login, data + 1, login_length);
        peer_login[login_length] = '\0';
    }
    reply = PP_qdata((const Uint8 *)message, reply_length);
    if (!reply) {
        return;
    }
    reply->data[0] = reply_length - 1;
    if (PP_fsm_send(fsm, accepted ? PAP_ACK : PAP_NAK, id, reply) < 0 ||
        !awaiting_peer) {
        return;
    }
    if (accepted) {
        fsm->session->peer_name = peer_login;
        pap_authenticated(fsm, PPP_AUTH_LOCAL);
    } else if (fsm->nak_retries) {
        fsm->nak_retries--;
    } else {
        pap_close_link(fsm);
    }
}

void PP_pap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet)
{
    struct pap_context *pap = fsm->context;
    struct ppp_control_header header;
    Uint8 length;
    if (PP_ntohcnf(&header, &packet) < 0 ||
        header.length < CONTROL_HEADER_BYTES ||
        header.length - CONTROL_HEADER_BYTES > packet->length) {
        return;
    }
    packet->length = header.length - CONTROL_HEADER_BYTES;
    if (header.code == PAP_REQUEST) {
        pap_authenticate(fsm, header.id, packet);
        return;
    }
    if ((header.code != PAP_ACK && header.code != PAP_NAK) ||
        !(fsm->flags & PPP_AUTH_REMOTE) || header.id != fsm->last_id ||
        packet->length == 0) {
        return;
    }
    length = packet->data[0];
    if (length + 1 != packet->length) {
        return;
    }
    memcpy(peer_message, packet->data + 1, length);
    peer_message[length] = '\0';
    pap->message = peer_message;
    if (header.code == PAP_ACK) {
        pap_authenticated(fsm, PPP_AUTH_REMOTE);
    } else {
        PP_stop_timer(&fsm->timer);
        pap->login = NULL;
        pap->password = NULL;
        fsm->fault = PPP_FAULT_REFUSED;
        fsm->state = PPP_CLOSED;
    }
}
#else
/* answer the peer's request with an ack or a nak, and return 0 for an ack.
 * the peer's login becomes the session's peer_name. a nak takes one of
 * nak_retries, and closes the link once none is left */
short pap_request(struct ppp_fsm *fsm, struct ppp_control_header *header,
                  struct ppp_buffer *packet)
{
    char *login = NULL;
    char *password = NULL;
    char *cursor;
    char *message;
    short length;
    short count;
    short code;
    int message_length;
    struct ppp_buffer *reply;

    length = PPP_PULL_BYTE(&packet);
    if (length != -1) {
        cursor = login = peer_login;
        count = length;
        /* @bug memory: copies a login of up to 255 bytes into peer_login's
         * PEER_NAME_BYTES, at 0x0c9e6c72 */
        while (count-- > 0) {
            *cursor++ = pull_octet(&packet);
        }
        *cursor = '\0';
    }
    length = PPP_PULL_BYTE(&packet);
    if (length != -1) {
        cursor = password = peer_password;
        count = length;
        /* @bug memory: copies a password of up to 255 bytes into
         * peer_password's PEER_NAME_BYTES, at 0x0c9e6d3a */
        while (count-- > 0) {
            *cursor++ = pull_octet(&packet);
        }
        *cursor = '\0';
    }
    if (!pap_check_credentials(login, password)) {
        fsm->session->peer_name = login;
        message = pap_welcome;
        code = PAP_ACK;
    } else {
        message = pap_invalid_credentials;
        code = PAP_NAK;
    }
    message_length = strlen(message);
    reply = PP_qdata((const Uint8 *)message, message_length);
    /* @hack type: the length converted to char, where the difference alone
     * would do */
    reply->data[0] = (char)(message_length - 1);
    PP_fsm_send(fsm, code, header->id, reply);
    if (code == PAP_NAK) {
        if (fsm->nak_retries > 0) {
            fsm->nak_retries--;
        } else {
            pap_close_link(fsm);
        }
    }
    return code != PAP_ACK;
}

/* keep the message of the peer's ack or nak, for the context's message. it
 * returns 0 for a reply to this end's last request whose message is whole, or
 * that has none */
short pap_reply(struct ppp_fsm *fsm, struct ppp_control_header *header,
                struct ppp_buffer *packet)
{
    struct pap_context *pap = fsm->context;
    short length;
    short available;
    short copied;

    if (header->id != fsm->last_id) {
        return -1;
    }
    length = PPP_PULL_BYTE(&packet);
    if (length != -1) {
        if (length) {
            available = PP_len_p(packet);
            /* @bug memory: copies a message of up to 255 bytes into
             * peer_message's PEER_MESSAGE_BYTES, by the call at 0x0c9e6e34 */
            copied = PP_dqdata(packet, (Uint8 *)peer_message, length);
            peer_message[copied] = '\0';
            pap->message = peer_message;
            return copied < length || length < available;
        }
        return 0;
    }
    return -1;
}

void PP_pap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet)
{
    struct pap_context *pap = fsm->context;
    struct ppp_control_header header;

    PP_ntohcnf(&header, &packet);
    header.length -= CONTROL_HEADER_BYTES;
    PP_trim_mbuf(&packet, header.length);
    switch (header.code) {
    case PAP_REQUEST:
        if (pap_request(fsm, &header, packet) == 0) {
            pap_authenticated(fsm, PPP_AUTH_LOCAL);
        }
        break;
    case PAP_ACK:
        if (pap_reply(fsm, &header, packet) == 0) {
            pap_authenticated(fsm, PPP_AUTH_REMOTE);
        }
        break;
    case PAP_NAK:
        if (pap_reply(fsm, &header, packet) == 0) {
            PP_stop_timer(&fsm->timer);
            pap->login = NULL;
            pap->password = NULL;
            fsm->fault = PPP_FAULT_REFUSED;
            fsm->state = PPP_CLOSED;
        }
        break;
    }
}
#endif

static void pap_timeout(void *argument)
{
    struct ppp_fsm *fsm = argument;
    struct pap_context *pap = fsm->context;
    if (fsm->retries > 0) {
        pap->message = pap_request_timeout;
        PP_fsm_sendreq(fsm);
    } else {
        pap->message = pap_retries_exceeded;
        pap_close_link(fsm);
        fsm->fault = PPP_FAULT_FAILED;
    }
}

void PP_pap_down(struct ppp_fsm *fsm)
{
    struct pap_context *pap = fsm->context;
    if (pap) {
        fsm->flags = 0;
        switch (fsm->state) {
        case PPP_REQUEST_SENT:
            PP_stop_timer(&fsm->timer);
            /* fall through */
        case PPP_OPENED:
        case PPP_LISTEN:
        case PPP_TERMINATE_SENT:
            fsm->state = PPP_CLOSED;
            break;
        case PPP_CLOSED:
            break;
        }
    }
}

void pap_release(struct ppp_fsm *fsm)
{
}

void PP_pap_init(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_PAP];
    struct ppp_timer *timer;
    if (!fsm->context) {
        fsm->session = session;
        fsm->protocol = &pap_protocol;
        fsm->context = &pap_state;
        fsm->max_requests = fsm->protocol->max_requests;
        fsm->max_naks = fsm->protocol->max_naks;
        fsm->max_terminate = fsm->protocol->max_terminate;
        fsm->state = PPP_CLOSED;
        fsm->retries = fsm->max_requests;
        fsm->nak_retries = fsm->max_naks;
        timer = &fsm->timer;
        timer->callback = pap_timeout;
        timer->argument = fsm;
        PP_set_timer(timer, fsm->protocol->timeout);
    }
}

void PP_pap_local(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_PAP];
    fsm->state = PPP_LISTEN;
    fsm->flags |= PPP_AUTH_LOCAL;
    session->flags |= PPP_AUTH_LOCAL;
    fsm->retries = fsm->max_requests;
}
short PP_pap_remote(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_PAP];
    fsm->state = PPP_REQUEST_SENT;
    fsm->flags |= PPP_AUTH_REMOTE;
    session->flags |= PPP_AUTH_REMOTE;
    return 0;
}

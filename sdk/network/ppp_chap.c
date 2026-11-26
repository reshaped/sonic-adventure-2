#include <machine.h>
#include <sg_xpt.h>
#include <string.h>
#include "md5.h"
#include "ppp_fsm.h"
#include "ppp_link.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_system.h"

enum { CHAP_CHALLENGE = 1, CHAP_RESPONSE, CHAP_SUCCESS, CHAP_FAILURE };
enum { CHAP_NAME_BYTES = 128, CHAP_CHALLENGE_BYTES = 128 };
/* the matching side's challenge holds CHALLENGE_MIN_BYTES random bytes, and
 * up to CHALLENGE_SPREAD more */
enum { CHALLENGE_MIN_BYTES = 16, CHALLENGE_SPREAD = 0x1f };
/* the credentials, and the status text: one of the two messages below, which
 * chap_timeout sets */
struct chap_context {
    char *login, *secret, *message;
#ifdef NONMATCHING
    /* this side's state for the challenge it sent, and for the response it
     * waits on */
    Uint8 challenge_pending, challenge_id, response_pending, response_id;
    Uint32 challenge_count;
#endif
};

static struct ppp_buffer *chap_make_challenge(struct ppp_fsm *fsm);
/* close CHAP and LCP */
static ppp_event chap_close_link;
/* CHAP's timer callback, whose argument is CHAP's fsm. it takes one of the
 * fsm's retries while any is left. after the last it closes CHAP and LCP and
 * marks the fsm failed */
static void chap_timeout(void *argument);
#ifdef NONMATCHING
#else
/* @hack spelling: SHC's _builtin_strcpy, where strcpy would do */
#define COPY_STRING _builtin_strcpy
static Uint8 pull_word[4], pull_byte;
#endif
/* the challenge this end sent, which a response is checked against. the
 * matching side keeps the packet as sent, from the byte that counts the
 * challenge's value, and the GCC side the value alone */
static Uint16 challenge_length;
static Uint8 challenge_value[CHAP_CHALLENGE_BYTES];
static struct chap_context chap_state;
static char login_buffer[CHAP_NAME_BYTES], secret_buffer[CHAP_NAME_BYTES];
#pragma section ATA
static char chap_name[5];
static struct ppp_protocol chap_protocol = {
#include "network/ppp_chap/chap_protocol.inc"
};
static char welcome[12] = {
#include "network/ppp_chap/welcome.inc"
};
static char chap_request_timeout[16] = {
#include "network/ppp_chap/chap_request_timeout.inc"
};
static char chap_retries_exceeded[24] = {
#include "network/ppp_chap/chap_retries_exceeded.inc"
};
static char chap_name[5] = {
#include "network/ppp_chap/chap_name.inc"
};
#pragma section

#ifdef NONMATCHING
/* this side copies a credential only where it fits its buffer, and its
 * challenge is the digest of a nonce, where the matching side's is 16 to 47
 * random bytes */
static int copy_credential(char destination[CHAP_NAME_BYTES],
                           const char *source)
{
    int length;
    if (!source) {
        return -1;
    }
    for (length = 0; length < CHAP_NAME_BYTES; length++) {
        if (source[length] == '\0') {
            if (!length) {
                return -1;
            }
            memcpy(destination, source, length + 1);
            return 0;
        }
    }
    return -1;
}
static void chap_shutdown(struct ppp_fsm *fsm)
{
    struct chap_context *chap = fsm->context;
    chap->challenge_pending = 0;
    chap->response_pending = 0;
    PP_stop_timer(&fsm->timer);
    chap_close_link(fsm);
}
static void chap_digest(Uint8 digest[MD5_DIGEST_BYTES], Uint8 id,
                        const char *secret, const Uint8 *challenge,
                        Uint16 length)
{
    MD5_CTX hash;
    MD5Init(&hash);
    MD5Update(&hash, &id, 1);
    MD5Update(&hash, (const Uint8 *)secret, strlen(secret));
    MD5Update(&hash, challenge, length);
    MD5Final(digest, &hash);
}
/* the challenge is the digest of the id, the secret and a nonce. the nonce
 * holds the time, a count of the challenges made, then random bytes */
enum { NONCE_COUNT = 4, NONCE_RANDOM = 8, NONCE_BYTES = 24 };
static struct ppp_buffer *chap_make_challenge(struct ppp_fsm *fsm)
{
    struct chap_context *chap = fsm->context;
    struct ppp_buffer *packet;
    Uint8 nonce[NONCE_BYTES];
    int index;
    int name_length;
    if (!chap->login || !chap->secret || !(fsm->flags & PPP_AUTH_LOCAL)) {
        return NULL;
    }
    packet = PP_alloc_mbuf_work(0);
    if (!packet) {
        return NULL;
    }
    _PP_srandom();
    PP_put32(nonce, PP_Clock());
    PP_put32(nonce + NONCE_COUNT, ++chap->challenge_count);
    for (index = NONCE_RANDOM; index < (int)sizeof(nonce); index++) {
        nonce[index] = _PP_random();
    }
    chap->challenge_id = fsm->session->id;
    challenge_length = MD5_DIGEST_BYTES;
    chap_digest(challenge_value, chap->challenge_id, chap->secret, nonce,
                sizeof(nonce));
    packet->data[0] = challenge_length;
    memcpy(packet->data + 1, challenge_value, challenge_length);
    name_length = strlen(chap->login);
    memcpy(packet->data + 1 + challenge_length, chap->login, name_length);
    packet->length = 1 + challenge_length + name_length;
    chap->challenge_pending = 1;
    return packet;
}
#else
/* a challenge: a random value after its length, then this end's login */
struct ppp_buffer *chap_make_challenge(struct ppp_fsm *fsm)
{
    struct chap_context *chap = fsm->context;
    struct ppp_buffer *packet;
    Uint8 *output;
    int length;
    int i;

    if (chap->login == NULL || chap->secret == NULL) {
        return NULL;
    }
    packet = PP_alloc_mbuf_work(0);
    if (packet == NULL) {
        return NULL;
    }
    output = packet->data;
    _PP_srandom();
    challenge_length = (_PP_random() & CHALLENGE_SPREAD) + CHALLENGE_MIN_BYTES;
    *output++ = challenge_length;
    for (i = 0; i < challenge_length; i++) {
        *output++ = _PP_random();
    }
    length = strlen(chap->login);
    if (length > 0) {
        COPY_STRING((char *)output, chap->login);
        output += length;
        *output++ = '\0';
        *output = '\0';
    }
    /* the 2 are the byte that counts the value and a NUL after the login */
    packet->length += length + challenge_length + 2;
    length = packet->length;
    if ((unsigned int)length > sizeof(challenge_value)) {
        length = sizeof(challenge_value);
    }
    memcpy(challenge_value, packet->data, length);
    return packet;
}
#endif

void chap_close_link(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
#ifdef NONMATCHING
    /* this side closes the fsm it is passed. each caller passes the session's
     * CHAP fsm, the one the matching side closes */
    PP_fsm_close(fsm);
#else
    PP_fsm_close(&session->fsm[PPP_CHAP]);
#endif
    PP_fsm_close(&session->fsm[PPP_LCP]);
}

#ifdef NONMATCHING
/* this side repairs the bugs tagged on the matching side below. it reads a
 * challenge or a response within the packet's bounds, and accepts only the
 * response whose digest and name are this end's. an accepted response ends
 * PPP_AUTH_LOCAL, and the peer's success PPP_AUTH_REMOTE */
static void chap_authenticated(struct ppp_fsm *fsm, Uint8 flag)
{
    fsm->flags &= ~flag;
    fsm->session->flags &= ~flag;
    if (!(fsm->flags & (PPP_AUTH_LOCAL | PPP_AUTH_REMOTE))) {
        PP_stop_timer(&fsm->timer);
        fsm->state = PPP_OPENED;
    }
    ppp_ready(fsm->session);
}
static void chap_respond(struct ppp_fsm *fsm, Uint8 id, const Uint8 *payload,
                         Uint16 length)
{
    struct chap_context *chap = fsm->context;
    struct ppp_buffer *response;
    Uint8 digest[MD5_DIGEST_BYTES];
    unsigned int name_length;
    if (!(fsm->flags & PPP_AUTH_REMOTE) || length < 2 || !payload[0] ||
        payload[0] + 1 > length) {
        return;
    }
    chap_digest(digest, id, chap->secret, payload + 1, payload[0]);
    name_length = strlen(chap->login);
    response = PP_alloc_mbuf_work(0);
    if (!response) {
        return;
    }
    response->data[0] = sizeof(digest);
    memcpy(response->data + 1, digest, sizeof(digest));
    memcpy(response->data + 1 + sizeof(digest), chap->login, name_length);
    response->length = 1 + sizeof(digest) + name_length;
    if (PP_fsm_send(fsm, CHAP_RESPONSE, id, response) >= 0) {
        chap->response_id = id;
        chap->response_pending = 1;
        PP_start_timer(&fsm->timer);
    }
}
static void chap_verify(struct ppp_fsm *fsm, Uint8 id, const Uint8 *payload,
                        Uint16 length)
{
    struct chap_context *chap = fsm->context;
    Uint8 expected[MD5_DIGEST_BYTES];
    struct ppp_buffer *reply;
    unsigned int difference = 0;
    unsigned int index;
    const char *message;
    if (!(fsm->flags & PPP_AUTH_LOCAL) || !chap->challenge_pending ||
        id != chap->challenge_id || length < 1 + MD5_DIGEST_BYTES ||
        payload[0] != MD5_DIGEST_BYTES) {
        return;
    }
    chap_digest(expected, id, chap->secret, challenge_value, challenge_length);
    for (index = 0; index < sizeof(expected); index++) {
        difference |= expected[index] ^ payload[1 + index];
    }
    if (length - (1 + MD5_DIGEST_BYTES) != strlen(chap->login) ||
        memcmp(payload + 1 + MD5_DIGEST_BYTES, chap->login,
               strlen(chap->login))) {
        difference |= 1;
    }
    chap->challenge_pending = 0;
    message = difference ? "Authentication failed" : welcome;
    reply = PP_qdata((const Uint8 *)message, strlen(message));
    if (!reply || PP_fsm_send(fsm, difference ? CHAP_FAILURE : CHAP_SUCCESS, id,
                              reply) < 0) {
        chap_shutdown(fsm);
        fsm->fault = PPP_FAULT_FAILED;
        return;
    }
    if (difference) {
        chap_shutdown(fsm);
        fsm->fault = PPP_FAULT_REFUSED;
    } else {
        fsm->session->peer_name = chap->login;
        chap_authenticated(fsm, PPP_AUTH_LOCAL);
    }
}
#else
/* end the role flag marks in CHAP's fsm and the session, and stop the timer.
 * the fsm opens once PPP_AUTH_LOCAL is clear */
static void chap_authenticated(struct ppp_fsm *fsm, int flag)
{
    struct ppp_session *session = fsm->session;

    PP_stop_timer(&fsm->timer);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (!((fsm->flags &= ~flag) & PPP_AUTH_LOCAL)) {
        fsm->state = PPP_OPENED;
    }
    session->flags &= ~flag;
    if (flag == PPP_AUTH_LOCAL) {
        session->flags &= ~PPP_AUTH_REMOTE;
    }
    ppp_ready(session);
}

/* answer the peer's challenge with the MD5 digest of the id, the secret and
 * the challenge's value, then this end's login */
static void chap_respond(struct ppp_fsm *fsm, struct ppp_control_header *header,
                         struct ppp_buffer *packet)
{
    struct chap_context *chap = fsm->context;
    struct ppp_buffer *reply;
    Uint8 *value = NULL;
    Uint8 *cursor;
    Uint8 *name_cursor;
    short length;
    short value_length;
    short name_length;
    int i;
    Uint16 secret_length;
    short login_length;
    MD5_CTX hash;
    Uint8 name[33];
    Uint8 value_bytes[64];
    Uint8 hashed[129];
    Uint8 response[129];

    length = header->length;
    value_length = PPP_PULL_BYTE(&packet);
    if (value_length != -1) {
        cursor = value = value_bytes;
        /* @bug memory: copies a challenge's value of up to 255 bytes into
         * value_bytes's 64, at 0x0c9e41e4 */
        for (i = 0; i < value_length; i++) {
            *cursor++ = pull_octet(&packet);
        }
    }
    name_length = length - value_length - 1;
    if (name_length < 0) {
        name_length = 0;
    }
    if (name_length != 0) {
        name_cursor = name;
        /* @bug memory: copies the peer's name, every byte the header's length
         * leaves after the value, into name's 33, at 0x0c9e4280 */
        for (i = 0; i < name_length; i++) {
            *name_cursor++ = pull_octet(&packet);
        }
        *name_cursor = '\0';
    }

    hashed[0] = header->id;
    COPY_STRING((char *)&hashed[1], chap->secret);
    secret_length = strlen(chap->secret);
    login_length = strlen(chap->login);
    /* @bug memory: copies from a null value, with a length of -1, where the
     * packet held no byte for the value's length, by the call at
     * 0x0c9e42b6 */
    memcpy(&hashed[secret_length + 1], value, value_length);

    memset(response, 0, sizeof(response));
    response[0] = MD5_DIGEST_BYTES;
    MD5Init(&hash);
    MD5Update(&hash, hashed, secret_length + value_length + 1);
    MD5Final(&response[1], &hash);
    memcpy(&response[1 + MD5_DIGEST_BYTES], chap->login, login_length);

    reply = PP_qdata(response, login_length + 1 + MD5_DIGEST_BYTES);
    PP_fsm_send(fsm, CHAP_RESPONSE, header->id, reply);

    if (fsm->nak_retries > 0) {
        fsm->nak_retries--;
    } else {
        chap_close_link(fsm);
    }
}

/* check the peer's response to this end's challenge, and answer it. it
 * returns 0 for a response it accepts, or -1 for one to another challenge */
static short chap_verify(struct ppp_fsm *fsm, struct ppp_control_header *header,
                         struct ppp_buffer *packet)
{
    struct chap_context *chap = fsm->context;
    struct ppp_buffer *reply;
    Uint8 *value = NULL;
    Uint8 *cursor;
    Uint8 *name_cursor;
    short length;
    short value_length;
    short name_length;
    int i;
    short secret_length;
    short code;
    short result;
    MD5_CTX hash;
    Uint8 name[33];
    Uint8 value_bytes[64];
    Uint8 hashed[129];
    Uint8 message[129];

    if (header->id != fsm->last_id) {
        return -1;
    }
    length = header->length;
    value_length = PPP_PULL_BYTE(&packet);
    if (value_length != -1) {
        cursor = value = value_bytes;
        /* @bug memory: copies a response's value of up to 255 bytes into
         * value_bytes's 64, at 0x0c9e4448 */
        for (i = 0; i < value_length; i++) {
            *cursor++ = pull_octet(&packet);
        }
    }
    name_length = length - value_length - 1;
    if (name_length < 0) {
        name_length = 0;
    }
    if (name_length != 0) {
        name_cursor = name;
        /* @bug memory: copies the peer's name, every byte the header's length
         * leaves after the value, into name's 33, at 0x0c9e44e6 */
        for (i = 0; i < name_length; i++) {
            *name_cursor++ = pull_octet(&packet);
        }
        *name_cursor = '\0';
    }

    hashed[0] = header->id;
    COPY_STRING((char *)&hashed[1], chap->secret);
    secret_length = strlen(chap->secret);
    MD5Init(&hash);
    MD5Update(&hash, hashed, secret_length + 1);
    MD5Update(&hash, &challenge_value[1], challenge_length);
    MD5Final(message, &hash);
    /* @bug memory: compares through a null value where the packet held no
     * byte for the value's length, by the call at 0x0c9e453e */
    /* @bug logic: accepts every response. whether the digests match or not,
     * the code from 0x0c9e4542 answers CHAP_SUCCESS and returns 0 */
    /* @hack redundant: the test of the digests, whose arms are the same */
    if (memcmp(message, value, MD5_DIGEST_BYTES) == 0) {
        COPY_STRING((char *)message, welcome);
        code = CHAP_SUCCESS;
        result = 0;
    } else {
        COPY_STRING((char *)message, welcome);
        code = CHAP_SUCCESS;
        result = 0;
    }
    reply = PP_qdata(message, strlen((char *)message));
    PP_fsm_send(fsm, code, header->id, reply);
    return result;
}
#endif

void chap_timeout(void *argument)
{
    struct ppp_fsm *fsm = argument;
    struct chap_context *chap = fsm->context;
#ifdef NONMATCHING
    /* the GCC side sends the request again or restarts the timer, where the
     * original only counts the retry */
    if (fsm->retries) {
        chap->message = chap_request_timeout;
        if (fsm->flags & PPP_AUTH_LOCAL) {
            if (PP_fsm_sendreq(fsm) >= 0) {
                return;
            }
        } else {
            fsm->retries--;
            PP_start_timer(&fsm->timer);
            return;
        }
    }
    chap->message = chap_retries_exceeded;
    chap_shutdown(fsm);
    fsm->fault = PPP_FAULT_FAILED;
#else
    if (fsm->retries > 0) {
        chap->message = chap_request_timeout;
        fsm->retries--;
    } else {
        chap->message = chap_retries_exceeded;
        chap_close_link(fsm);
        fsm->fault = PPP_FAULT_FAILED;
    }
#endif
}

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching PP_chap_proc below: an
 * accepted response ends PPP_AUTH_LOCAL, and a success PPP_AUTH_REMOTE. it
 * also drops a packet that is short, or that comes with no credentials set,
 * and takes a success or a failure only for the response it sent */
void PP_chap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet)
{
    struct chap_context *chap = fsm->context;
    struct ppp_control_header header;
    Uint16 length;
    if (!chap || !chap->login || !chap->secret ||
        PP_ntohcnf(&header, &packet) < 0 ||
        header.length < CONTROL_HEADER_BYTES ||
        header.length - CONTROL_HEADER_BYTES > packet->length) {
        return;
    }
    length = header.length - CONTROL_HEADER_BYTES;
    switch (header.code) {
    case CHAP_CHALLENGE:
        chap_respond(fsm, header.id, packet->data, length);
        break;
    case CHAP_RESPONSE:
        chap_verify(fsm, header.id, packet->data, length);
        break;
    case CHAP_SUCCESS:
    case CHAP_FAILURE:
        if (!(fsm->flags & PPP_AUTH_REMOTE) || !chap->response_pending ||
            header.id != chap->response_id) {
            return;
        }
        chap->response_pending = 0;
        if (header.code == CHAP_SUCCESS) {
            chap_authenticated(fsm, PPP_AUTH_REMOTE);
        } else {
            chap_shutdown(fsm);
            fsm->fault = PPP_FAULT_REFUSED;
        }
        break;
    }
}
short PP_dochap_user(char *login, char *secret, void *argument)
{
    struct ppp_fsm *fsm = argument;
    struct chap_context *chap = fsm->context;
    chap->login = NULL;
    chap->secret = NULL;
    if (copy_credential(login_buffer, login) < 0 ||
        copy_credential(secret_buffer, secret) < 0) {
        chap_shutdown(fsm);
        fsm->fault = PPP_FAULT_REFUSED;
        return -1;
    }
    chap->login = login_buffer;
    chap->secret = secret_buffer;
    if (fsm->flags & PPP_AUTH_LOCAL) {
        if (PP_fsm_sendreq(fsm) < 0) {
            chap_shutdown(fsm);
            fsm->fault = PPP_FAULT_FAILED;
            return -1;
        }
    } else {
        PP_start_timer(&fsm->timer);
    }
    return 0;
}
#else
/* @hack type: the fsm as a void *, where a struct ppp_fsm * would do */
short PP_dochap_user(char *login, char *secret, void *argument)
{
    struct ppp_fsm *fsm = argument;
    struct chap_context *chap = fsm->context;
    char *login_copy;
    char *secret_copy;

    if (strlen(login) >= sizeof(login_buffer)) {
        login[sizeof(login_buffer) - 1] = '\0';
    }
    if (strlen(secret) >= sizeof(secret_buffer)) {
        secret[sizeof(secret_buffer) - 1] = '\0';
    }
    /* @hack spelling: each buffer's address held in a local, where its uses
     * could name the buffer */
    login_copy = login_buffer;
    COPY_STRING(login_copy, login);
    secret_copy = secret_buffer;
    COPY_STRING(secret_copy, secret);
    chap->login = login_copy;
    chap->secret = secret_copy;
    return 0;
}

void PP_chap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet)
{
    struct ppp_control_header header;

    if (PP_ntohcnf(&header, &packet) == -1) {
        return;
    }
    header.length -= CONTROL_HEADER_BYTES;
    PP_trim_mbuf(&packet, header.length);
    switch (header.code) {
    case CHAP_CHALLENGE:
        chap_respond(fsm, &header, packet);
        break;
    case CHAP_RESPONSE:
        /* @bug logic: an accepted response ends PPP_AUTH_REMOTE, at
         * 0x0c9e46b8, though it is PPP_AUTH_LOCAL that PP_chap_local set for
         * it. so CHAP never opens where this end authenticates the peer. the
         * peer's success ends PPP_AUTH_LOCAL, at 0x0c9e46ca, and with it
         * chap_authenticated clears the session's PPP_AUTH_REMOTE */
        if (chap_verify(fsm, &header, packet) == 0) {
            chap_authenticated(fsm, PPP_AUTH_REMOTE);
        }
        break;
    case CHAP_SUCCESS:
        PP_stop_timer(&fsm->timer);
        chap_authenticated(fsm, PPP_AUTH_LOCAL);
        break;
    case CHAP_FAILURE:
        PP_stop_timer(&fsm->timer);
        chap_close_link(fsm);
        fsm->fault = PPP_FAULT_REFUSED;
        break;
    }
}
#endif

void PP_chap_init(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_CHAP];
    struct ppp_timer *timer;

    if (fsm->context != NULL) {
        return;
    }
    fsm->session = session;
    fsm->protocol = &chap_protocol;
#ifdef NONMATCHING
    /* this side clears the state it adds to struct chap_context */
    memset(&chap_state, 0, sizeof(chap_state));
#endif
    fsm->context = &chap_state;
    fsm->max_requests = fsm->protocol->max_requests;
    fsm->max_naks = fsm->protocol->max_naks;
    fsm->max_terminate = fsm->protocol->max_terminate;
    fsm->state = PPP_CLOSED;
    fsm->retries = fsm->max_requests;
    fsm->nak_retries = fsm->max_naks;
    timer = &fsm->timer;
    timer->callback = chap_timeout;
    timer->argument = fsm;
    PP_set_timer(timer, fsm->protocol->timeout);
}

short PP_chap_local(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_CHAP];
#ifdef NONMATCHING
    /* this side forgets a challenge it sent before */
    struct chap_context *chap = fsm->context;
    chap->challenge_pending = 0;
#endif
    fsm->state = PPP_LISTEN;
    fsm->flags |= PPP_AUTH_LOCAL;
    session->flags |= PPP_AUTH_LOCAL;
    fsm->retries = fsm->max_requests;
    return 0;
}

short PP_chap_remote(struct ppp_session *session)
{
    struct ppp_fsm *fsm = &session->fsm[PPP_CHAP];
#ifdef NONMATCHING
    /* this side forgets a response it sent before */
    struct chap_context *chap = fsm->context;
    chap->response_pending = 0;
#endif
    fsm->state = PPP_REQUEST_SENT;
    fsm->flags |= PPP_AUTH_REMOTE;
    session->flags |= PPP_AUTH_REMOTE;
    return 0;
}

short PP_chap_monitor(struct ppp_fsm *fsm)
{
    struct ppp_session *session = fsm->session;
    fsm->fault = PPP_FAULT_NONE;
    while (fsm->state != PPP_OPENED) {
        if (ppp_is_abort() || fsm->fault != PPP_FAULT_NONE ||
            session->phase == PPP_TERMINATING) {
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

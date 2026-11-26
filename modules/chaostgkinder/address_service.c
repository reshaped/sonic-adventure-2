#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/address_packet.h"
#include "chaostgkinder/address_service.h"
#include "chaostgkinder/network.h"
#include "chaostgkinder/packet.h"
#include "chaostgkinder/transport.h"
#include "system/task.h"

struct address_session {
    struct packet_transport transport;
    Sint32 state; /* an address_state */
    enum address_error error;
    enum kinder_lookup_step lookup_step;
    Sint32 ticket;
    Sint32 pending, next_request, next_reply;
    struct address_request *requests[ADDRESS_QUEUE];
};

static const char server_host[] = "209.179.195.100";
static const Uint16 server_port = 15880;
static const char server_greeting[] =
    "\nSA2DTP 1.0 S ADDRESS\nSonicAdv2 data transfer server."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";
static const char client_greeting[] =
    "\nSA2DTP 1.0 C ADDRESS\nSonicAdv2 data transfer client."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";

#ifdef NONMATCHING
/* this side closes a session for good. it releases a lookup still pending and
 * closes the connection. each pending request gets ADDRESS_UNAVAILABLE, so that
 * no requester waits on a session that has ended */
static void close_address_session(struct address_session *session)
{
    if (session->state == ADDRESS_LOOKUP &&
        session->lookup_step == LOOKUP_POLL) {
        kinder_lookup_release(session->ticket);
        session->lookup_step = LOOKUP_START;
    }
    transport_close(&session->transport);
    while (session->pending > 0) {
        session->requests[session->next_reply]->result = ADDRESS_UNAVAILABLE;
        session->next_reply = (session->next_reply + 1) % ADDRESS_QUEUE;
        session->pending--;
    }
}
#endif

static void address_handle_greeting(struct address_session *session,
                                    struct greeting_packet *greeting)
{
    struct address_greeting *message;
    /* @hack spelling: the gotos to the failure's shared tail, where the
     * failure's stores and a return at each test would do */
    if (session->state != ADDRESS_WAIT_GREETING) {
        goto failed;
    }
    if (strcmp(greeting->text, server_greeting) != 0) {
        goto failed;
    }
    transport_start_cipher(&session->transport, greeting->receive_seed,
                           greeting->send_seed);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((message = syCalloc(1, sizeof(*message))) != NULL) {
        address_greeting_init(message);
        strcpy(message->text, client_greeting);
        transport_send(&session->transport, &message->header);
        syFree(message);
        session->state = ADDRESS_WAIT_ACK;
    }
#ifdef NONMATCHING
    /* this side fails the session where the heap has no reply to the greeting.
     * the matching side waits for good */
    else {
        goto failed;
    }
#endif
    return;
failed:
    session->state = ADDRESS_FAILED;
    session->error = ADDRESS_PROTOCOL_ERROR;
}

static void address_handle_ack(struct address_session *session,
                               struct kinder_packet *packet)
{
    /* @hack spelling: the goto to the failure's tail, where an else would
     * do */
    if (session->state != ADDRESS_WAIT_ACK) {
        goto failed;
    }
    session->state = ADDRESS_READY;
    return;
failed:
    session->state = ADDRESS_FAILED;
    session->error = ADDRESS_PROTOCOL_ERROR;
}

static void address_handle_reply(struct address_session *session,
                                 struct address_reply *reply)
{
    struct address_request *request;
#ifdef NONMATCHING
    /* this side fails the session on a reply while it isn't ready or has
     * nothing pending */
    if (session->state != ADDRESS_READY || session->pending == 0) {
        session->state = ADDRESS_FAILED;
        session->error = ADDRESS_PROTOCOL_ERROR;
        return;
    }
#endif
    request = session->requests[session->next_reply++];
    if (session->next_reply == ADDRESS_QUEUE) {
        session->next_reply = 0;
    }
    session->pending--;
    request->result = ADDRESS_FOUND;
    request->address = reply->address;
    request->port = reply->port;
}

static void address_dispatch(struct packet_transport *transport,
                             struct kinder_packet *packet)
{
    struct address_session *session = (struct address_session *)transport;
    switch (packet->kind) {
    case PACKET_GREETING:
        address_handle_greeting(session, (struct greeting_packet *)packet);
        break;
    case ADDRESS_ACK:
        address_handle_ack(session, packet);
        break;
    case ADDRESS_REPLY:
        address_handle_reply(session, (struct address_reply *)packet);
        break;
    default:
        transport_dispatch(transport, packet);
        break;
    }
}

static void address_destroy(struct task *task)
{
    struct address_session *session = task->extra.p;
    if (session) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: it closes the session before
         * it frees it */
        close_address_session(session);
#endif
        /* @bug logic: the session is freed with its connection open, and
         * with any lookup ticket it holds unreleased. account_destroy closes
         * its own connection first. address_destroy frees the session at
         * 0x0c9d5054 */
        syFree(session);
        task->extra.p = NULL;
    }
}

static void address_step(struct task *task)
{
    struct address_session *session = task->extra.p;
    switch (session->state) {
    case ADDRESS_IDLE:
        break;
    case ADDRESS_LOOKUP:
        switch (session->lookup_step) {
        case LOOKUP_START:
            session->ticket = kinder_lookup_start(server_host);
            if (session->ticket < 0) {
                session->state = ADDRESS_FAILED;
                session->error = ADDRESS_DNS_ERROR;
            } else {
                session->lookup_step = LOOKUP_POLL;
            }
            break;
        case LOOKUP_POLL:
            switch (kinder_lookup_poll(session->ticket,
                                       &session->transport.address)) {
            case KINDER_LOOKUP_DONE:
                kinder_lookup_release(session->ticket);
                session->transport.port = server_port;
                session->state = ADDRESS_CONNECTING;
                session->lookup_step = LOOKUP_START;
                break;
            case KINDER_LOOKUP_WAITING:
                break;
            case KINDER_LOOKUP_FAILED:
            default:
                kinder_lookup_release(session->ticket);
                session->state = ADDRESS_FAILED;
                session->error = ADDRESS_DNS_ERROR;
                break;
            }
            break;
        }
        break;
    case ADDRESS_CONNECTING:
        transport_open(&session->transport);
        switch (session->transport.state) {
        case TRANSPORT_OPENING:
            break;
        case TRANSPORT_OPEN:
            session->state = ADDRESS_WAIT_GREETING;
            break;
        case TRANSPORT_IDLE:
        case TRANSPORT_FAILED:
        default:
            session->state = ADDRESS_FAILED;
            session->error = ADDRESS_SOCKET_ERROR;
            break;
        }
        break;
    case ADDRESS_WAIT_GREETING:
    case ADDRESS_WAIT_ACK:
    case ADDRESS_READY:
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: a lost
         * connection fails the session */
        if (!transport_receive(&session->transport) &&
            session->state != ADDRESS_FAILED) {
            session->state = ADDRESS_FAILED;
            session->error = ADDRESS_SOCKET_ERROR;
        }
#else
        /* @bug logic: a lost connection never fails the session, so a
         * request pending on it waits forever. address_step jumps to
         * transport_receive at 0x0c9d51bc and leaves its result unread,
         * where account_step tests it at 0x0c9ddb5e */
        transport_receive(&session->transport);
#endif
        break;
    }
#ifdef NONMATCHING
    /* this side closes a session that failed */
    if (session->state == ADDRESS_FAILED) {
        close_address_session(session);
    }
#endif
}

struct task *address_service_create(struct task *parent)
{
    struct task *task;
    struct address_session *session = NULL;
    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, which tests task and session again, where a statement and
     * a return of each would do */
    if (!(task = task_create_child(0, address_step, parent))) {
        goto failed;
    }
    if (!(session = syCalloc(1, sizeof(*session)))) {
        goto failed;
    }
    session->transport.dispatch = address_dispatch;
    session->transport.decode = address_packet_decode;
    session->pending = 0;
    session->next_request = 0;
    session->next_reply = 0;
    task->extra.p = session;
    task->destructor = address_destroy;
    return task;
failed:
    if (task) {
        task_kill(task);
    }
    if (session) {
        syFree(session);
    }
    return NULL;
}

void address_service_destroy(struct task *task)
{
    task_kill(task);
}

Sint32 address_service_start(struct task *task)
{
    Sint32 started = 0;
    struct address_session *session = task->extra.p;
    if (session->state == ADDRESS_IDLE) {
        session->error = ADDRESS_OK;
        session->state = ADDRESS_LOOKUP;
        session->lookup_step = LOOKUP_START;
        started = 1;
    }
    return started;
}

void address_service_finish(void)
{
}

Sint32 address_service_state(struct task *task)
{
    struct address_session *session = task->extra.p;
    return session->state;
}

Sint32 address_service_error(struct task *task)
{
    struct address_session *session = task->extra.p;
    return session->error;
}

Sint32 address_service_submit(struct task *task,
                              struct address_request *request)
{
    struct address_session *session = task->extra.p;
    struct address_value *message;
    Sint32 accepted = 0;
    if (session->state == ADDRESS_READY && session->pending < ADDRESS_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            address_request_init(message);
            message->value = request->service;
            transport_send(&session->transport, &message->header);
            syFree(message);
            request->result = ADDRESS_PENDING;
            session->requests[session->next_request++] = request;
            /* @bug memory: the fifth request a session takes, and each one
             * after it, is stored past the session's end, at 0x0c9d5354.
             * next_request never wraps. next_reply does, so the fifth reply
             * answers the first request again */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: next_request wraps as
             * next_reply does */
            if (session->next_request == ADDRESS_QUEUE) {
                session->next_request = 0;
            }
#endif
            session->pending++;
            accepted = 1;
        }
    }
    return accepted;
}

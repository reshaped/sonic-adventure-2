#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/address_service.h"
#include "chaostgkinder/network.h"
#include "chaostgkinder/packet.h"
#include "chaostgkinder/registration.h"
#include "chaostgkinder/registration_packet.h"
#include "chaostgkinder/transport.h"
#include "system/task.h"

struct registration_session {
    struct packet_transport transport;
    Sint32 state, result; /* a registration_state and a registration_error */
    enum kinder_lookup_step lookup_step;
    struct task *lookup;
};

Uint32 registration_address = 0;
Uint16 registration_port = 0;
static const char *server_greeting =
    "\nSA2DTP 1.0 S REGISTER\nSonicAdv2 data transfer server."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";
static const char *client_greeting =
    "\nSA2DTP 1.0 C REGISTER\nSonicAdv2 data transfer client."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";

static void request_registration_list(struct registration_session *session)
{
    struct kinder_packet *request;
    /* @bug logic: leaves the session in REGISTRATION_WAIT_LIST when the
     * request's allocation fails, though it asked for no list (0x0c9c7cb2) */
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((request = syCalloc(1, sizeof(*request))) != NULL) {
        registration_list_request_init(request);
        transport_send(&session->transport, request);
        syFree(request);
    }
#ifdef NONMATCHING
    /* this side fails the session where the heap has no request. the matching
     * side waits on that request for good */
    else {
        session->state = REGISTRATION_FAILED;
    }
#endif
}

static void registration_greeting(struct registration_session *session,
                                  struct greeting_packet *greeting)
{
    struct identity_packet *identity;
    if (session->state != REGISTRATION_WAIT_GREETING) {
        goto failed;
    }
    if (strcmp(greeting->text, server_greeting) != 0) {
        goto failed;
    }
    transport_start_cipher(&session->transport, greeting->receive_seed,
                           greeting->send_seed);
    /* @bug logic: goes on to REGISTRATION_WAIT_STATUS when the identity's
     * allocation fails, and waits for a reply to an identity it never sent
     * (0x0c9c7d14, 0x0c9c7d4e) */
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((identity = syCalloc(1, sizeof(*identity))) != NULL) {
        identity_packet_init(identity);
        kinder_console_id(identity->console_id);
        identity->flags = 0;
        strcpy(identity->greeting, client_greeting);
        transport_send(&session->transport, &identity->header);
        syFree(identity);
    }
#ifdef NONMATCHING
    /* this side fails the session where the heap has no identity to send. the
     * matching side waits on it for good */
    else {
        goto failed;
    }
#endif
    session->state = REGISTRATION_WAIT_STATUS;
    return;
failed:
    session->state = REGISTRATION_FAILED;
}

static void registration_status_reply(struct registration_session *session,
                                      struct registration_status *reply)
{
    if (session->state != REGISTRATION_WAIT_STATUS) {
        goto failed;
    }
    switch (reply->status) {
    case 0:
        session->state = REGISTRATION_WAIT_LIST;
        request_registration_list(session);
        break;
    case 1:
        session->state = REGISTRATION_STATUS_ONE;
        break;
    case 2:
        session->state = REGISTRATION_STATUS_TWO;
        break;
    case 16:
        session->result = REGISTRATION_REFUSED_16;
        goto failed;
    case 17:
        session->result = REGISTRATION_REFUSED_17;
        goto failed;
    case 18:
        session->result = REGISTRATION_REFUSED_18;
        goto failed;
    default:
        session->result = REGISTRATION_PROTOCOL_ERROR;
        goto failed;
    }
    return;
failed:
    session->state = REGISTRATION_FAILED;
}

static void registration_list_reply(struct registration_session *session,
                                    struct registration_list *list)
{
    if (session->state == REGISTRATION_WAIT_LIST) {
        /* @hack spelling: a switch on the count, where an if would do */
        switch (list->count) {
        case 0:
            return;
        default:
            session->result = REGISTRATION_PROTOCOL_ERROR;
            break;
        }
    }
    session->state = REGISTRATION_FAILED;
}

static void registration_dispatch(struct packet_transport *transport,
                                  struct kinder_packet *packet)
{
    struct registration_session *session =
        (struct registration_session *)transport;
    switch (packet->kind) {
    case PACKET_GREETING:
        registration_greeting(session, (struct greeting_packet *)packet);
        break;
    case REGISTER_STATUS:
        registration_status_reply(session,
                                  (struct registration_status *)packet);
        break;
    case REGISTER_LIST:
        registration_list_reply(session, (struct registration_list *)packet);
        break;
    default:
        transport_dispatch(transport, packet);
        break;
    }
}

static void registration_step(struct task *task)
{
    struct registration_session *session = task->extra.p;
    switch (session->state) {
    case REGISTRATION_IDLE:
        break;
    case REGISTRATION_LOOKUP:
        /* @hack redundant: the switch on lookup_step and its LOOKUP_POLL case,
         * where the LOOKUP_START case alone would do, since nothing sets
         * lookup_step but to LOOKUP_START */
        switch (session->lookup_step) {
        case LOOKUP_START:
            /* @bug logic: makes another lookup task each frame while
             * registration_address is 0, since the step never tests the
             * one it made (0x0c9c7f04, 0x0c9c7f20) */
            if (registration_address) {
                session->transport.address = registration_address;
                session->transport.port = registration_port;
                session->state = REGISTRATION_CONNECTING;
                session->lookup_step = LOOKUP_START;
#ifdef NONMATCHING
                /* this side waits for a lookup still running, where the
                 * matching side starts another */
            } else if (session->lookup) {
                break;
#endif
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
            } else if ((session->lookup = address_service_create(task)) ==
                       NULL) {
                session->state = REGISTRATION_FAILED;
                session->result = REGISTRATION_LOOKUP_ERROR;
            }
            break;
        case LOOKUP_POLL:
            break;
        }
        break;
    case REGISTRATION_CONNECTING:
        transport_open(&session->transport);
        switch (session->transport.state) {
        case TRANSPORT_OPENING:
            break;
        case TRANSPORT_OPEN:
            session->state = REGISTRATION_WAIT_GREETING;
            break;
        case TRANSPORT_IDLE:
        case TRANSPORT_FAILED:
        default:
            session->state = REGISTRATION_FAILED;
            session->result = REGISTRATION_SOCKET_ERROR;
        }
        break;
    case REGISTRATION_WAIT_GREETING:
    case REGISTRATION_WAIT_STATUS:
        /* @bug logic: never reads the list it asks for, since
         * REGISTRATION_WAIT_LIST takes the switch's default, which reads
         * nothing (0x0c9c7ee6) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. the list's wait reads the
     * connection, and fails where it is lost */
    case REGISTRATION_WAIT_LIST:
        if (!transport_receive(&session->transport)) {
            session->result = REGISTRATION_SOCKET_ERROR;
            session->state = REGISTRATION_FAILED;
        }
#else
        transport_receive(&session->transport);
#endif
        break;
    case REGISTRATION_DONE:
        break;
    }
}

#ifdef NONMATCHING
/* the destructor this side gives the task, which closes the connection before
 * it frees the session */
static void registration_destroy(struct task *task)
{
    struct registration_session *session = task->extra.p;
    transport_close(&session->transport);
    syFree(session);
    task->extra.p = NULL;
}
#endif

struct task *registration_create(struct task *parent)
{
    struct task *task;
    struct registration_session *session = NULL;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create_child(0, registration_step, parent)) == NULL) {
        goto failed;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((session = syCalloc(1, sizeof(*session))) == NULL) {
        goto failed;
    }
    session->transport.dispatch = registration_dispatch;
    session->transport.decode = registration_packet_decode;
    task->extra.p = session;
    /* @bug logic: never closes the session's socket, since the task gets no
     * destructor. destroying the task frees the session with the socket
     * still open (0x0c9c7ffe) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the task closes its connection as
     * it dies */
    task->destructor = registration_destroy;
#endif
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

Bool registration_start(struct task *task)
{
    struct registration_session *session = task->extra.p;
    Bool started = FALSE;
    if (session->state == REGISTRATION_IDLE) {
        session->result = REGISTRATION_OK;
        session->state = REGISTRATION_LOOKUP;
        session->lookup_step = LOOKUP_START;
        started = TRUE;
    }
    return started;
}

void registration_stop(struct task *task)
{
    struct registration_session *session = task->extra.p;
    switch (session->state) {
    case REGISTRATION_IDLE:
    case REGISTRATION_DONE:
        break;
    default:
        /* @bug logic: returns the session to REGISTRATION_IDLE with its
         * connection open and its lookup task alive (0x0c9c8068) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it closes the connection and
         * ends the lookup */
        transport_close(&session->transport);
        if (session->lookup) {
            address_service_destroy(session->lookup);
            session->lookup = NULL;
        }
#endif
        session->state = REGISTRATION_IDLE;
        session->lookup_step = LOOKUP_START;
        break;
    }
}

Sint32 registration_state(struct task *task)
{
    /* @hack spelling: task->extra cast in the return, where a session local
     * as the other functions have would do */
    return ((struct registration_session *)task->extra.p)->state;
}

Sint32 registration_result(struct task *task)
{
    /* @hack spelling: task->extra cast in the return, where a session local
     * as the other functions have would do */
    return ((struct registration_session *)task->extra.p)->result;
}

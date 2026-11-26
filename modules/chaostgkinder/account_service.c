#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/account_packet.h"
#include "chaostgkinder/account_service.h"
#include "chaostgkinder/packet.h"
#include "chaostgkinder/transport.h"
#include "chaostgkinder/wide_text.h"
#include "system/task.h"

/* why a session failed. enum address_error gives the same failures the same
 * numbers */
enum account_error {
    ACCOUNT_OK = 0,
    ACCOUNT_SOCKET_ERROR = 2,
    ACCOUNT_PROTOCOL_ERROR = 3
};

struct account_session {
    struct packet_transport transport;
    Sint32 state; /* an account_state */
    enum account_error error;
    Sint32 pending, next_request, next_reply;
    struct account_request *requests[ACCOUNT_QUEUE];
#ifdef NONMATCHING
    /* the reply kind each pending request expects, by which this side's
     * dispatch refuses a reply out of order */
    int expected[ACCOUNT_QUEUE];
#else
    Sint32 reserved;
#endif
};

/* the request a STATUS reply answers */
struct account_status_request {
    struct account_request request;
    struct account_status_packet *reply;
};

#pragma section ATA
static char server_greeting[] =
    "\nSA2DTP 1.0 S ACCOUNT\nSonicAdv2 data transfer server."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";
static char client_greeting[] =
    "\nSA2DTP 1.0 C ACCOUNT\nSonicAdv2 data transfer client."
    "  (c) 2001 SONICTEAM, LTD.  All rights reserved.\n";
#pragma section

static void account_handle_greeting(struct account_session *session,
                                    struct greeting_packet *greeting)
{
    struct account_greeting *message;
    /* @hack spelling: the gotos to the failure's shared tail, where the
     * failure's stores and a return at each test would do */
    if (session->state != ACCOUNT_WAIT_GREETING) {
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
        account_greeting_init(message);
        strcpy(message->text, client_greeting);
        transport_send(&session->transport, &message->header);
        syFree(message);
        session->state = ACCOUNT_WAIT_ACK;
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
    session->state = ACCOUNT_FAILED;
    session->error = ACCOUNT_PROTOCOL_ERROR;
}

static void account_handle_ack(struct account_session *session,
                               struct kinder_packet *packet)
{
    /* @hack spelling: the goto to the failure's tail, where an else would
     * do */
    if (session->state != ACCOUNT_WAIT_ACK) {
        goto failed;
    }
    session->state = ACCOUNT_READY;
    return;
failed:
    session->state = ACCOUNT_FAILED;
    session->error = ACCOUNT_PROTOCOL_ERROR;
}

/* declare request, a struct type *, as session's oldest queued request.
 * take it off the queue, and mark it ACCOUNT_ANSWERED. SHC's code tests
 * neither that a request is pending nor that it awaits this kind of reply */
#define ANSWER_OLDEST(request, type, session)                                  \
    struct type *request =                                                     \
        (struct type *)(session)->requests[(session)->next_reply++];           \
    if ((session)->next_reply == ACCOUNT_QUEUE) {                              \
        (session)->next_reply = 0;                                             \
    }                                                                          \
    --(session)->pending;                                                      \
    (request)->request.result = ACCOUNT_ANSWERED

static void account_handle_mail(struct account_session *session,
                                struct account_mail_reply *reply)
{
    ANSWER_OLDEST(request, account_mail_request, session);
    request->user_id = reply->user_id;
    kinder_wide_terminate(request->name, reply->name, ACCOUNT_NAME_CODES - 1);
}

static void account_handle_name(struct account_session *session,
                                struct account_value_reply *reply)
{
    ANSWER_OLDEST(request, account_name_request, session);
    request->user_id = reply->value;
    request->taken = reply->status;
}

static void account_handle_create(struct account_session *session,
                                  struct account_create_reply *reply)
{
    ANSWER_OLDEST(request, account_create_request, session);
    request->status = reply->status;
}

static void account_handle_find(struct account_session *session,
                                struct account_value_reply *reply)
{
    ANSWER_OLDEST(request, account_find_request, session);
    request->status = reply->status;
    request->value = reply->value;
}

static void account_handle_read(struct account_session *session,
                                struct account_record *reply)
{
    ANSWER_OLDEST(request, account_read_request, session);
    *request->record = *reply;
}

static void account_handle_write(struct account_session *session,
                                 struct account_status_packet *reply)
{
    ANSWER_OLDEST(request, account_write_request, session);
    *request->reply = *reply;
}

static void account_handle_status(struct account_session *session,
                                  struct account_status_packet *reply)
{
    ANSWER_OLDEST(request, account_status_request, session);
    *request->reply = *reply;
}

static void account_dispatch(struct packet_transport *transport,
                             struct kinder_packet *packet)
{
    struct account_session *session = (struct account_session *)transport;
#ifdef NONMATCHING
    /* this side fails the session on a reply it isn't waiting for. it judges a
     * reply by its kind and by the count pending */
    if (packet->kind >= ACCOUNT_MAIL_REPLY &&
        packet->kind <= ACCOUNT_STATUS_REPLY &&
        (session->state != ACCOUNT_READY || session->pending == 0 ||
         session->expected[session->next_reply] != packet->kind)) {
        session->state = ACCOUNT_FAILED;
        session->error = ACCOUNT_PROTOCOL_ERROR;
        return;
    }
#endif
    switch (packet->kind) {
    case PACKET_GREETING:
        account_handle_greeting(session, (struct greeting_packet *)packet);
        break;
    case ACCOUNT_ACK:
        account_handle_ack(session, packet);
        break;
    case ACCOUNT_MAIL_REPLY:
        account_handle_mail(session, (struct account_mail_reply *)packet);
        break;
    case ACCOUNT_NAME_REPLY:
        account_handle_name(session, (struct account_value_reply *)packet);
        break;
    case ACCOUNT_CREATE_REPLY:
        account_handle_create(session, (struct account_create_reply *)packet);
        break;
    case ACCOUNT_FIND_REPLY:
        account_handle_find(session, (struct account_value_reply *)packet);
        break;
    case ACCOUNT_READ_REPLY:
        account_handle_read(session, (struct account_record *)packet);
        break;
    case ACCOUNT_WRITE_REPLY:
        account_handle_write(session, (struct account_status_packet *)packet);
        break;
    case ACCOUNT_STATUS_REPLY:
        account_handle_status(session, (struct account_status_packet *)packet);
        break;
    default:
        transport_dispatch(transport, packet);
        break;
    }
}

#ifdef NONMATCHING
/* this side gives each pending request ACCOUNT_CANCELLED, so that no requester
 * waits on a session that has ended */
static void cancel_pending(struct account_session *session)
{
    while (session->pending > 0) {
        session->requests[session->next_reply]->result = ACCOUNT_CANCELLED;
        session->next_reply = (session->next_reply + 1) % ACCOUNT_QUEUE;
        session->pending--;
    }
}
#endif

static void account_destroy(struct task *task)
{
    struct account_session *session = task->extra.p;
    transport_close(&session->transport);
#ifdef NONMATCHING
    /* this side cancels the pending requests before it frees the session they
     * wait on */
    cancel_pending(session);
#endif
    syFree(session);
    task->extra.p = NULL;
}

static void account_step(struct task *task)
{
    struct account_session *session = task->extra.p;
    switch (session->state) {
    case ACCOUNT_IDLE:
        break;
    case ACCOUNT_CONNECTING:
        transport_open(&session->transport);
        switch (session->transport.state) {
        case TRANSPORT_OPENING:
            break;
        case TRANSPORT_OPEN:
            session->state = ACCOUNT_WAIT_GREETING;
            break;
        case TRANSPORT_IDLE:
        case TRANSPORT_FAILED:
        default:
            session->state = ACCOUNT_FAILED;
            session->error = ACCOUNT_SOCKET_ERROR;
        }
        break;
    case ACCOUNT_WAIT_GREETING:
    case ACCOUNT_WAIT_ACK:
    case ACCOUNT_READY:
        if (!transport_receive(&session->transport)) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side. a lost
             * connection fails the session, and the task lives on for the menu
             * to read */
            session->state = ACCOUNT_FAILED;
            session->error = ACCOUNT_SOCKET_ERROR;
#else
            /* @bug memory: a lost connection kills the task, and the account
             * menu goes on using it through account_task once it is freed.
             * account_step's branch to account_service_cancel at 0x0c9ddb68
             * kills it. connect_account at 0x0c9d558a polls its state, and
             * check_user at 0x0c9d59e0 sends a request through it */
            account_service_cancel(task);
#endif
        }
        break;
    }
#ifdef NONMATCHING
    /* this side cancels the pending requests and closes the connection of a
     * session that failed */
    if (session->state == ACCOUNT_FAILED) {
        cancel_pending(session);
        transport_close(&session->transport);
    }
#endif
}

struct task *account_service_create(struct task *parent)
{
    struct task *task;
    struct account_session *session;
    /* @hack spelling: each assignment inside its test, and the failures'
     * shared tail, where a statement and a return of each would do */
    if ((task = task_create_child(0, account_step, parent)) == NULL) {
        goto failed;
    }
    if ((session = syCalloc(1, sizeof(*session))) == NULL) {
        goto release_task;
    }
    session->transport.dispatch = account_dispatch;
    session->transport.decode = account_packet_decode;
    session->state = ACCOUNT_IDLE;
    session->error = ACCOUNT_OK;
    session->pending = 0;
    session->next_request = 0;
    session->next_reply = 0;
    task->extra.p = session;
    task->destructor = account_destroy;
    return task;
release_task:
    task_kill(task);
failed:
    return NULL;
}

void account_service_cancel(struct task *task)
{
    struct account_session *session = task->extra.p;
#ifdef NONMATCHING
    /* this side cancels the pending requests by cancel_pending, whose loop this
     * side's destroy and failure share. each request ends as on the matching
     * side */
    cancel_pending(session);
#else
    while (session->pending > 0) {
        struct account_request *request =
            session->requests[session->next_reply++];
        if (session->next_reply == ACCOUNT_QUEUE) {
            session->next_reply = 0;
        }
        --session->pending;
        request->result = ACCOUNT_CANCELLED;
    }
#endif
    task_kill(task);
}

Sint32 account_service_state(struct task *task)
{
    struct account_session *session = task->extra.p;
    return session->state;
}

int account_service_start(struct task *task)
{
    struct account_session *session = task->extra.p;
    int started = 0;
    if (session->state == ACCOUNT_IDLE) {
        session->state = ACCOUNT_CONNECTING;
        started = 1;
    }
    return started;
}

int account_service_endpoint(struct task *task, Uint32 address, Uint16 port)
{
    struct account_session *session = task->extra.p;
    int set = 0;
    if (session->state == ACCOUNT_IDLE) {
        set = 1;
        session->transport.address = address;
        session->transport.port = port;
    }
    return set;
}

#ifdef NONMATCHING
/* this side records the reply each request expects, and copies a field no
 * longer than the field holds */
#define EXPECT_REPLY(session, reply_kind)                                      \
    ((session)->expected[(session)->next_request] = (reply_kind))
#define COPY_FIELD(dst, src)                                                   \
    do {                                                                       \
        strncpy((dst), (src), sizeof(dst) - 1);                                \
        (dst)[sizeof(dst) - 1] = 0;                                            \
    } while (0)
#else
#define EXPECT_REPLY(session, reply_kind) ((void)0)
#define COPY_FIELD(dst, src) strcpy((dst), (src))
#endif

/* give request the result ACCOUNT_PENDING, and queue it in session */
#define QUEUE_REQUEST(session, request, reply_kind)                            \
    do {                                                                       \
        (request)->request.result = ACCOUNT_PENDING;                           \
        EXPECT_REPLY(session, reply_kind);                                     \
        (session)->requests[(session)->next_request++] = &(request)->request;  \
        if ((session)->next_request == ACCOUNT_QUEUE) {                        \
            (session)->next_request = 0;                                       \
        }                                                                      \
        ++(session)->pending;                                                  \
    } while (0)

int account_find_mail(struct task *task, struct account_mail_request *request)
{
    struct account_session *session = task->extra.p;
    struct account_mail *message;
    int queued = 0;
    if (session->state == ACCOUNT_READY && session->pending < ACCOUNT_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            account_mail_init(message);
            COPY_FIELD(message->mail, request->mail);
            transport_send(&session->transport, &message->header);
            syFree(message);
            QUEUE_REQUEST(session, request, ACCOUNT_MAIL_REPLY);
            queued = 1;
        }
    }
    return queued;
}

int account_check_name(struct task *task, struct account_name_request *request)
{
    struct account_session *session = task->extra.p;
    struct account_name *message;
    int queued = 0;
    if (session->state == ACCOUNT_READY && session->pending < ACCOUNT_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            account_name_init(message);
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: it copies
             * the name's codes whole */
            kinder_wide_terminate(message->name, request->name,
                                  ACCOUNT_NAME_CODES - 1);
#else
            /* @bug logic: strcpy copies the name's codes a byte at a time
             * and stops at the first zero byte, the high byte of a code
             * under 256, at 0x0c9ddd64 */
            strcpy((char *)message->name, (const char *)request->name);
#endif
            transport_send(&session->transport, &message->header);
            syFree(message);
            QUEUE_REQUEST(session, request, ACCOUNT_NAME_REPLY);
            queued = 1;
        }
    }
    return queued;
}

int account_create_profile(struct task *task,
                           struct account_create_request *request)
{
    struct account_session *session = task->extra.p;
    struct account_create *message;
    int queued = 0;
    if (session->state == ACCOUNT_READY && session->pending < ACCOUNT_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            account_create_init(message);
            COPY_FIELD(message->profile.name, request->name);
            COPY_FIELD(message->profile.text, request->text);
            message->profile.flag = request->flag;
            message->profile.group = request->group;
            message->profile.choice = request->choice;
            transport_send(&session->transport, &message->header);
            syFree(message);
            QUEUE_REQUEST(session, request, ACCOUNT_CREATE_REPLY);
            queued = 1;
        }
    }
    return queued;
}

int account_find_profile(struct task *task,
                         struct account_find_request *request)
{
    struct account_session *session = task->extra.p;
    struct account_find *message;
    int queued = 0;
    if (session->state == ACCOUNT_READY && session->pending < ACCOUNT_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            account_find_init(message);
            COPY_FIELD(message->name, request->name);
            transport_send(&session->transport, &message->header);
            syFree(message);
            QUEUE_REQUEST(session, request, ACCOUNT_FIND_REPLY);
            queued = 1;
        }
    }
    return queued;
}

int account_read_profile(struct task *task,
                         struct account_read_request *request)
{
    struct account_session *session = task->extra.p;
    struct kinder_packet *message;
    int queued = 0;
#ifdef NONMATCHING
    /* this side sends nothing on a session that isn't ready, as the other
     * requests' sends refuse to. the matching side sends anyway */
    if (session->state != ACCOUNT_READY) {
        return 0;
    }
#endif
    if (session->pending < ACCOUNT_QUEUE) {
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((message = syCalloc(1, sizeof(*message))) != NULL) {
            account_read_init(message);
            transport_send(&session->transport, message);
            syFree(message);
            QUEUE_REQUEST(session, request, ACCOUNT_READ_REPLY);
            queued = 1;
        }
    }
    return queued;
}

int account_write_profile(struct task *task,
                          struct account_write_request *request)
{
    struct account_session *session = task->extra.p;
    int queued = 0;
#ifdef NONMATCHING
    /* this side sends nothing on a session that isn't ready, as the other
     * requests' sends refuse to. the matching side sends anyway */
    if (session->state != ACCOUNT_READY) {
        return 0;
    }
#endif
    if (session->pending < ACCOUNT_QUEUE) {
        transport_send(&session->transport, &request->record->header);
        QUEUE_REQUEST(session, request, ACCOUNT_WRITE_REPLY);
        queued = 1;
    }
    return queued;
}

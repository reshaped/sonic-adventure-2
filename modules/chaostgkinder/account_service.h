/* the ACCOUNT service: a task that holds a session with the account server.
 * the session writes each request's result. a request must stay alive while
 * its result is ACCOUNT_PENDING. in SHC's code, a session whose connection
 * is lost once open cancels its requests and kills its task */
#ifndef CHAOSTGKINDER_ACCOUNT_SERVICE_H
#define CHAOSTGKINDER_ACCOUNT_SERVICE_H
#include <sg_xpt.h>

struct account_record;
struct account_status_packet;
struct task;

/* the most requests a session holds pending */
#define ACCOUNT_QUEUE 4

enum account_state {
    ACCOUNT_IDLE,
    ACCOUNT_CONNECTING,
    ACCOUNT_WAIT_GREETING,
    ACCOUNT_WAIT_ACK,
    ACCOUNT_READY,
    ACCOUNT_FAILED
};
enum account_request_state {
    ACCOUNT_CANCELLED = -1,
    ACCOUNT_PENDING = 0,
    ACCOUNT_ANSWERED = 1
};
/* the head of every request */
struct account_request {
    enum account_request_state result;
};
/* a mail reply's user_id where no user has the mail */
#define ACCOUNT_NO_USER (-1)
/* mail is an ISP's mail address. the reply gives user_id, the id of the user
 * with that mail or ACCOUNT_NO_USER, and the user's name. name needs room for
 * ACCOUNT_NAME_CODES codes */
struct account_mail_request {
    struct account_request request;
    const char *mail;
    Uint32 user_id;
    Uint16 *name;
};
/* name is a user name to check. the reply gives user_id, and taken, TRUE
 * where another user has the name */
struct account_name_request {
    struct account_request request;
    const Uint16 *name;
    Uint32 user_id;
    Bool taken;
};
/* a new profile, in account_profile_fields' terms. the reply sets status to
 * the server's */
struct account_create_request {
    struct account_request request;
    const char *name, *text;
    Bool flag;
    Sint16 group, choice, status;
};
/* name is a profile name to find. the reply gives status, 0 where the
 * profile is found, and value, its id */
struct account_find_request {
    struct account_request request;
    const char *name;
    Sint32 status;
    Uint32 value;
};
/* the reply's record goes to *record */
struct account_read_request {
    struct account_request request;
    struct account_record *record;
};
/* record goes to the server as it is, so account_write_init must have made
 * it. the reply goes to *reply, whose status is 0 where the server saved the
 * profile */
struct account_write_request {
    struct account_request request;
    struct account_record *record;
    struct account_status_packet *reply;
};
/* a child task of parent with an idle session, or NULL */
struct task *account_service_create(struct task *parent);
/* mark each pending request ACCOUNT_CANCELLED, and kill the task */
void account_service_cancel(struct task *task);
/* an enum account_state */
Sint32 account_service_state(struct task *task);
/* start connecting an idle session to its server, and return 1. on any other
 * session, return 0 */
int account_service_start(struct task *task);
/* set an idle session's server, and return 1. on any other session, return
 * 0 */
int account_service_endpoint(struct task *task, Uint32 address, Uint16 port);
/* send request, give it the result ACCOUNT_PENDING, and return 1. return 0
 * without sending where the session isn't ACCOUNT_READY, already holds
 * ACCOUNT_QUEUE requests or can't allocate the packet */
int account_find_mail(struct task *task, struct account_mail_request *request);
int account_check_name(struct task *task, struct account_name_request *request);
int account_create_profile(struct task *task,
                           struct account_create_request *request);
int account_find_profile(struct task *task,
                         struct account_find_request *request);
/* SHC's code sends these two whatever the session's state */
int account_read_profile(struct task *task,
                         struct account_read_request *request);
int account_write_profile(struct task *task,
                          struct account_write_request *request);
#endif

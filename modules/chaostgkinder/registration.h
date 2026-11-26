/* the REGISTER service, a task. it sends its server the console's ID, and
 * asks for the console's registration list. a started session waits in
 * REGISTRATION_LOOKUP until the caller sets registration_address. it then
 * connects to that address at registration_port */
#ifndef CHAOSTGKINDER_REGISTRATION_H
#define CHAOSTGKINDER_REGISTRATION_H
#include <sg_xpt.h>

struct task;

/* registration_state returns these. the server's status 0 moves the session
 * to REGISTRATION_WAIT_LIST. its statuses 1 and 2 move it to
 * REGISTRATION_STATUS_ONE and REGISTRATION_STATUS_TWO. the session reads no
 * more replies in any of the three. NONMATCHING reads on in
 * REGISTRATION_WAIT_LIST, where a list with entries fails the session with
 * REGISTRATION_PROTOCOL_ERROR. nothing sets REGISTRATION_DONE */
enum registration_state {
    REGISTRATION_IDLE = 0,
    REGISTRATION_LOOKUP = 1,
    REGISTRATION_CONNECTING = 2,
    REGISTRATION_WAIT_GREETING = 3,
    REGISTRATION_WAIT_STATUS = 4,
    REGISTRATION_STATUS_ONE = 5,
    REGISTRATION_STATUS_TWO = 6,
    REGISTRATION_WAIT_LIST = 7,
    REGISTRATION_DONE = 9,
    REGISTRATION_FAILED = 10
};
/* registration_result returns these. a wrong greeting or a reply out of turn
 * fails the session and leaves REGISTRATION_OK. the server's statuses 16 to
 * 18 fail it with the REGISTRATION_REFUSED_ value of the same number. any
 * other status the session doesn't know fails it with
 * REGISTRATION_PROTOCOL_ERROR */
enum registration_error {
    REGISTRATION_OK,
    REGISTRATION_LOOKUP_ERROR,
    REGISTRATION_SOCKET_ERROR,
    REGISTRATION_PROTOCOL_ERROR,
    REGISTRATION_REFUSED_16,
    REGISTRATION_REFUSED_17,
    REGISTRATION_REFUSED_18
};

extern Uint32 registration_address;
extern Uint16 registration_port;

/* the session's task, a child of parent, in REGISTRATION_IDLE, or NULL */
struct task *registration_create(struct task *parent);
/* start an idle session and return TRUE. a session that isn't idle is left
 * as it is, and the call returns FALSE */
Bool registration_start(struct task *task);
/* return a started session to REGISTRATION_IDLE. its connection stays open
 * and its lookup task alive, so a session started again doesn't connect
 * anew. NONMATCHING closes the connection and destroys the lookup task. a
 * session in REGISTRATION_DONE stays there */
void registration_stop(struct task *task);
/* the session's registration_state, and its registration_error */
Sint32 registration_state(struct task *task);
Sint32 registration_result(struct task *task);
#endif

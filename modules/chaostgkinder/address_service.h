/* the ADDRESS service: a task that asks the ADDRESS server where other
 * services are. the session writes each request's result. a request must
 * stay alive while its result is ADDRESS_PENDING, until
 * address_service_destroy */
#ifndef CHAOSTGKINDER_ADDRESS_SERVICE_H
#define CHAOSTGKINDER_ADDRESS_SERVICE_H
#include <sg_xpt.h>

struct task;
enum address_state {
    ADDRESS_IDLE,
    ADDRESS_LOOKUP,
    ADDRESS_CONNECTING,
    ADDRESS_WAIT_GREETING,
    ADDRESS_WAIT_ACK,
    ADDRESS_READY,
    ADDRESS_FAILED
};
enum address_error {
    ADDRESS_OK,
    ADDRESS_DNS_ERROR,
    ADDRESS_SOCKET_ERROR,
    ADDRESS_PROTOCOL_ERROR
};
enum address_request_status {
    ADDRESS_PENDING,
    ADDRESS_FOUND,
    ADDRESS_UNAVAILABLE
};
enum address_service_id { ADDRESS_ACCOUNT_SERVICE = 1 };

/* the most requests a session holds pending */
#define ADDRESS_QUEUE 4

/* the reply gives the service's address and port, and the result
 * ADDRESS_FOUND. SHC's code gives no other result. a request pending when
 * the session fails stays ADDRESS_PENDING. so does one pending when the
 * connection is lost, which leaves the session ADDRESS_READY. the GCC side
 * fails the session there, and makes its pending requests
 * ADDRESS_UNAVAILABLE */
struct address_request {
    enum address_request_status result;
    enum address_service_id service;
    Uint32 address;
    Uint16 port;
};
/* a child task of parent with an idle session, or NULL */
struct task *address_service_create(struct task *parent);
/* kill the task. SHC's code leaves its pending requests ADDRESS_PENDING, and
 * the GCC side makes them ADDRESS_UNAVAILABLE */
void address_service_destroy(struct task *task);
/* start an idle session's lookup of the ADDRESS server's host, and return 1.
 * on any other session, return 0 */
Sint32 address_service_start(struct task *task);
/* does nothing */
void address_service_finish(void);
/* an enum address_state */
Sint32 address_service_state(struct task *task);
/* an enum address_error: why the session failed, or ADDRESS_OK */
Sint32 address_service_error(struct task *task);
/* send request, give it the result ADDRESS_PENDING, and return 1. return 0
 * without sending where the session isn't ADDRESS_READY, holds ADDRESS_QUEUE
 * requests or can't allocate the packet. SHC's code stores a session's
 * fifth request, and every one after it, past the session's end */
Sint32 address_service_submit(struct task *task,
                              struct address_request *request);
#endif

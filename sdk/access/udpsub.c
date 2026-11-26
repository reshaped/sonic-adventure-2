/* the bridge from a datagram to its UDP or raw IP socket's AT_udp_receiver */
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <access/at_types.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* this side calls the receiver through its own type, the plain form the hack
 * tag below gives: a call through another function type is undefined in C */
#define UDP_RECEIVER(callback) (callback)
#else
/* @hack type: the calls through a type that takes and returns ints, where
 * receiver's own type would do */
typedef int (*promoted_receiver)(int status, AT_UDPPRM *request);
#define UDP_RECEIVER(callback) ((promoted_receiver)(callback))
#endif

void udp_rcv_call(AT_udp_receiver receiver, Sint16 socket, AT_MaxBuffs *packet,
                  Sint16 length, Uint32 source, Sint16 port)
{
    AT_UDPPRM request;

    request.handle = socket;
    request.buf = packet;
    request.len = length;
    request.srcaddr.u32 = source;
    request.srcport.u16 = port;
#ifdef NONMATCHING
    /* this side starts usrbuff at NULL, so a receiver that leaves it unset
     * fails the test below */
    request.usrbuff = NULL;
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    if (UDP_RECEIVER(receiver)(AT_UDP_GET_BUFFER, &request) == 0) {
        if (request.len >= length) {
            request.len = length;
        }
#ifdef NONMATCHING
        /* this side refuses a negative length, and a length with no buffer to
         * copy into */
        if (request.len < 0 || (request.len > 0 && !request.usrbuff)) {
            return;
        }
#endif
        API_UdpRecvCallback(request.usrbuff, packet, request.len);
        /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
        UDP_RECEIVER(receiver)(AT_UDP_COPIED, &request);
    }
}

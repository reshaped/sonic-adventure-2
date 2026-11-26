/* the bridges from a socket's events to the application's callbacks */
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <sg_xpt.h>

Sint32 tcp_accept_notify(Sint16 socket, AT_accept_notify callback, Sint16 child,
                         Uint32 address, Uint16 local_port, Uint16 peer_port)
{
#ifdef NONMATCHING
    /* this side calls callback through its own type, the plain form the hack
     * tag below gives: a call through another function type is undefined in
     * C */
    callback(child, socket, address, local_port, peer_port);
#else
    /* @hack type: the call through a type that takes each 16-bit word as an
     * int, where callback's own type would do */
    typedef void (*promoted_accept)(int, int, Uint32, int, int);
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    ((promoted_accept)callback)(child, socket, address, local_port, peer_port);
#endif
    return 0;
}

Sint32 tcp_send_notify(Sint16 socket, AT_send_notify callback, Sint16 result)
{
    callback(result, socket);
    return 0;
}

Sint32 tcp_sendf_notify(Sint16 socket, AT_sendf_notify callback, Sint16 result,
                        Uint8 *buffer)
{
    callback(result, socket, buffer);
    return 0;
}

Sint32 tcp_receive_notify(Sint16 socket, AT_receive_notify callback,
                          Sint16 result, Sint16 length)
{
    callback(result, socket, length);
    return 0;
}

void asr_call(Sint16 socket, AT_asr callback, Sint16 event)
{
#ifdef NONMATCHING
    /* this side calls callback through its own type, the plain form the hack
     * tag below gives: a call through another function type is undefined in
     * C */
    callback(socket, event);
#else
    /* @hack type: the call through a type that takes each 16-bit word as an
     * int, where callback's own type would do */
    typedef void (*promoted_asr)(int, int);
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    ((promoted_asr)callback)(socket, event);
#endif
}

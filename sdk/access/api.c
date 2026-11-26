/* the handlers of ACCESS's requests, and the notify calls that complete them.
 * an accept, send or receive with a null notifyaddr waits in its handler. the
 * handler yields with the critical section unlocked until the wait ends */
#ifdef NONMATCHING
/* this side repairs the bug tagged below: api.c declares and calls the bridges
 * with apisub.c's Sint16 words */
#else
/* @bug mismatched-call: api.c passes the socket, child, result and length
 * words of its notify calls as Sint32s, where apisub.c's bridges take
 * Sint16s. the calls are API_AcceptNotify's, API_SendNotify's,
 * API_SendfNotify's and API_RecvNotify's (0x0cad586c, 0x0cad5b8e,
 * 0x0cad5c56, 0x0cad5d72) */
#define AT_PROMOTED_CALLBACKS
#endif
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <access/at_types.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/mib.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <access/stack_globals.h>
#include <string.h>

/* a handle's SOCKET_CLASS bits are its class: UDP_HANDLE, IP_HANDLE or 0
 * for TCP. SOCKET_INDEX keeps a UDP or IP handle's socket number, and
 * UDP_INDEX clears the UDP bit alone */
#define SOCKET_CLASS 0xc0
#define SOCKET_INDEX 0x3f
#define UDP_INDEX 0x7f
#define UDP_HANDLE 0x80
#define IP_HANDLE 0xc0
#define TCP_SEND_BUFFERS 4
/* @hack type: API_GENERAL as a Uint16, where API_GENERAL would do. each
 * handler's caller narrows its result to a Sint16 or tests it against 0 */
#define API_GENERAL_U16 ((Uint16)API_GENERAL)

const char *const ntgTcpBuild = "\nntTcp Ver 1.07 Build:Oct 25 2000 13:30:42\n";

/* the requests that wait, by socket. socket is API_PENDING while one waits.
 * the request's notify call ends the wait: it stores result, then sets socket
 * to the socket's own number. API_ASRCall ends a send's or a receive's wait
 * too, setting socket before result. request_socket takes the socket's number
 * at its first send or receive, and keeps it. socket 0's holds its number from
 * the start */
struct accept_wait {
    Sint32 socket, result, complete;
    Uint32 address;
    Uint16 local_port, peer_port;
};
struct send_wait {
    Sint32 socket, result, request_socket;
};
struct buffer_wait {
    Sint32 socket, result;
};
struct receive_wait {
    Sint32 socket, result, length, request_socket;
};

static Bool interface_up;
static volatile struct accept_wait accept_waits[AT_TCP_SOCKETS];
static volatile struct send_wait send_waits[AT_TCP_SOCKETS];
static volatile struct buffer_wait buffer_waits[AT_TCP_SOCKETS];
static volatile struct receive_wait receive_waits[AT_TCP_SOCKETS];

Sint32 API_SetASR(AT_setasr *request)
{
    switch (request->nh & SOCKET_CLASS) {
    case UDP_HANDLE:
        return UDP_SetASR(request->nh & SOCKET_INDEX, (Sint32)request->asraddr);
    case IP_HANDLE:
        return IP_SetASR(request->nh & SOCKET_INDEX, (Sint32)request->asraddr);
    default:
        return TCP_SetASR(request->nh, (Sint32)request->asraddr);
    }
}

Sint32 API_IfConfigUp(AT_ifconfigupparam *request)
{
    int result;
    if (request->if_num == 0) {
        ACCESS_LOCAL_ADDRESS = request->ip_addr.u32;
        ACCESS_NETMASK = request->netmask.u32;
        ACCESS_NETMASK = IP_SetNetmask(ACCESS_NETMASK, ACCESS_LOCAL_ADDRESS);
        ACCESS_BROADCAST = request->broadcast.u32;
        interface_up = TRUE;
    }
    if (request->if_num == 0) {
        result = INIT_IfconfigUp(ACCESS_LOCAL_ADDRESS, ACCESS_NETMASK,
                                 ACCESS_BROADCAST, request->if_num);
    } else {
        result = INIT_IfconfigUp(request->ip_addr.u32, request->netmask.u32,
                                 request->broadcast.u32, request->if_num);
    }
    mib_init();
    if (result) {
        return API_GENERAL_U16;
    }
    return 0;
}

Sint32 API_IfConfigDown(AT_ifconfigdownparam *request)
{
    if (request->if_num == 0) {
        interface_up = FALSE;
    }
    return INIT_IfconfigDown(request->if_num);
}

Sint32 API_TCP_Open(AT_openparam *request)
{
    int socket;
    Sint32 error;
    if ((socket = TCP_Socket()) < 0) {
        return API_PORTFULL;
    }
    if (TCP_Bind(socket, request->myport.u16, request->youraddr.u32,
                 request->yourport.u16) < 0) {
        TCP_Close(socket);
        return API_DUPSOCK;
    }
    request->reserve2 = 0;
    request->reserve3 = 0;
    TCP_InitSockopt(socket, request->ttl, request->svctype, request->df_flag,
                    request->reserve2, request->reserve3,
                    (Sint32)request->asraddr);
    if (TCP_Connect(socket, request->youraddr.u32, request->yourport.u16) < 0) {
        error = TCP_GetErrcode(socket);
        TCP_Close(socket);
        if (error == TCP_EUNREACHABLE) {
            return API_UNREACHABLE;
        }
        return API_CONNRESET;
    }
    return socket;
}

Sint32 API_TCP_POpen(AT_popenparam *request)
{
    int socket;
    if ((socket = TCP_Socket()) < 0) {
        return API_PORTFULL;
    }
    if (TCP_Bind(socket, request->myport.u16, request->youraddr.u32,
                 request->yourport.u16) < 0) {
        TCP_Close(socket);
        return API_DUPSOCK;
    }
    TCP_InitSockopt(socket, request->ttl, request->svctype, request->df_flag,
                    request->reserve2, request->reserve3, 0);
    if (TCP_Listen(socket, request->youraddr.u32, request->yourport.u16) < 0) {
        TCP_Close(socket);
        return API_UNREACHABLE;
    }
    return socket;
}

Sint32 API_TCP_Accept(AT_acceptparam *request, Uint32 *address,
                      Uint16 *local_port, Uint16 *peer_port)
{
    if (request->nh >= max_tcb_entry || request->nh < 0 ||
        tcb_table[request->nh].in_use != 1) {
        return API_INVALIDHANDLE;
    }
    accept_waits[request->nh].socket = API_PENDING;
    if (TCP_Accept(request->nh, (Sint32)request->notifyaddr,
                   (Sint32)request->asraddr) < 0) {
        return API_INVALIDHANDLE;
    }
    if (request->notifyaddr) {
        return API_PENDING;
    } else {
        OS_CriticalUnLock();
        while (accept_waits[request->nh].socket == API_PENDING) {
            OS_Yield();
        }
        OS_CriticalLock();
        TCP_AcceptNow(accept_waits[request->nh].result);
        *address = accept_waits[request->nh].address;
        *local_port = accept_waits[request->nh].local_port;
        *peer_port = accept_waits[request->nh].peer_port;
        return accept_waits[request->nh].result;
    }
}

Sint32 API_AcceptNotify(Sint32 socket, Sint32 request, Sint32 child,
                        Uint32 peer, Uint16 local_port, Uint16 peer_port)
{
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS entries of
     * accept_waits, which the matching side indexes unchecked */
    if ((Uint32)socket >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
#endif
    if (request) {
        TCP_AcceptNow(child);
        TCP_AcceptEndNotify(
            socket, tcp_accept_notify(socket, (AT_accept_notify)request, child,
                                      peer, local_port, peer_port));
    } else {
        accept_waits[socket].result = child;
        accept_waits[socket].complete = 1;
        accept_waits[socket].address = peer;
        accept_waits[socket].local_port = local_port;
        accept_waits[socket].peer_port = peer_port;
        accept_waits[socket].socket = socket;
    }
    return 0;
}

Sint32 API_TCP_Close(AT_nhparam *request)
{
    switch (request->nh & SOCKET_CLASS) {
    case UDP_HANDLE:
        return UDP_Close(request->nh & SOCKET_INDEX);
    case IP_HANDLE:
        return IP_Close(request->nh & SOCKET_INDEX);
    }
    return TCP_Close(request->nh);
}

Sint32 API_TCP_Shutdown(AT_nhparam *request)
{
    if (TCP_Shutdown(request->nh, TCP_SHUT_WR) < 0) {
        return API_INVALIDHANDLE;
    }
    return 0;
}

Sint32 API_TCP_Abort(AT_nhparam *request)
{
    return TCP_Abort(request->nh);
}

Sint32 API_TCP_GetAddr(AT_nhparam *request, Uint32 *address, Uint16 *local_port,
                       Uint16 *peer_port)
{
    struct sock_addr endpoint;
    if (request->nh & UDP_HANDLE) {
        if (UDP_ApiGetaddr(request->nh & UDP_INDEX, &endpoint) < 0) {
            return API_INVALIDHANDLE;
        }
        *address = endpoint.addr;
        *local_port = endpoint.local_port;
        *peer_port = endpoint.peer_port;
        return 0;
    } else {
        if (TCP_ApiGetaddr(request->nh, &endpoint) < 0) {
            return API_INVALIDHANDLE;
        }
        *address = endpoint.addr;
        *local_port = endpoint.local_port;
        *peer_port = endpoint.peer_port;
    }
    return 0;
}

Sint32 API_TCP_Stat(AT_nhparam *request, Uint16 *state, Uint16 *pending,
                    Uint16 *send_window, Uint16 *receive_window)
{
    struct tcp_stat status;
    Sint32 result;
    result = TCP_SockStat(request->nh, &status);
    if (result < 0) {
        return API_INVALIDHANDLE;
    }
    *state = result;
    *pending = status.pending;
    *send_window = status.snd_space;
    *receive_window = status.rcv_cc;
    switch (TCP_GetErrcode(request->nh)) {
    case 0:
        return 0;
    case TCP_ETIMEDOUT:
        return API_TIMEOUT;
    case TCP_EUNREACHABLE:
        return API_UNREACHABLE;
    }
    return API_CONNRESET;
}

Sint32 API_TCP_Send(AT_sendparam *request)
{
    AT_MaxBuffs packet;
    Sint32 i;
    Sint32 result;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it reads bufnum unsigned, so a
     * send of more than TCP_SEND_BUFFERS buffers fails with API_NOBUF. it
     * refuses a socket past the AT_TCP_SOCKETS wait entries too */
    if ((Uint32)request->nh >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
    if (request->bufnum > TCP_SEND_BUFFERS) {
        return API_NOBUF;
    }
#endif
    packet.total_len = 0;
    packet.bufnum = (Sint8)request->bufnum;
    /* @bug logic: reads bufnum as signed, so a send of 128 buffers or more
     * passes the test and returns 0 without sending, where API_NOBUF is meant
     * (0x0cad5a6e) */
    if ((Sint8)request->bufnum > TCP_SEND_BUFFERS) {
        return API_NOBUF;
    }
    request->reserve = 0;
    for (i = 0; i < packet.bufnum; i++) {
#ifdef NONMATCHING
        /* this side refuses a buffer of negative length, or with a length and
         * no address, which the matching side hands TCP_Write */
        if (request->buffers[i].len < 0 ||
            (request->buffers[i].len && !request->buffers[i].buf)) {
            return API_NOBUF;
        }
#endif
        OS_SetVptrExternal(&packet.buffers[i].vptr, request->buffers[i].buf);
        packet.buffers[i].len = request->buffers[i].len;
        packet.total_len += packet.buffers[i].len;
    }
    if (packet.total_len == 0) {
        return 0;
    }
    send_waits[request->nh].socket = API_PENDING;
    send_waits[request->nh].request_socket = request->nh;
    if ((result = TCP_Write(request->nh, &packet, (Sint32)request->notifyaddr,
                            request->reserve)) < 0) {
        return result;
    }
    if (request->notifyaddr) {
        return API_PENDING;
    } else {
        OS_CriticalUnLock();
        while (send_waits[request->nh].socket == API_PENDING) {
            OS_Yield();
        }
        OS_CriticalLock();
        return send_waits[request->nh].result;
    }
}

Sint32 API_SendNotify(Sint32 socket, Sint32 request, Sint32 result)
{
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS entries of send_waits,
     * which the matching side indexes unchecked */
    if ((Uint32)socket >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
#endif
    if (request) {
        request = tcp_send_notify(socket, (AT_send_notify)request, result);
        TCP_WriteEndNotify(socket, request);
    } else {
        send_waits[socket].result = result;
        send_waits[socket].socket = socket;
    }
    return 0;
}

Sint32 API_TCP_Sendf(AT_sendfparam *request)
{
    AT_Vptr address;
    Sint32 result;
    Sint16 length;
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS wait entries, a
     * negative length, and a length with no buffer, which the matching side
     * hands TCP_Writef */
    if ((Uint32)request->nh >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
    if (request->len < 0 || (request->len && !request->buf)) {
        return API_NOBUF;
    }
#endif
    OS_SetVptrExternal(&address, request->buf);
    length = request->len;
    buffer_waits[request->nh].socket = API_PENDING;
    result = (Sint16)TCP_Writef(request->nh, &address.buff, length,
                                (Sint32)request->notifyaddr, request->reserve);
    if (result < 0) {
        return (Sint16)result;
    }
    if (request->notifyaddr) {
        return API_PENDING;
    } else {
        OS_CriticalUnLock();
        while (buffer_waits[request->nh].socket == API_PENDING) {
            OS_Yield();
        }
        OS_CriticalLock();
        return buffer_waits[request->nh].result;
    }
}

Sint32 API_SendfNotify(Sint32 socket, Sint32 request, Sint32 result,
                       AT_Vptr *buffer)
{
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS entries of
     * buffer_waits, which the matching side indexes unchecked */
    if ((Uint32)socket >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
#endif
    if (request) {
        tcp_sendf_notify(socket, (AT_sendf_notify)request, result,
                         (Uint8 *)buffer->buff);
    } else {
        buffer_waits[socket].result = result;
        buffer_waits[socket].socket = socket;
    }
    return 0;
}

Sint32 API_TCP_Receive(AT_rcvparam *request, Sint16 *length)
{
    AT_Vptr address;
    int result;
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS wait entries, a
     * negative length, and a length with no buffer, which the matching side
     * hands TCP_Read */
    if ((Uint32)request->nh >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
    if (request->len < 0 || (request->len && !request->buf)) {
        return API_NOBUF;
    }
#endif
    OS_SetVptrExternal(&address, request->buf);
    receive_waits[request->nh].socket = API_PENDING;
    receive_waits[request->nh].request_socket = request->nh;
    if ((result = TCP_Read(request->nh, &address, request->len,
                           (Sint32)request->notifyaddr)) < 0) {
        return result;
    }
    if (request->notifyaddr) {
        return API_PENDING;
    } else {
        OS_CriticalUnLock();
        while (receive_waits[request->nh].socket == API_PENDING) {
            OS_Yield();
        }
        OS_CriticalLock();
        *length = receive_waits[request->nh].length;
        if (receive_waits[request->nh].result == 0) {
            return API_RECVFIN;
        }
        return receive_waits[request->nh].result;
    }
}

Sint32 API_RecvNotify(Sint32 socket, Sint32 request, Uint32 result,
                      Sint32 available)
{
    Sint32 end;
#ifdef NONMATCHING
    /* this side refuses a socket past the AT_TCP_SOCKETS entries of
     * receive_waits, which the matching side indexes unchecked */
    if ((Uint32)socket >= AT_TCP_SOCKETS) {
        return API_INVALIDHANDLE;
    }
#endif
    if (request) {
        end = tcp_receive_notify(socket, (AT_receive_notify)request,
                                 result == 0 ? API_RECVFIN : (Sint32)result,
                                 available);
        TCP_ReadEndNotify(socket, end);
    } else {
        receive_waits[socket].result = result;
        receive_waits[socket].length = available;
        receive_waits[socket].socket = socket;
    }
    return 0;
}

Sint32 API_TCP_Receivef(void *request, Sint16 *length)
{
    return API_GENERAL_U16;
}

Sint32 API_TCP_Cancel(AT_cancelparam *request)
{
    return TCP_Cancel(request->nh, request->what);
}

Sint32 API_SetSockopt(struct at_option *request)
{
    int result;
    if ((result = TCP_SetSockopt(request->nh, request->option, request->value,
                                 request->length)) < 0) {
        return result;
    }
    return 0;
}

Sint32 API_GetSockopt(struct at_option *request)
{
    int length = request->length;
    int result;
    if ((result = TCP_GetSockopt(request->nh, request->option, request->value,
                                 &length)) < 0) {
        return result;
    }
    return length;
}

Sint32 API_UDP_Open(AT_uopenparam *request)
{
    Sint32 result;
    result = UDP_Open(request->youraddr.u32, request->yourport.u16,
                      request->myport.u16, request->reserve1, request->ttl,
                      request->svctype, request->df_flag, request->reserve2,
                      request->reserve3, (Sint32)request->asraddr,
                      request->receiver);
    if (result < 0) {
        return result;
    }
    return result | UDP_HANDLE;
}

Sint32 API_UDP_Send(AT_usendparam *request)
{
    AT_MaxBuffs *packet;
    Uint32 remaining;
    Sint32 index;
    Sint32 result;
#ifdef NONMATCHING
    /* this side refuses more buffers than the packet's AT_MAX_BUFFERS, which
     * the copy below would write past */
    if (request->bufnum > AT_MAX_BUFFERS) {
        return API_NOBUF;
    }
#endif
    packet = OS_AllocBuffs();
#ifdef NONMATCHING
    /* this side fails the send while every packet is lent, where the matching
     * side writes through OS_AllocBuffs's null */
    if (!packet) {
        return API_NOBUF;
    }
#endif
    packet->bufnum = remaining = (Sint8)request->bufnum;
    packet->total_len = 0;
    index = 0;
    while (remaining > 0) {
        remaining--;
#ifdef NONMATCHING
        /* this side refuses a buffer of negative length, or with a length and
         * no address, and returns the packet it took */
        if (request->buffers[remaining].len < 0 ||
            (request->buffers[remaining].len &&
             !request->buffers[remaining].buf)) {
            OS_FreeBuffs(packet);
            return API_NOBUF;
        }
#endif
        OS_SetVptrExternal(&packet->buffers[index].vptr,
                           request->buffers[remaining].buf);
        packet->buffers[index].len = request->buffers[remaining].len;
        packet->total_len += packet->buffers[index].len;
        index++;
    }
    result = UDP_Send(packet, request->youraddr.u32, request->yourport.u16,
                      request->nh & UDP_INDEX);
    OS_FreeBuffs(packet);
    if (result) {
        return API_GENERAL_U16;
    }
    return 0;
}

Sint32 API_IP_Open(struct at_ip_open *request)
{
    Sint32 result;
    result = IP_Open(request->address, request->protocol, (Sint32)request->asr,
                     request->receiver);
    if (result < 0) {
        return result;
    }
    return result | IP_HANDLE;
}

Sint32 API_IP_Send(struct at_ip_send *request)
{
    AT_MaxBuffs *packet;
    Uint32 remaining;
    Sint32 index;
    Sint32 result;
#ifdef NONMATCHING
    /* this side refuses a bufnum below 0 or past the packet's AT_MAX_BUFFERS,
     * which the copy below would write past */
    if (request->bufnum < 0 || request->bufnum > AT_MAX_BUFFERS) {
        return API_NOBUF;
    }
#endif
    packet = OS_AllocBuffs();
#ifdef NONMATCHING
    /* this side fails the send while every packet is lent, where the matching
     * side writes through OS_AllocBuffs's null */
    if (!packet) {
        return API_NOBUF;
    }
#endif
    packet->bufnum = remaining = request->bufnum;
    packet->total_len = 0;
    index = 0;
    while (remaining > 0) {
        remaining--;
#ifdef NONMATCHING
        /* this side refuses a buffer of negative length, or with a length and
         * no address, and returns the packet it took */
        if (request->buffers[remaining].len < 0 ||
            (request->buffers[remaining].len &&
             !request->buffers[remaining].buf)) {
            OS_FreeBuffs(packet);
            return API_NOBUF;
        }
#endif
        OS_SetVptrExternal(&packet->buffers[index].vptr,
                           request->buffers[remaining].buf);
        packet->buffers[index].len = request->buffers[remaining].len;
        packet->total_len += packet->buffers[index].len;
        index++;
    }
    result = IP_ApiSend(packet, request->nh & SOCKET_INDEX, 0, IP_DEFAULT_TTL,
                        0, 0, 0, 0);
    OS_FreeBuffs(packet);
    if (result < 0) {
        return result;
    }
    return 0;
}

Sint32 API_RouteAdd(AT_addrtparam *request)
{
    Uint32 destination;
    Uint32 mask;
    destination = request->dstaddr.u32;
    if (ACCESS_LOCAL_ADDRESS == 0 && ACCESS_NETMASK == 0 &&
        ACCESS_BROADCAST == 0) {
        mask = IP_SetNetmaskRoute(request->gtwayaddr.u32, ACCESS_NETMASK,
                                  ACCESS_LOCAL_ADDRESS);
    } else {
        mask = IP_SetNetmaskRoute(destination, ACCESS_NETMASK,
                                  ACCESS_LOCAL_ADDRESS);
    }
    if (IP_RouteAdd(destination, mask, request->gtwayaddr.u32, request->reserve,
                    RT_KIND_APPLICATION, 0) < 0) {
        return API_GENERAL;
    }
    return 0;
}

Sint32 API_RouteDel(AT_delrtparam *request)
{
    Uint32 destination;
    Uint32 mask;
    destination = request->dstaddr.u32;
    mask =
        IP_SetNetmaskRoute(destination, ACCESS_NETMASK, ACCESS_LOCAL_ADDRESS);
    if (IP_RouteDel(destination, mask, request->gtwayaddr.u32,
                    RT_KIND_APPLICATION) < 0) {
        return API_GENERAL;
    }
    return 0;
}

Sint32 API_RouteList(AT_lstrtparam *request)
{
    AT_Vptr destination;
    destination.buff = (Uint32)request->buf;
    destination.type = AT_PTR_TYPE_EXTERNAL;
    return IP_RouteList(request->len, &destination, 0);
}

Sint32 API_ArpAdd(struct at_arp_entry *request)
{
    Uint8 hardware[sizeof(request->hardware)];
    memcpy(hardware, request->hardware, sizeof(hardware));
    return ARP_TableAddApi(request->address, hardware, request->flags);
}

Sint32 API_ArpDel(struct at_arp_entry *request)
{
    Uint8 hardware[sizeof(request->hardware)];
    memcpy(hardware, request->hardware, sizeof(hardware));
    return ARP_TableDelApi(request->address, hardware);
}

Sint32 API_ArpList(struct at_arp_list *request)
{
    AT_Vptr destination;
    destination.buff = (Uint32)request->entries;
    destination.type = AT_PTR_TYPE_EXTERNAL;
    return ARP_ArpList(request->length, &destination);
}

Sint32 API_UdpRecvCallback(void *destination, AT_MaxBuffs *packet,
                           Sint32 length)
{
    AT_Vptr cursor;
    Sint32 index;
    Sint32 size;
#ifdef NONMATCHING
    /* this side refuses a negative length, a length with no destination, a
     * missing packet, a bufnum outside 0 to AT_MAX_BUFFERS, and a length past
     * the packet's */
    if (length < 0 || (!destination && length) || !packet ||
        packet->bufnum < 0 || packet->bufnum > AT_MAX_BUFFERS ||
        length > packet->total_len) {
        return API_NOBUF;
    }
#endif
    OS_SetVptrExternal(&cursor, destination);
    index = 0;
    while (length != 0) {
#ifdef NONMATCHING
        /* this side stops at the packet's last buffer, and at a negative
         * length, where the matching side reads on */
        if (index >= packet->bufnum || packet->buffers[index].len < 0) {
            return API_NOBUF;
        }
#endif
        if (length - packet->buffers[index].len >= 0) {
            size = packet->buffers[index].len;
        } else {
            size = length;
        }
        OS_Vmemcpy(&cursor, &packet->buffers[index].vptr, size);
        OS_IncVptr(&cursor, size);
        length -= size;
        index++;
    }
    return 0;
}

Sint32 API_UdpRecv(AT_MaxBuffs *packet, Uint32 source, Sint16 port,
                   AT_udp_receiver receiver, Sint16 socket)
{
    if (receiver) {
        udp_rcv_call(receiver, socket | UDP_HANDLE, packet, packet->total_len,
                     source, port);
    }
    return 0;
}

Sint32 API_IpRecv(AT_MaxBuffs *packet, Uint32 source, AT_udp_receiver receiver,
                  int socket)
{
    if (receiver) {
        udp_rcv_call(receiver, socket | IP_HANDLE, packet, packet->total_len,
                     source, 0);
    }
    return 0;
}

int API_ASRCall(int socket, Sint32 asr, int protocol, int event)
{
    if (protocol == ASR_UDP) {
        socket |= UDP_HANDLE;
    } else if (protocol == ASR_IP) {
        socket |= IP_HANDLE;
    } else if (event == ASR_RESET || event == ASR_TIMEOUT ||
               event == ASR_CLOSED) {
#ifdef NONMATCHING
        /* this side refuses a socket past the AT_TCP_SOCKETS wait entries,
         * which the matching side indexes unchecked */
        if ((Uint32)socket >= AT_TCP_SOCKETS) {
            return API_INVALIDHANDLE;
        }
#endif
        if (receive_waits[socket].request_socket == socket &&
            tcb_table[receive_waits[socket].request_socket].state ==
                TCPS_ESTABLISHED) {
            receive_waits[socket].socket = socket;
            receive_waits[socket].result = API_CONNRESET;
        } else if (send_waits[socket].request_socket == socket &&
                   tcb_table[send_waits[socket].request_socket].state ==
                       TCPS_ESTABLISHED) {
            send_waits[socket].socket = socket;
            send_waits[socket].result = API_CONNRESET;
        }
    }
    if (asr) {
        asr_call(socket, (AT_asr)asr, event);
    }
    /* API_ASRCall returns no value, and no caller reads one */
#ifdef NONMATCHING
    /* this side returns 0, since GCC fails a non-void function that runs off
     * its end */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

int initialize_all(int reserved)
{
    if (INIT_All() != 0) {
        return 1;
    }
    return 0;
}

void terminate_all(void)
{
    if (interface_up) {
        DL_Exit();
    }
}

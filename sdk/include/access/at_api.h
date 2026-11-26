/* ACCESS's requests: AT_apiCall's codes and results, and each request's record.
 * a record's addresses and ports are in network byte order */
#ifndef ACCESS_AT_API_H
#define ACCESS_AT_API_H
#include <access/at_types.h>
#include <sg_xpt.h>

/* the count of buffers a send request's record holds. the matching side's
 * holds one, and a request of more continues past the record's end */
#ifdef NONMATCHING
/* this side's holds AT_MAX_BUFFERS, room for every buffer */
#define AT_REQUEST_BUFFERS AT_MAX_BUFFERS
#else
#define AT_REQUEST_BUFFERS 1
#endif
#define GETCONFIG 0x4101
#define SETASR 0x4102
#define IFCONFIG 0x4105
#define IFDOWN 0x4106
#define ROUTE_ADD 0x4130
#define ROUTE_DEL 0x4131
#define TCP_OPEN 0x4110
#define TCP_POPEN 0x4111
#define TCP_ACCEPT 0x4112
#define TCP_CLOSE 0x4113
#define TCP_SHUTDOWN 0x4114
#define TCP_GETADDR 0x4116
#define TCP_ABORT 0x4115
#define TCP_STAT 0x4117
#define TCP_SEND 0x4118
#define TCP_RECEIVE 0x4119
#define TCP_CANCEL 0x411a
#define TCP_SEND_F 0x411b
#define UDP_OPEN 0x4120
#define UDP_SEND 0x4121
#define ROUTE_LIST 0x4132

#define API_PENDING (-1)
#define API_PORTFULL (-2)
#define API_DUPSOCK (-3)
#define API_INVALIDHANDLE (-4)
#define API_CONNRESET (-6)
#define API_UNREACHABLE (-7)
#define API_RECVFIN (-8)
/* a send's result on a socket shut for sending. it is also a receive's on one
 * shut for receiving with no bytes left */
#define API_SHUTDOWN (-10)
#define API_TIMEOUT (-11)
#define API_NOBUF (-12)
#define API_BADOPT (-13)
#define API_GENERAL (-16)

typedef union at_nint32 {
    Uint32 u32;
    Uint8 b[4];
} AT_NINT32;
typedef union at_nint16 {
    Uint16 u16;
    Uint8 b[2];
} AT_NINT16;
typedef struct _AT_config {
    AT_NINT32 ip_addr, netmask, broadcast;
} AT_config;
typedef void (*AT_asr)(Sint16 handle, Sint16 event);
typedef struct _AT_setasr {
    Sint16 nh;
    AT_asr asraddr;
} AT_setasr;
typedef struct _AT_configptr {
    void *config_ptr;
} AT_configptr;
typedef struct _AT_ifconfigparam {
    Sint16 if_num;
    AT_NINT32 ip_addr, netmask, broadcast;
} AT_ifconfigupparam;
typedef struct _AT_ifconfigdownparam {
    Sint16 if_num;
} AT_ifconfigdownparam;
typedef struct _AT_addrtparam {
    AT_NINT32 dstaddr, gtwayaddr;
    Sint16 reserve;
} AT_addrtparam;
typedef struct _AT_delrtparam {
    AT_NINT32 dstaddr, gtwayaddr;
} AT_delrtparam;
typedef struct _AT_UDPPRM {
    Sint16 handle;
    void *buf;
    Sint16 len;
    AT_NINT32 srcaddr;
    AT_NINT16 srcport;
    Uint8 *usrbuff;
} AT_UDPPRM;
typedef Sint16 (*AT_udp_receiver)(Sint16 status, AT_UDPPRM *packet);
/* an AT_udp_receiver's status. with AT_UDP_GET_BUFFER, packet->len is the
 * datagram's length. a receiver that takes it sets usrbuff and len to a buffer
 * and its size. it then returns 0. with AT_UDP_COPIED, the datagram's first len
 * bytes are in usrbuff */
#define AT_UDP_GET_BUFFER 0
#define AT_UDP_COPIED 1
typedef struct _AT_uopenparam {
    AT_NINT32 youraddr;
    AT_NINT16 yourport, myport;
    Uint8 reserve1, ttl, svctype, df_flag;
    Sint16 reserve2;
    void *reserve3;
    AT_asr asraddr;
    AT_udp_receiver receiver;
} AT_uopenparam;
/* a UDP send of bufnum buffers, AT_MAX_BUFFERS - 3 at most: UDP_Send,
 * IP_Send and DL_Send each append one */
typedef struct _AT_usendparam {
    Sint16 nh;
    AT_NINT32 youraddr;
    AT_NINT16 yourport;
    Uint8 bufnum;
    struct {
        Sint16 len;
        Uint8 *buf;
    } buffers[AT_REQUEST_BUFFERS];
} AT_usendparam;
typedef struct _AT_openparam {
    AT_NINT32 youraddr;
    AT_NINT16 yourport, myport;
    Uint8 reserve1, ttl, svctype, df_flag;
    Sint16 reserve2;
    void *reserve3;
    AT_asr asraddr;
} AT_openparam;
typedef struct _AT_popenparam {
    AT_NINT32 youraddr;
    AT_NINT16 yourport, myport;
    Uint8 reserve1, ttl, svctype, df_flag;
    Sint16 reserve2;
    void *reserve3;
} AT_popenparam;
typedef void (*AT_accept_notify)(Sint16, Sint16, Uint32, Uint16, Uint16);
typedef struct _AT_acceptparam {
    Sint16 nh;
    AT_accept_notify notifyaddr;
    AT_asr asraddr;
} AT_acceptparam;
typedef struct _AT_getaddr_r {
    AT_NINT32 ipaddr;
    AT_NINT16 myport, yourport;
} AT_getaddr_r;
typedef struct _AT_nhparam {
    Sint16 nh;
} AT_nhparam;
typedef struct _AT_stat_r {
    Uint16 stat, reserve, sendwin, recvwin;
} AT_stat_r;
typedef void (*AT_send_notify)(Sint16, Sint16);
typedef void (*AT_receive_notify)(Sint16, Sint16, Sint16);
typedef void (*AT_sendf_notify)(Sint16, Sint16, Uint8 *);
typedef struct _AT_sendparam {
    Sint16 nh;
    AT_send_notify notifyaddr;
    Uint8 reserve, bufnum;
    struct {
        Sint16 len;
        Uint8 *buf;
    } buffers[AT_REQUEST_BUFFERS];
} AT_sendparam;
typedef struct _AT_rcvparam {
    Sint16 nh, len;
    Uint8 *buf;
    AT_receive_notify notifyaddr;
} AT_rcvparam;
typedef struct _AT_sendfparam {
    Sint16 nh;
    AT_sendf_notify notifyaddr;
    Uint8 reserve;
    Sint16 len;
    Uint8 *buf;
} AT_sendfparam;
typedef struct _AT_cancelparam {
    Sint16 nh, what;
} AT_cancelparam;
/* AT_cancelparam's what: the waiting requests whose notify to cancel */
#define CANCEL_SEND 1
#define CANCEL_RECEIVE 2
#define CANCEL_ACCEPT 4
typedef struct _AT_RouteEntry {
    AT_NINT32 dstaddr, gtwayaddr;
    Uint16 reserve1, reserve2, reserve3;
} AT_RouteEntry;
typedef struct _AT_lstrtparam {
    Sint16 len;
    AT_RouteEntry *buf;
} AT_lstrtparam;

Sint16 AT_apiCall(Uint16 code, void *inp, void *outp);
Sint16 AT_init(void *argument);
Sint16 AT_disp(void *argument);
#endif

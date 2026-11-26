/* the records that IP, UDP and TCP share, and the IP and UDP calls. the records
 * are the checksum's pseudo header, the route entry and its cache, and a
 * socket's addresses */
#ifndef ACCESS_IP_TYPES_H
#define ACCESS_IP_TYPES_H
#include <access/at_types.h>
#include <sg_xpt.h>

struct tcp_pseudo {
    Uint32 src;
    Uint32 dst;
    Uint8 zero;
    Uint8 proto;
    Uint16 len;
};

typedef struct tcp_route_entry {
    Uint32 dst;
    Uint32 mask;
    Uint32 gateway;
    Uint16 metric;
    Uint16 flags;
    Sint32 refcnt;
    Sint32 ifno;
    Uint8 redirect;
    Uint8 kind;
} RtEntry;
/* RtEntry's flags */
#define RTF_UP 0x01
#define RTF_GATEWAY 0x02
#define RTF_HOST 0x04
#define RTF_LOCAL 0x08
#define RTF_DYNAMIC 0x10
#define RTF_MODIFIED 0x20
/* RtEntry's kinds. RT_KIND_APPLICATION is a route the application adds.
 * RT_KIND_INTERFACE is the route to an interface's network. RT_KIND_REDIRECT is
 * a route an ICMP redirect adds. IP_RouteAdd enters a route of RT_KIND_NETWORK
 * as a network's, whatever its destination's host bits */
#define RT_KIND_APPLICATION 0
#define RT_KIND_INTERFACE 1
#define RT_KIND_REDIRECT 2
#define RT_KIND_NETWORK 3
/* an address's class, and the mask of its class's network, in host order */
#define IN_CLASSA(a) (((a) & 0x80000000) == 0)
#define IN_CLASSB(a) (((a) & 0xc0000000) == 0x80000000)
#define IN_CLASSC(a) (((a) & 0xe0000000) == 0xc0000000)
#define IN_CLASSA_NET 0xff000000
#define IN_CLASSB_NET 0xffff0000
#define IN_CLASSC_NET 0xffffff00
/* the loopback address, in host order, and the broadcast to every host */
#define INADDR_LOOPBACK 0x7f000001
#define INADDR_BROADCAST 0xffffffff

typedef struct tcp_route_cache {
    RtEntry *rt;
    Uint32 gateway;
} RtCache;

struct sock_addr {
    Uint32 addr;
    Uint16 local_port;
    Uint16 peer_port;
};

#define IP_ROUTE_ENTRIES 32
struct route_info {
    Uint32 gateway;
    Uint32 addr;
    Sint32 ifno;
    Uint32 mtu;
};
extern RtEntry route_table[IP_ROUTE_ENTRIES];
extern Uint32 icb_struct_size;
extern Sint32 tcpQueNotAvailCt;
int IP_Init(void);
int IP_ClearRoutingTable(void);
int IP_InitIfconfig(int unit);
int IP_IfDown(int unit);
int IP_CleanFragmentAll(void);
Sint32 IP_Recv(Uint8 *data, Sint32 size, Uint16 offset, Sint32 ifno);
/* IP_Chksum's count is of 16-bit words, CHECKSUM_WORD_BYTES each */
#define CHECKSUM_WORD_BYTES 2
Uint16 IP_Chksum(void *data, Sint32 count);
Sint32 IP_IsLocalloopback(Uint32 ip, Sint32 ifno);
extern Uint32 ucb_struct_size;
Sint32 UDP_Init(void);
Sint32 UDP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_size, Uint32 src,
                Uint32 dst);
/* UDP_Recv's result for a datagram to this host that no socket takes */
#define UDP_NO_SOCKET (-2)
/* tell the UDP sockets whose peer address is address that ICMP finds it
 * unreachable, and return 0. a socket whose peer port is port hears
 * ASR_ADDR_PORT_UNREACH for an unreachable port or network. one without a
 * peer port hears ASR_ADDR_UNREACH for an unreachable host or protocol */
Sint32 UDP_ComeIcmpUnreach(Uint16 port, Uint32 address, Uint8 code);
int IP_ComeIcmpRedirect(Uint32 destination, Uint32 gateway, Uint16 flags,
                        Uint32 source);
RtEntry *IP_GetRouteEntry(Uint32 destination);
int IP_GetRouteAddr(Uint32 destination, RtCache *route,
                    struct route_info *info);
/* IP_Send and DL_Send each append a buffer, for the IP and the Ethernet header.
 * buffers must have room for both */
Sint32 IP_Send(AT_MaxBuffs *buffers, Uint32 destination, Uint8 raw, Uint8 ttl,
               Uint8 service, Uint8 dont_fragment, Uint16 options_size,
               void *options, Uint8 protocol, Sint32 flags);
/* an IPv4 header's bytes without options, and the ttl IP_Send gives a
 * datagram for a ttl of 0 */
#define IP_HEADER_BYTES 20
#define IP_DEFAULT_TTL 30
/* the header's first byte holds the version above IP_VERSION_SHIFT. under
 * IP_IHL_MASK it holds the header's length, in words of IP_WORD_BYTES */
#define IP_VERSION 4
#define IP_VERSION_SHIFT 4
#define IP_IHL_MASK 0x0f
#define IP_WORD_BYTES 4
#define IP_ADDRESS_BYTES 4
void IP_FreeRoute(RtCache *route);
RtEntry *IP_RouteSearch(Uint32 destination);
Uint32 IP_GetMyaddrRoute(RtEntry *route);
Sint32 IP_IsBroadcast(Uint32 address, Sint32 interface_number);
Uint16 UDP_Chksum(struct tcp_pseudo *pseudo, AT_MaxBuffs *buffers,
                  Sint32 reverse);
#endif

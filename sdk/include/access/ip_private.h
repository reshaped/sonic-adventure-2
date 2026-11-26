/* ACCESS's UDP and raw IP sockets and their tables' sizes. the IP, ARP and
 * setup calls that the API makes follow */
#ifndef ACCESS_IP_PRIVATE_H
#define ACCESS_IP_PRIVATE_H
#include <access/at_api.h>
#include <access/at_types.h>
#include <access/ip_types.h>
#include <sg_xpt.h>
#include <access/stack_globals.h> /* IWYU pragma: export */

#define AT_TCP_SOCKETS 8
#define AT_UDP_SOCKETS 4
#define AT_IP_SOCKETS 1
#define AT_MEMORY_POOLS 10
#define AT_POOL_BYTES 8192

struct udp_hdr {
    Uint16 sport, dport, len, sum;
};
struct ucb {
    RtCache route;
    Sint16 in_use;
    Uint8 reserved;
    Sint8 raw;
    struct udp_hdr hdr;
    Uint8 has_faddr, has_fport;
    Uint32 faddr;
    Sint16 options_size;
    void *options;
    AT_udp_receiver receiver;
    Sint32 asr;
    Uint8 svctype, df_flag, ttl;
};
struct net_icb {
    Uint16 state;
    Uint32 addr;
    AT_udp_receiver receiver;
    Sint32 asr;
    Uint8 protocol;
};

Sint32 UDP_Open(Uint32 destination, Uint16 peer_port, Uint16 local_port,
                Sint8 raw, Uint8 ttl, Uint8 service, Uint8 dont_fragment,
                Sint16 options_size, void *options, Sint32 asr,
                AT_udp_receiver receiver);
Sint32 UDP_SetASR(Sint32 socket, Sint32 asr);
Sint32 UDP_Close(Sint32 socket);
Sint32 UDP_ApiGetaddr(Sint32 socket, struct sock_addr *address);
/* UDP_Send appends a buffer for the UDP header. IP_Send and DL_Send each append
 * one for theirs, so packet must have room for three more */
Sint32 UDP_Send(AT_MaxBuffs *packet, Uint32 destination, Uint16 port,
                Sint32 socket);
Sint32 IP_Open(Uint32 address, Uint8 protocol, Sint32 asr,
               AT_udp_receiver receiver);
Sint32 IP_Close(Sint32 socket);
Sint32 IP_SetASR(Sint32 socket, Sint32 asr);
Sint32 IP_ApiSend(AT_MaxBuffs *packet, Sint16 socket, Sint8 raw, Uint8 ttl,
                  Uint8 service, Uint8 dont_fragment, Uint16 options_size,
                  void *options);
/* the two take and return addresses and masks in network order.
 * IP_SetNetmask returns mask with the bits of address's class network set, or
 * 0 for address 0. an address of no class A, B or C leaves mask as it is.
 * IP_SetNetmaskRoute returns the mask of destination's class network, class
 * C's for a class past C. where destination lies in interface_address's
 * network under that mask, it returns mask */
Uint32 IP_SetNetmask(Uint32 mask, Uint32 address);
Uint32 IP_SetNetmaskRoute(Uint32 destination, Uint32 mask,
                          Uint32 interface_address);
int IP_RouteAdd(Uint32 destination, Uint32 mask, Uint32 gateway, Uint16 metric,
                Uint8 kind, Uint16 flags);
int IP_RouteDel(Uint32 destination, Uint32 mask, Uint32 gateway, Uint8 kind);
Uint16 IP_RouteList(Uint16 size, AT_Vptr *destination, Uint8 remove);
void IP_Timer(void);
void ARP_UpdateTable(void);
Sint32 ARP_TableAddApi(Uint32 ip, Uint8 *hardware, Sint16 flags);
Sint32 ARP_TableDelApi(Uint32 ip, Uint8 *hardware);
Uint16 ARP_ArpList(Uint16 size, AT_Vptr *destination);
int INIT_All(void);
int INIT_IfconfigUp(Uint32 address, Uint32 mask, Uint32 broadcast_address,
                    int interface_number);
int INIT_IfconfigDown(int interface_number);
Sint32 DL_Exit(void);
Sint32 DL_Recv(Uint8 *packet, Sint32 length, Sint32 interface_number);
#endif

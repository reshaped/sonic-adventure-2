/* the data link: its interfaces' records, and the calls of the data link, ARP
 * and ICMP */
#ifndef ACCESS_DATALINK_H
#define ACCESS_DATALINK_H
#include <sg_xpt.h>
#include <access/at_types.h>

struct net_iface {
    Uint8 kind;
    Uint8 state;
    Uint8 pad2[2];
    Sint32 mtu;
    Uint32 addr;
    Uint32 broadcast;
    Uint32 network;
    Uint32 mask;
    Uint16 htype;
    Uint8 hlen;
    Uint8 pad_1b;
    Uint8 haddr[6];
    Sint8 bcast[6];
    Uint8 pad_28[4];
    void *statistics;
};
/* net_iface's kind for an interface whose frames go through the packet
 * driver */
#define DL_KIND_PKTDRV 1
/* net_iface's state while its interface is down, and while it is up */
#define DL_IF_DOWN 1
#define DL_IF_UP 2
/* net_iface's htype for Ethernet, whose addresses are ETHERNET_ADDRESS_BYTES
 * long */
#define DL_TYPE_ETHER 1
#define ETHERNET_ADDRESS_BYTES 6
/* an Ethernet header: the destination's and the source's addresses, then
 * the type */
#define ETHERNET_TYPE_OFFSET (ETHERNET_ADDRESS_BYTES * 2)
#define ETHERNET_TYPE_BYTES 2
#define ETHERNET_HEADER_BYTES (ETHERNET_TYPE_OFFSET + ETHERNET_TYPE_BYTES)
/* the frame types DL_Send takes */
#define DL_PROTO_IP 0
#define DL_PROTO_ARP 4
/* the Ethernet types of an IPv4 packet and of an ARP packet */
#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806

struct net_iface *DL_GetIfEntry(Sint32 index);
struct net_iface *DL_GetIfEntryFirst(Sint32 index);
extern Sint32 arp_timer_1sec;
extern volatile Sint32 icmp_timer_1sec;
Sint32 ARP_Init(void);
Sint32 ARP_Recv(Uint8 *data, Sint32 size, Uint16 offset, Sint32 ifno);
Sint32 ARP_PktSend(AT_MaxBuffs *buffers, Uint32 ip, Sint32 ifno);
Sint32 DL_Init(void);
Sint32 DL_IfUp(Uint32 address, Uint32 mask, Uint32 broadcast_address,
               Sint32 interface_number);
Sint32 DL_IfDown(Sint32 interface_number);
Sint32 DL_IfSetMask(Uint32 mask, Sint32 interface_number);
Sint32 DL_IfStat(Sint32 interface_number);
void *DL_GetCardStatistics(void);
Uint32 DL_GetIpAddress(void);
/* DL_Send appends a buffer for the Ethernet header, so buffers must have
 * room for one more */
Sint32 DL_Send(AT_MaxBuffs *buffers, Uint8 *hardware, Uint16 type, Sint32 ifno);
Sint32 DL_MatchMyIpaddr(Uint32 ip);
Sint32 ICMP_Recv(AT_MaxBuffs *buffers, Uint16 offset, Uint32 source);
Sint32 ICMP_GetSubnetMask(Uint32 *mask, Uint32 destination);
/* ICMP_Send can append a buffer for the ICMP header. IP_Send and DL_Send each
 * append one for theirs, so buffers must have room for three more */
Sint32 ICMP_Send(Uint8 type, Uint8 code, Uint32 destination,
                 AT_MaxBuffs *buffers);
/* ICMP_Send's type and codes for a protocol or a port no socket takes */
#define ICMP_UNREACH 3
#define ICMP_UNREACH_PROTOCOL 2
#define ICMP_UNREACH_PORT 3
/* ICMP_UNREACH's codes for a network and a host that can't be reached */
#define ICMP_UNREACH_NET 0
#define ICMP_UNREACH_HOST 1
#endif

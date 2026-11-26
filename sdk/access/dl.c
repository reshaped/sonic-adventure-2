/* the data link: its interfaces' records, and the Ethernet frames it sends
 * and reads */
#include <access/at_types.h>
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/pktdrv.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>
#define DL_IF_MAX 4
/* the headers dl_headers holds, of which dl_alloc_header lends the first
 * alone */
#define DL_HEADERS 4

struct ethernet_header {
    Uint8 dest[ETHERNET_ADDRESS_BYTES];
    Uint8 source[ETHERNET_ADDRESS_BYTES];
    Uint16 type;
    Uint16 reserve;
};

struct ethernet_header_pool {
    struct ethernet_header header[DL_HEADERS];
    Sint32 used;
};

static struct ethernet_header_pool dl_headers;
static struct net_iface if_table[DL_IF_MAX];
static Sint32 if_table_num;
static struct ethernet_header dl_header_wk;

Sint32 DL_Init(void)
{
    int ifnum;

    if ((if_table_num = PKTDRV_GetIfnum()) == -1) {
        return -1;
    }
    for (ifnum = 0; if_table_num > ifnum; ifnum++) {
        if (PKTDRV_IfInit(ifnum) != 0) {
            return -1;
        }
    }
    return 0;
}

Sint32 DL_Exit(void)
{
    int count;
    int ifnum;

    if ((count = PKTDRV_GetIfnum()) == -1) {
        return -1;
    }
    for (ifnum = 0; count > ifnum; ifnum++) {
        if (PKTDRV_IfDown(ifnum) != 0) {
            return -1;
        }
    }
    return 0;
}

Sint32 DL_IfUp(Uint32 address, Uint32 mask, Uint32 broadcast_address,
               Sint32 interface_number)
{
#ifdef NONMATCHING
    /* this side refuses an interface number outside if_table's if_table_num
     * entries, which the matching side indexes unchecked */
    if (interface_number < 0 || interface_number >= if_table_num) {
        return -1;
    }
#endif
    if (if_table[interface_number].state == DL_IF_UP) {
        return -1;
    }
    if_table[interface_number].addr = address;
    if_table[interface_number].network = address & mask;
    if_table[interface_number].broadcast = broadcast_address;
    if_table[interface_number].mask = mask;
    if (PKTDRV_IfUp(interface_number) != 0) {
        return -1;
    }
    return 0;
}

Sint32 DL_IfSetMask(Uint32 mask, Sint32 interface_number)
{
#ifdef NONMATCHING
    /* this side refuses an interface number outside if_table's if_table_num
     * entries, which the matching side indexes unchecked */
    if (interface_number < 0 || interface_number >= if_table_num) {
        return -1;
    }
#endif
    if (if_table[interface_number].state == DL_IF_UP) {
        if_table[interface_number].mask = mask;
    } else {
        return -1;
    }
    return 0;
}

Sint32 DL_IfDown(Sint32 interface_number)
{
    if (PKTDRV_IfDown(interface_number) != 0) {
        return -1;
    }
    return 0;
}

Sint32 DL_IfStat(Sint32 interface_number)
{
#ifdef NONMATCHING
    /* this side refuses an interface number outside if_table's if_table_num
     * entries, which the matching side indexes unchecked */
    if (interface_number < 0 || interface_number >= if_table_num) {
        return -1;
    }
#endif
    return if_table[interface_number].state;
}

Sint32 DL_MatchMyIpaddr(Uint32 ip)
{
    int count;
    int ifnum;

    if ((count = PKTDRV_GetIfnum()) == -1) {
        return -1;
    }
    for (ifnum = 0; count > ifnum; ifnum++) {
        if (if_table[ifnum].addr == ip) {
            return ifnum;
        }
    }
    return -1;
}

struct net_iface *DL_GetIfEntry(Sint32 index)
{
#ifdef NONMATCHING
    /* this side returns NULL for a negative index, which the matching side's
     * test against if_table_num lets through */
    if (index < 0) {
        return NULL;
    }
#endif
    if (if_table_num > index) {
        if (if_table[index].state == DL_IF_UP) {
            return &if_table[index];
        }
    }
    return NULL;
}

struct net_iface *DL_GetIfEntryFirst(Sint32 index)
{
#ifdef NONMATCHING
    /* this side returns NULL for a negative index, which the matching side's
     * test against if_table_num lets through */
    if (index < 0) {
        return NULL;
    }
#endif
    if (if_table_num > index) {
        return &if_table[index];
    }
    return NULL;
}

static struct ethernet_header *dl_alloc_header(void)
{
    return dl_headers.header;
}

static int dl_free_header(struct ethernet_header *header)
{
    /* @hack redundant: the test of header, whose two ways both return 0 */
    if (header == NULL) {
        return 0;
    }
    return 0;
}

Sint32 DL_Send(AT_MaxBuffs *buffers, Uint8 *hardware, Uint16 type, Sint32 ifno)
{
    Sint32 count;
    struct ethernet_header *header;
    struct net_iface *entry;

#ifdef NONMATCHING
    /* this side refuses a missing list or address, an interface outside
     * if_table, a list with no room for the header buffer the send adds, and a
     * hardware address longer than the header's */
    if (!buffers || !hardware || ifno < 0 || ifno >= if_table_num ||
        buffers->bufnum < 0 || buffers->bufnum >= AT_MAX_BUFFERS ||
        if_table[ifno].hlen > ETHERNET_ADDRESS_BYTES) {
        return -1;
    }
#endif
    if ((header = dl_alloc_header()) == NULL) {
        return -1;
    }
    entry = &if_table[ifno];
    switch (entry->htype) {
    case DL_TYPE_ETHER:
        memcpy(header->source, entry->haddr, entry->hlen);
        memcpy(header->dest, hardware, entry->hlen);
        if (type == DL_PROTO_ARP) {
            header->type = OS_Htons(ETHERTYPE_ARP);
        } else if (type == DL_PROTO_IP) {
            header->type = OS_Htons(ETHERTYPE_IP);
        } else {
            dl_free_header(header);
            return -1;
        }
        break;
    default:
        dl_free_header(header);
        return -1;
    }
    count = buffers->bufnum;
    memcpy(dl_header_wk.dest, header->dest, ETHERNET_ADDRESS_BYTES);
    memcpy(dl_header_wk.source, header->source, ETHERNET_ADDRESS_BYTES);
    memcpy(&dl_header_wk.type, &header->type, ETHERNET_TYPE_BYTES);
    OS_SetVptrLocal(&buffers->buffers[count].vptr, &dl_header_wk);
    buffers->buffers[count].len = ETHERNET_HEADER_BYTES;
    buffers->bufnum++;
    if (PKTDRV_Send(buffers, ifno) != 0) {
        dl_free_header(header);
        return -1;
    }
    dl_free_header(header);
    return 0;
}

Sint32 DL_Recv(Uint8 *packet, Sint32 length, Sint32 interface_number)
{
    /* @hack spelling: data and size, copies of packet and length, where the
     * parameters would do */
    Uint8 *data = packet;
    Sint32 size = length;
    struct ethernet_header header;
    struct ethernet_header *hp;
    Uint16 proto;

#ifdef NONMATCHING
    /* this side drops a missing frame, an interface outside if_table, and a
     * frame too short for the Ethernet header the matching side copies */
    if (!packet || interface_number < 0 || interface_number >= if_table_num ||
        size < ETHERNET_HEADER_BYTES) {
        return -1;
    }
#endif
    switch (if_table[interface_number].htype) {
    case DL_TYPE_ETHER:
        memcpy(header.dest, packet, ETHERNET_ADDRESS_BYTES);
        memcpy(header.source, packet + ETHERNET_ADDRESS_BYTES,
               ETHERNET_ADDRESS_BYTES);
        memcpy(&header.type, packet + ETHERNET_TYPE_OFFSET,
               ETHERNET_TYPE_BYTES);
        hp = &header;
        if (hp->type == OS_Htons(ETHERTYPE_ARP)) {
            proto = DL_PROTO_ARP;
        } else if (hp->type == OS_Htons(ETHERTYPE_IP)) {
            proto = DL_PROTO_IP;
        } else {
            return -1;
        }
        break;
    default:
        return -1;
    }
    switch (proto) {
    case DL_PROTO_IP:
        if (IP_Recv(data, size, ETHERNET_HEADER_BYTES, interface_number) != 0) {
            return -1;
        }
        return 0;
    case DL_PROTO_ARP:
        if (ARP_Recv(data, size, ETHERNET_HEADER_BYTES, interface_number) !=
            0) {
            return -1;
        }
        return 0;
    }
    return -1;
}

void *DL_GetCardStatistics(void)
{
    struct net_iface *entry;

    entry = DL_GetIfEntry(0);
#ifdef NONMATCHING
    /* this side returns NULL while interface 0 is down, where the matching side
     * reads through DL_GetIfEntry's null */
    if (!entry) {
        return NULL;
    }
#endif
    return entry->statistics;
}

Sint32 DL_CheckSend(void)
{
    return 0;
}

Uint32 DL_GetIpAddress(void)
{
    static Uint32 ipaddr;

    memcpy(&ipaddr, &if_table[0].addr, sizeof(ipaddr));
    return ipaddr;
}

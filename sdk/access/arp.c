/* the ARP table, which resolves IPv4 addresses to Ethernet addresses, and
 * the ARP packets that fill it */
#include "access/ip_types.h"
#include <access/at_types.h>
#include <access/cardprobe.h>
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/packet_queue.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>

#define ARP_TABLE_SIZE 19
/* an entry's flags, ARP_FREE for a free entry. an ARP_STATIC entry never
 * expires, and no new entry evicts it */
#define ARP_FREE 0
#define ARP_PENDING 1
#define ARP_RESOLVED 2
#define ARP_STATIC 4
/* an entry's timer: the calls of ARP_UpdateTable it outlives after it is
 * set or found. the search for the oldest entry starts above any timer, at
 * ARP_TIMER_LIMIT */
#define ARP_RESOLVED_LIFE 20
#define ARP_PENDING_LIFE 3
#define ARP_TIMER_LIMIT 0xff
/* an ARP packet's op: a request, or a reply */
#define ARP_REQUEST 1
#define ARP_REPLY 2
/* an ARP packet's bytes before its addresses */
#define ARP_HEADER_BYTES 8
/* @bug logic: the length counts one byte past the ARP packet, which is
 * ARP_HEADER_BYTES and two addresses of each kind. arp_send_request and
 * arp_send_reply send that byte (0x0cad6b6e, 0x0cad6c70) */
#define ARP_SENT_BYTES(hlen)                                                   \
    ((((hlen) + IP_ADDRESS_BYTES) * 2) + (ARP_HEADER_BYTES + 1))

/* a frame held for a pending entry: len bytes at pointer, in the pool. next
 * is always NULL */
struct arp_hold {
    Sint16 len;
    AT_Vptr *pointer;
    struct arp_hold *next;
};

struct arp_entry {
    Uint8 flags;
    Uint8 hlen;
    Uint16 htype;
    Sint32 ifno;
    struct arp_hold *head;
    struct arp_hold *tail;
    Uint8 timer;
    Uint32 ip;
    Uint8 haddr[8];
};

/* an ARP packet. addrs holds its sender's hardware and IPv4 addresses, then
 * its target's, of hlen and plen bytes */
struct arp_packet {
    Uint16 htype;
    Uint16 ptype;
    Uint8 hlen;
    Uint8 plen;
    Uint16 op;
    Uint8 addrs[32];
};

/* a row of the list ARP_ArpList writes */
struct arp_record {
    Uint32 ip;
    Uint8 haddr[ETHERNET_ADDRESS_BYTES];
    Uint8 timer;
    Uint8 flags;
};

Sint32 arp_timer_1sec;
static struct arp_entry arp_table[ARP_TABLE_SIZE];
static struct arp_packet arp_send_buff;
/* whether the hold's pool buffer is allocated, and the buffer */
static Bool arp_hold_allocated;
static AT_Vptr arp_hold_buffer;

static struct arp_entry *arp_new_entry(Uint32 ip, Sint32 ifno);
static struct arp_entry *arp_set_entry(Uint32 ip, Sint16 htype, Uint8 hlen,
                                       Uint8 *haddr, Uint8 flags, Sint32 ifno);
static Sint32 arp_free_entry(Sint32 index);
static Sint32 arp_flush(struct arp_entry *entry);
static Sint32 arp_send_request(Uint32 ip, Sint32 ifno);
static Sint32 arp_send_reply(Uint8 *haddr, Uint32 ip, Sint32 ifno);
static struct arp_hold *arp_get_hold(void);
static Sint32 arp_free_hold(struct arp_hold *hold);
static Sint32 arp_hold_packet(AT_MaxBuffs *buffers, struct arp_entry *entry);
static Sint32 arp_drop_pending(struct arp_entry *entry);

Sint32 ARP_Init(void)
{
    Sint32 index;
    struct arp_entry *entry;

    arp_hold_allocated = FALSE;
    for (index = 0; index < ARP_TABLE_SIZE; index++) {
        entry = &arp_table[index];
        entry->flags = ARP_FREE;
        entry->head = entry->tail = NULL;
    }
    return 0;
}

/* the entry for ip with any of flags set, its timer renewed, or NULL */
static struct arp_entry *arp_find(Uint32 ip, Uint8 flags)
{
    Sint32 index;
    struct arp_entry *entry;

    for (index = 0; index < ARP_TABLE_SIZE; index++) {
        entry = &arp_table[index];
        if ((entry->flags & flags) && entry->ip == ip) {
            if (entry->flags & ARP_RESOLVED) {
                entry->timer = ARP_RESOLVED_LIFE;
            } else if (entry->flags & ARP_PENDING) {
                entry->timer = ARP_PENDING_LIFE;
            }
            return entry;
        }
    }
    return NULL;
}

Sint32 ARP_PktSend(AT_MaxBuffs *buffers, Uint32 ip, Sint32 ifno)
{
    struct net_iface *interface;
    struct arp_entry *entry;
    Uint8 haddr[ETHERNET_ADDRESS_BYTES];

    if ((interface = DL_GetIfEntry(ifno)) == NULL) {
        return -1;
    }
    if (IP_IsBroadcast(ip, ifno)) {
        if (DL_Send(buffers, (Uint8 *)interface->bcast, DL_PROTO_IP, ifno) ==
            0) {
            return 0;
        }
        goto error;
    }
    switch (AT_CardType) {
    case AT_CARD_TYPE_LAN:
        if ((entry = arp_find(ip, ARP_RESOLVED)) != NULL) {
            if (DL_Send(buffers, entry->haddr, DL_PROTO_IP, ifno) != 0) {
                return -1;
            }
            return 0;
        }
        break;
    case AT_CARD_TYPE_PPP:
        memset(haddr, 0, sizeof(haddr));
        if (DL_Send(buffers, haddr, DL_PROTO_IP, ifno) != 0) {
            return -1;
        }
        return 0;
    default:
        return -1;
    }
    if ((entry = arp_find(ip, ARP_PENDING)) == NULL) {
        if ((entry = arp_new_entry(ip, ifno)) == NULL) {
            return -1;
        }
    }
    if (arp_hold_packet(buffers, entry) == -1) {
        return -1;
    }
    if (arp_send_request(ip, ifno) == -1) {
        return -1;
    }
    /* @hack spelling: the jumps to these two returns, here and from the
     * broadcast's failure, where a return in place of each would do */
    goto success;
error:
    return -1;
success:
    return 0;
}

Sint32 ARP_Recv(Uint8 *data, Sint32 size, Uint16 offset, Sint32 ifno)
{
    Uint32 sender_ip;
    Uint32 target_ip;
    struct net_iface *interface;
    struct arp_packet *packet;
    Uint8 *p;
    Uint8 *sender_haddr;
    struct arp_entry *entry;

    if ((interface = DL_GetIfEntry(ifno)) == NULL) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side drops a packet too short to hold offset and an ARP header,
     * which the matching side reads past */
    if (size < offset || size - offset < ARP_HEADER_BYTES) {
        return -1;
    }
#endif
    data += offset;
    packet = (struct arp_packet *)data;
#ifdef NONMATCHING
    /* this side drops a packet whose hardware address is longer than an entry's
     * haddr, or whose four addresses run past its end */
    if (packet->hlen > sizeof(entry->haddr) ||
        size - offset - ARP_HEADER_BYTES < 2 * (packet->hlen + packet->plen)) {
        return -1;
    }
#endif
    if (OS_Htons(packet->ptype) != ETHERTYPE_IP) {
        return -1;
    }
    if (packet->plen != IP_ADDRESS_BYTES) {
        return -1;
    }
    p = packet->addrs;
    sender_haddr = p;
    p += packet->hlen;
    memcpy(&sender_ip, p, sizeof(sender_ip));
    p += packet->plen;
    p += packet->hlen;
    memcpy(&target_ip, p, sizeof(target_ip));
    if (target_ip != interface->addr) {
        return -1;
    }
    if (OS_Htons(packet->op) == ARP_REQUEST) {
        if ((entry = arp_find(sender_ip, ARP_PENDING | ARP_RESOLVED)) != NULL) {
            if (entry->flags & ARP_PENDING) {
                memcpy(entry->haddr, sender_haddr, entry->hlen);
                entry->flags &= ~ARP_PENDING;
                entry->flags |= ARP_RESOLVED;
                if (arp_flush(entry) != 0) {
                    return -1;
                }
            }
        } else if (arp_set_entry(sender_ip, packet->htype, packet->hlen,
                                 sender_haddr, 0, ifno) == NULL) {
            return -1;
        }
        if (arp_send_reply(sender_haddr, sender_ip, ifno) == 0) {
            return 0;
        }
        return -1;
    } else if (OS_Htons(packet->op) == ARP_REPLY) {
        if ((entry = arp_find(sender_ip, ARP_PENDING)) == NULL) {
            return 0;
        }
        memcpy(entry->haddr, sender_haddr, entry->hlen);
        entry->flags &= ~ARP_PENDING;
        entry->flags |= ARP_RESOLVED;
        if (arp_flush(entry) == 0) {
            return 0;
        }
    }
    return -1;
}

static struct arp_entry *arp_new_entry(Uint32 ip, Sint32 ifno)
{
    struct net_iface *interface;
    struct arp_entry *entry;
    Sint32 index;
    Sint32 oldest;
    Uint8 age;
    Sint32 found;

    age = ARP_TIMER_LIMIT;
    found = 0;
    if ((interface = DL_GetIfEntry(ifno)) == NULL) {
        return NULL;
    }
    oldest = -1;
    index = 0;
    do {
        entry = &arp_table[index];
        if (entry->flags == ARP_FREE) {
            found = 1;
            break;
        } else if ((entry->flags & (ARP_PENDING | ARP_RESOLVED)) &&
                   !(entry->flags & ARP_STATIC) && entry->timer < age) {

            age = entry->timer;
            oldest = index;
        }
    } while (++index < ARP_TABLE_SIZE);

    if (found != 1 && oldest != -1) {
        index = oldest;
        entry = &arp_table[index];
        arp_free_entry(index);
        found = 1;
    }
    if (found == 1) {
        entry->flags = ARP_PENDING;

        entry->ifno = ifno;
        if (interface->htype == DL_TYPE_ETHER) {
            entry->htype = DL_TYPE_ETHER;
        } else {
            entry->htype = 0;
        }

        entry->hlen = interface->hlen;
        entry->timer = ARP_PENDING_LIFE;
        entry->ip = ip;
        return entry;
    }
    return NULL;
}

static struct arp_entry *arp_set_entry(Uint32 ip, Sint16 htype, Uint8 hlen,
                                       Uint8 *haddr, Uint8 flags, Sint32 ifno)
{
    struct arp_entry *entry;
    struct arp_entry *p;
    Sint32 index;
    Sint32 selected;
    Sint32 oldest;
    Uint8 age;
    Sint32 found;
    Sint32 pending;

    found = 0;
    age = ARP_TIMER_LIMIT;
    pending = 0;
    oldest = -1;
    for (index = 0; index < ARP_TABLE_SIZE; index++) {
        p = &arp_table[index];
        if (p->flags == ARP_FREE) {
            selected = index;
            entry = &arp_table[selected];
            found = 1;
            continue;
        }
        if (p->ip == ip) {
            if (!(p->flags & ARP_PENDING) || (p->flags & ARP_RESOLVED)) {
                return NULL;
            }
            selected = index;
            entry = &arp_table[selected];
            found = pending = 1;
            break;
        } else if ((p->flags & (ARP_PENDING | ARP_RESOLVED)) &&
                   !(p->flags & ARP_STATIC) && p->timer < age) {

            age = p->timer;
            oldest = index;
        }
    }
    if (found != 1 && oldest != -1) {
        selected = oldest;
        entry = &arp_table[selected];
        arp_free_entry(selected);
        found = 1;
    }
    if (found == 1) {
        entry->flags = flags | ARP_RESOLVED;
        entry->ifno = ifno;
        entry->htype = htype;
        entry->hlen = hlen;
        entry->timer = ARP_RESOLVED_LIFE;
        memcpy(entry->haddr, haddr, hlen);
        entry->ip = ip;
        if (pending == 1) {
            entry->flags &= ~ARP_PENDING;
            arp_flush(entry);
        }
        return entry;
    }
    return NULL;
}

void ARP_UpdateTable(void)
{
    Sint32 index;
    struct arp_entry *entry;

    for (index = 0; index < ARP_TABLE_SIZE; index++) {
        entry = &arp_table[index];
        if ((entry->flags & (ARP_PENDING | ARP_RESOLVED)) &&
            !(entry->flags & ARP_STATIC)) {
            if (!entry->timer) {
                arp_free_entry(index);
                continue;
            }
            /* @hack redundant: the test of a timer the test above found
             * nonzero */
            if (entry->timer != 0) {
                entry->timer--;
            }
        }
    }
}

Sint32 ARP_TableAddApi(Uint32 ip, Uint8 *hardware, Sint16 flags)
{
    Sint32 ifno = 0;

    if (arp_set_entry(ip, DL_TYPE_ETHER, ETHERNET_ADDRESS_BYTES, hardware,
                      flags, ifno) == NULL) {
        return -1;
    }
    return 0;
}

Sint32 ARP_TableDelApi(Uint32 ip, Uint8 *hardware)
{
    Sint32 index;
    struct arp_entry *entry;

    for (index = 0; index < ARP_TABLE_SIZE; index++) {
        entry = &arp_table[index];
        if (entry->ip == ip && entry->flags != ARP_FREE) {
            arp_free_entry(index);
            return 0;
        }
    }
    return -1;
}

static Sint32 arp_free_entry(Sint32 index)
{
    arp_drop_pending(&arp_table[index]);
    arp_table[index].flags = ARP_FREE;
    return 0;
}

Uint16 ARP_ArpList(Uint16 size, AT_Vptr *destination)
{
    struct arp_record row;
    AT_Vptr local;
    struct arp_entry *entry;
    Sint32 index;
    Sint32 count;

    count = 0;
    index = 0;
    /* @hack redundant: the test of size against 0, which the test against
     * the row's size implies */
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    while (size != 0 && size >= sizeof(struct arp_record)) {
        entry = &arp_table[index];
        while (index < ARP_TABLE_SIZE && entry->flags == ARP_FREE) {
            index++;
            entry = &arp_table[index];
        }
        if (index >= ARP_TABLE_SIZE) {
            break;
        }
        entry = &arp_table[index];
        row.ip = entry->ip;
        row.haddr[0] = entry->haddr[0];
        row.haddr[1] = entry->haddr[1];
        row.haddr[2] = entry->haddr[2];
        row.haddr[3] = entry->haddr[3];
        row.haddr[4] = entry->haddr[4];
        row.haddr[5] = entry->haddr[5];
        row.timer = entry->timer;
        row.flags = entry->flags | ARP_PENDING;
        OS_SetVptrLocal(&local, &row);
        OS_Vmemcpy(destination, &local, sizeof(struct arp_record));
        OS_IncVptr(destination, sizeof(struct arp_record));
        count++;
        index++;
        size -= sizeof(struct arp_record);
    }
    return count;
}

static Sint32 arp_flush(struct arp_entry *entry)
{
    AT_MaxBuffs *buffers;
    struct arp_hold *hold;

    if (entry->head == NULL) {
        return 0;
    }
    if ((buffers = OS_AllocBuffs()) == NULL) {
        return -1;
    }
    while ((hold = entry->head) != NULL) {
        buffers->bufnum = 1;
        buffers->buffers[0].len = hold->len;
        memcpy(&buffers->buffers[0].vptr, hold->pointer, sizeof(AT_Vptr));
        if (DL_Send(buffers, entry->haddr, DL_PROTO_IP, entry->ifno) != 0) {
            OS_FreeBuffs(buffers);
            return -1;
        }
        if (arp_drop_pending(entry) != 0) {
            return -1;
        }
    }
    OS_FreeBuffs(buffers);
    return 0;
}

static Sint32 arp_send_request(Uint32 ip, Sint32 ifno)
{
    struct net_iface *interface;
    AT_MaxBuffs *buffers;
    Uint8 *p;
    Uint8 index;
    struct arp_packet *packet;

    if ((interface = DL_GetIfEntry(ifno)) == NULL) {
        return -1;
    }
    if ((buffers = OS_AllocBuffs()) == NULL) {
        return -1;
    }
    packet = &arp_send_buff;
    packet->htype = OS_Htons(interface->htype);
    packet->ptype = OS_Htons(ETHERTYPE_IP);
    packet->hlen = interface->hlen;
    packet->plen = IP_ADDRESS_BYTES;
    packet->op = OS_Htons(ARP_REQUEST);
    p = packet->addrs;
    memcpy(p, interface->haddr, interface->hlen);
    p += interface->hlen;
    memcpy(p, &interface->addr, IP_ADDRESS_BYTES);
    p += IP_ADDRESS_BYTES;
    for (index = 0; index < interface->hlen; index++) {
        *p = 0;
        p++;
    }
    memcpy(p, &ip, IP_ADDRESS_BYTES);
    buffers->bufnum = 1;
    buffers->buffers[0].len = ARP_SENT_BYTES(interface->hlen);
    OS_SetVptrLocal(&buffers->buffers[0].vptr, &arp_send_buff);
    if (DL_Send(buffers, (Uint8 *)interface->bcast, DL_PROTO_ARP, ifno) != 0) {
        OS_FreeBuffs(buffers);
        return -1;
    }
    OS_FreeBuffs(buffers);
    return 0;
}

static Sint32 arp_send_reply(Uint8 *haddr, Uint32 ip, Sint32 ifno)
{
    struct net_iface *interface;
    AT_MaxBuffs *buffers;
    Uint8 *p;
    struct arp_packet *packet;

    if ((interface = DL_GetIfEntry(ifno)) == NULL) {
        return -1;
    }
    if ((buffers = OS_AllocBuffs()) == NULL) {
        return -1;
    }
    packet = &arp_send_buff;
    packet->htype = OS_Htons(interface->htype);
    packet->ptype = OS_Htons(ETHERTYPE_IP);
    packet->hlen = interface->hlen;
    packet->plen = IP_ADDRESS_BYTES;
    packet->op = OS_Htons(ARP_REPLY);
    p = packet->addrs;
    memcpy(p, interface->haddr, interface->hlen);
    p += interface->hlen;
    memcpy(p, &interface->addr, IP_ADDRESS_BYTES);
    p += IP_ADDRESS_BYTES;
    memcpy(p, haddr, interface->hlen);
    /* @hack spelling: the sum assigned to p, where p += interface->hlen would
     * do */
    p = p + interface->hlen;
    memcpy(p, &ip, IP_ADDRESS_BYTES);
    buffers->bufnum = 1;
    buffers->buffers[0].len = ARP_SENT_BYTES(interface->hlen);
    OS_SetVptrLocal(&buffers->buffers[0].vptr, &arp_send_buff);
    if (DL_Send(buffers, haddr, DL_PROTO_ARP, ifno) != 0) {
        OS_FreeBuffs(buffers);
        return -1;
    }
    OS_FreeBuffs(buffers);
    return 0;
}

static struct arp_hold *arp_get_hold(void)
{
    static struct arp_hold arp_hold;

    if (!arp_hold_allocated) {
        if (MP_Alloc(&arp_hold_buffer, ETHERNET_MTU, MP_ARP_HOLD) == 0) {
            arp_hold_allocated = TRUE;
        } else {
            return NULL;
        }
    }
    arp_hold.pointer = &arp_hold_buffer;
    arp_hold.next = NULL;
    return &arp_hold;
}

static Sint32 arp_free_hold(struct arp_hold *hold)
{
    if (hold != NULL) {
        if (hold->pointer != NULL) {
            /* @bug memory: frees the hold's buffer while the heads of other
             * pending entries still name it. arp_flush sends from the freed
             * buffer where one of them resolves before another frame is held
             * (0x0cad6d2e, 0x0cad6a70) */
            MP_Free(hold->pointer);
            arp_hold_allocated = FALSE;
        }
        return 0;
    }
    return -1;
}

static Sint32 arp_hold_packet(AT_MaxBuffs *buffers, struct arp_entry *entry)
{
    struct arp_hold *hold;
    AT_Vptr pointer;
    Sint32 n;
    Sint32 total;

    if ((hold = arp_get_hold()) == NULL) {
        return -1;
    }
    n = buffers->bufnum;
    total = 0;
    memcpy(&pointer, hold->pointer, sizeof(AT_Vptr));
    for (; n > 0; n--) {
        OS_Vmemcpy(&pointer, &buffers->buffers[n - 1].vptr,
                   buffers->buffers[n - 1].len);
        OS_IncVptr(&pointer, buffers->buffers[n - 1].len);
        total += buffers->buffers[n - 1].len;
    }
    hold->len = total;
    /* @bug logic: every pending entry takes the one hold arp_get_hold gives,
     * so the frame held last goes to the first of them to resolve, whichever
     * entry it was for. arp_flush sends it (0x0cad6dc8, 0x0cad6a70) */
    entry->tail = entry->head = hold;

    return 0;
}

static Sint32 arp_drop_pending(struct arp_entry *entry)
{
    if (arp_free_hold(entry->head) != 0) {
        return -1;
    }
    entry->head = entry->tail = NULL;
    return 0;
}

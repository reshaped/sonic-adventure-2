/* IPv4: raw IP sockets, the sending and receiving of datagrams, and the
 * reassembly of fragments. a datagram to this host goes back through the
 * receive queue, not out to the data link */
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <access/at_types.h>
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/packet_queue.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>

/* the offsets of struct ip_header's src and dst */
#define IP_SRC_OFFSET 12
#define IP_DST_OFFSET 16
/* the fragment field: don't fragment, more fragments, and the offset, which
 * counts FRAGMENT_UNIT_BYTES */
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
#define FRAGMENT_UNIT_BYTES 8
/* the bytes of a datagram's data an ICMP error carries after its header */
#define QUOTED_DATA_BYTES 8
/* a raw IP socket's state */
#define ICB_CLOSED 0
#define ICB_OPEN 1
/* the datagrams in reassembly at once, and the fragments they hold */
#define REASSEMBLIES 4
#define FRAGMENTS 32

/* a fragment held for reassembly: length bytes of the datagram's data, at
 * offset, which counts FRAGMENT_UNIT_BYTES. the bytes are in the pool at vptr.
 * the fragment at offset 0 holds its frame's link and IP headers ahead of
 * them */
struct ip_fragment {
    AT_Vptr vptr;
    Uint16 offset;
    Uint16 length;
    struct ip_fragment *next;
    Bool used;
};
/* a datagram in reassembly, from src and named by ident. its fragments run
 * from head to tail in order of offset. payload_offset is the bytes of link
 * and IP headers ahead of the data at offset 0. ttl starts as the TTL of the
 * fragment that came first. IP_Timer counts it down a second at a time, but
 * only while the reassembly is reassembly_head. last_received is 1 once the
 * fragment without IP_MF has come */
struct ip_reassembly {
    Uint32 src;
    Sint32 ifno;
    Uint16 ident;
    Uint16 payload_offset;
    Uint8 ttl;
    Uint8 last_received;
    struct ip_reassembly *next;
    struct ip_fragment *head;
    struct ip_fragment *tail;
    Bool used;
};

struct ip_header {
    Uint8 version_ihl;
    Uint8 tos;
    Uint16 length;
    Uint16 ident;
    Uint16 fragment;
    Uint8 ttl;
    Uint8 protocol;
    Uint16 checksum;
    Uint8 src[IP_ADDRESS_BYTES];
    Uint8 dst[IP_ADDRESS_BYTES];
};

Uint32 icb_struct_size = sizeof(struct net_icb);
Sint32 tcpQueNotAvailCt = 0;

RtEntry route_table[IP_ROUTE_ENTRIES];
static struct ip_reassembly fragpktarray[REASSEMBLIES];
static struct ip_fragment fragmentarray[FRAGMENTS];

/* the header IP_Send fills for each datagram it sends. it has room for a word
 * of options, which no code writes */
static struct {
    struct ip_header header;
    Uint8 options[IP_WORD_BYTES];
} send_header;
static Uint16 ip_identification;
static struct ip_reassembly *reassembly_head;
static struct ip_reassembly *reassembly_tail;

static AT_MaxBuffs *receive_fragment(Uint8 *data, Uint16 payload_offset,
                                     Sint32 length, Sint32 ifno, Uint8 ttl,
                                     Uint16 ident, Uint32 src,
                                     Uint16 fragment_offset, Uint8 more);
static AT_MaxBuffs *insert_fragment(struct ip_reassembly *reassembly,
                                    Uint8 *data, Uint16 payload_offset,
                                    Uint16 fragment_offset, Uint16 length,
                                    Uint8 more);
static int fragments_missing(struct ip_reassembly *reassembly);
static struct ip_reassembly *start_reassembly(Uint32 src, Sint32 ifno,
                                              Uint16 ident, Uint8 ttl);
static int unlink_reassembly(Uint32 src, Sint32 ifno, Uint16 ident);
static int free_reassembled_buffers(AT_MaxBuffs *buffers);
static struct ip_fragment *allocate_fragment(void);
static struct ip_reassembly *allocate_reassembly(void);
static int release_fragment(struct ip_fragment *fragment);
static int release_reassembly(struct ip_reassembly *reassembly);

static int discard_other_reassemblies(struct ip_reassembly *keep);

int IP_Init(void)
{
    int index;
    RtEntry *route;
    struct ip_reassembly *reassembly;
    struct ip_fragment *fragment;

    ip_identification = 1;
    for (index = 0; index < max_icb_entry; index++) {
        icb_table[index].state = ICB_CLOSED;
    }
    route = route_table;
    do {
        route->flags = 0;
    } while (++route < &route_table[IP_ROUTE_ENTRIES]);
    reassembly_tail = 0;
    reassembly_head = 0;
    reassembly = fragpktarray;
    do {
        reassembly->used = FALSE;
    } while (++reassembly < &fragpktarray[REASSEMBLIES]);
    fragment = fragmentarray;
    do {
        fragment->used = FALSE;
    } while (++fragment < &fragmentarray[FRAGMENTS]);
    return 0;
}

int IP_ClearRoutingTable(void)
{
    RtEntry *route;

    route = route_table;
    do {
        route->flags = 0;
    } while (++route < &route_table[IP_ROUTE_ENTRIES]);
    return 0;
}

int IP_InitIfconfig(int unit)
{
    struct net_iface *iface;

    if ((iface = DL_GetIfEntry(unit)) == 0) {
        return -1;
    }
    if (IP_RouteAdd(iface->addr & iface->mask, iface->mask, iface->addr, 0,
                    RT_KIND_INTERFACE, 0)) {
        return -1;
    }
    return 0;
}

int IP_IfDown(int unit)
{
    struct net_iface *iface;

    if ((iface = DL_GetIfEntry(unit)) == 0) {
        return -1;
    }
    IP_RouteDel(iface->addr & iface->mask, iface->mask, iface->addr,
                RT_KIND_INTERFACE);
    IP_ClearRoutingTable();
    return 0;
}

Sint32 IP_Open(Uint32 address, Uint8 protocol, Sint32 asr,
               AT_udp_receiver receiver)
{
    int found = 0;
    int slot;
    int index;

    if (protocol == IPPROTO_TCP || protocol == IPPROTO_UDP) {
        return API_DUPSOCK;
    }
    for (index = 0; index < max_icb_entry; index++) {
        if (icb_table[index].state == ICB_CLOSED) {
            slot = index;
            found = 1;
        } else if (icb_table[index].protocol == protocol &&
                   (icb_table[index].addr == address || address == 0 ||
                    icb_table[index].addr == 0)) {
            return API_DUPSOCK;
        }
    }
    if (found == 1) {
        icb_table[slot].state = ICB_OPEN;
        icb_table[slot].addr = address;
        icb_table[slot].protocol = protocol;
        icb_table[slot].receiver = receiver;
        icb_table[slot].asr = asr;
        return slot;
    }
    return API_PORTFULL;
}

Sint32 IP_SetASR(Sint32 socket, Sint32 asr)
{
    if (socket >= max_icb_entry || socket < 0 ||
        icb_table[socket].state == ICB_CLOSED) {
        return API_INVALIDHANDLE;
    }
    icb_table[socket].asr = asr;
    return 0;
}

Sint32 IP_Close(Sint32 socket)
{
    if (socket >= max_icb_entry || socket < 0 ||
        icb_table[socket].state == ICB_CLOSED) {
        return API_INVALIDHANDLE;
    }
    API_ASRCall(socket, icb_table[socket].asr, ASR_IP, ASR_LAST);
    icb_table[socket].state = ICB_CLOSED;
    return 0;
}

Sint32 IP_ApiSend(AT_MaxBuffs *packet, Sint16 socket, Sint8 raw, Uint8 ttl,
                  Uint8 service, Uint8 dont_fragment, Uint16 options_size,
                  void *options)
{
    if (socket >= max_icb_entry || socket < 0 ||
        icb_table[socket].state == ICB_CLOSED) {
        return API_INVALIDHANDLE;
    }
    if (DL_CheckSend() == -1) {
        return -1;
    }
    if (IP_Send(packet, icb_table[socket].addr, raw, ttl, service,
                dont_fragment, options_size, options,
                icb_table[socket].protocol, 0)) {
        return -1;
    }
    return 0;
}

Sint32 IP_Send(AT_MaxBuffs *buffers, Uint32 destination, Uint8 raw, Uint8 ttl,
               Uint8 service, Uint8 dont_fragment, Uint16 options_size,
               void *options, Uint8 protocol, Sint32 flags)
{
    Sint16 length;
    Uint8 header_words;
    Uint16 header_size;
    Uint32 loopback;
    Sint32 local;
    RtEntry *route;
    struct net_iface *iface;
    struct ip_header *header;
    Uint32 next_hop;
    Sint32 index;
    Sint32 segment;

#ifdef NONMATCHING
    /* this side refuses a list with no buffer or more than AT_MAX_BUFFERS. the
     * code below reads the buffer at bufnum - 1 */
    if (buffers->bufnum <= 0 || buffers->bufnum > AT_MAX_BUFFERS) {
        return -1;
    }
#endif
    header_words = IP_HEADER_BYTES / IP_WORD_BYTES;
    length = 0;
    local = 0;
    loopback = OS_Htonl(INADDR_LOOPBACK);
    if (DL_MatchMyIpaddr(destination) != -1 || destination == loopback) {
        local = 1;
    }
    if ((route = IP_RouteSearch(destination)) == NULL) {
        return -1;
    }
    header = &send_header.header;
    if ((iface = DL_GetIfEntry(route->ifno)) == NULL) {
        return -1;
    }
    /* a raw datagram comes with its own Ethernet and IP headers. IP_Send
     * keeps the IP header's tos and ttl, and writes the rest */
    if (raw == 1) {
        AT_Vptr pointer;
        OS_IncVptr(&buffers->buffers[buffers->bufnum - 1].vptr,
                   ETHERNET_HEADER_BYTES);
        OS_SetVptrLocal(&pointer, header);
        OS_Vmemcpy(&pointer, &buffers->buffers[buffers->bufnum - 1].vptr,
                   IP_HEADER_BYTES);
        OS_IncVptr(&buffers->buffers[buffers->bufnum - 1].vptr,
                   IP_HEADER_BYTES);
        buffers->buffers[buffers->bufnum - 1].len -=
            ETHERNET_HEADER_BYTES + IP_HEADER_BYTES;
        buffers->total_len -= ETHERNET_HEADER_BYTES + IP_HEADER_BYTES;
    }
    header->version_ihl = IP_VERSION << IP_VERSION_SHIFT;
    if (options_size && options != NULL) {
        /* @bug logic: never copies the caller's options, though they count
         * in the header's length. send_header.options goes out in their place
         * (0x0cad768c) */
        /* @bug memory: sends past send_header for more options than its
         * word holds. the header's length counts them all (0x0cad768c) */
        header_words += (Uint8)(options_size + 3) / 4U;
    }
    header->version_ihl |= header_words;
    if (raw == 0) {
        if (service) {
            header->tos = service;
        } else {
            header->tos = 0;
        }
    }
    if (raw == 0) {
        if (ttl) {
            header->ttl = ttl;
        } else {
            header->ttl = IP_DEFAULT_TTL;
        }
    }
    for (index = 0; index < buffers->bufnum; index++) {
        length = length + buffers->buffers[index].len;
    }
    if (length == 0) {
        return -1;
    }
    length += header_size = header_words * IP_WORD_BYTES;
    header->length = OS_Htons(length);
    header->protocol = protocol;
    header->ident = OS_Htons(ip_identification++);
    memcpy(header->dst, &destination, sizeof(header->dst));
    if (destination == loopback) {
        memcpy(header->src, &loopback, sizeof(header->src));
    } else {
        memcpy(header->src, &iface->addr, sizeof(header->src));
    }
    if (iface->addr == route->gateway) {
        next_hop = destination;
    } else {
        next_hop = route->gateway;
    }
    if (length > iface->mtu) {
        Sint32 offset;
        Sint32 header_bytes;
        Sint32 fragment_size;
        Sint32 fragment_units;
        AT_MaxBuffs part;
        AT_MaxBuffs *fragment;
        Sint32 last;
        if (dont_fragment) {
            return -1;
        }
        header->fragment = 0;
        offset = 0;
        fragment_size =
            (iface->mtu - (header_words * IP_WORD_BYTES)) / FRAGMENT_UNIT_BYTES;
        header_bytes = header_words * IP_WORD_BYTES;
        fragment_size *= FRAGMENT_UNIT_BYTES;
        fragment_units = fragment_size / FRAGMENT_UNIT_BYTES;
        /* @hack redundant: the assignment of header_size to fragment_size,
         * which never runs */
        if (0U) {
            fragment_size = header_size;
        }
        for (;;) {
            Sint32 k;
            Sint32 available;
            Sint32 count;
            Sint32 remaining;
            Uint16 sum;
            Uint8 *checksum;
            fragment = &part;
            fragment->bufnum = 0;
            fragment->total_len = 0;
            available = fragment_size;
            index = buffers->bufnum - 1;
            count = 0;
            while (available > 0 && index >= 0) {
                available = available - buffers->buffers[index].len;
                index--;
                count++;
            }
            remaining = fragment_size;
            segment = count - 1;
            k = buffers->bufnum - 1;
            last = 0;
            while (remaining > 0 && segment >= 0) {
                memcpy(&fragment->buffers[segment].vptr,
                       &buffers->buffers[k].vptr, sizeof(AT_Vptr));
                if (buffers->buffers[k].len > remaining) {
                    fragment->buffers[segment].len = remaining;
                    fragment->total_len += remaining;
                    buffers->buffers[k].len -= remaining;
                    buffers->total_len -= remaining;
                    OS_IncVptr(&buffers->buffers[k].vptr, remaining);
                    remaining = 0;
                } else {
                    fragment->buffers[segment].len = buffers->buffers[k].len;
                    fragment->total_len += buffers->buffers[k].len;
                    buffers->total_len -= buffers->buffers[k].len;
                    remaining -= buffers->buffers[k].len;
                    if (k == 0) {
                        remaining = buffers->buffers[k].len;
                        last = 1;
                        segment--;
                        k--;
                    } else {
                        buffers->bufnum--;
                        segment--;
                        k--;
                    }
                }
                fragment->bufnum++;
            }
            if (last == 1) {
                header->fragment = 0;
            } else {
                header->fragment = OS_Htons(IP_MF);
            }
            header->fragment |= OS_Htons(offset);
            length = fragment->total_len + header_bytes;
            header->length = OS_Htons(length);
            header->checksum = 0;
            checksum = (Uint8 *)&header->checksum;
            sum = IP_Chksum((Uint8 *)header,
                            IP_HEADER_BYTES / CHECKSUM_WORD_BYTES);
            *checksum = sum >> 8;
            checksum++;
            *checksum = sum;
            index = fragment->bufnum;
            OS_SetVptrLocal(&fragment->buffers[index].vptr, header);
            fragment->buffers[index].len = header_size;
            fragment->bufnum++;
            if (local) {
                struct queued_frame *slot;
                Uint8 *out;
                Sint32 size;
                AT_Vptr pointer;
                /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
                Sint32 total;
                if (!tcp_que_available(&tcp_que_tbl)) {
                    tcpQueNotAvailCt++;
                    return -1;
                }
                slot = tcp_que_wptr(&tcp_que_tbl);
                out = &slot->frame[ETHERNET_HEADER_BYTES];
                index = fragment->bufnum;
                size = buffers->total_len;
                total = 0;
                for (; index > 0; index--) {
#ifdef NONMATCHING
                    /* this side refuses a fragment's buffer of negative length,
                     * or one that would run past the queued frame's
                     * TCP_FRAME_BYTES */
                    if (fragment->buffers[index - 1].len < 0 ||
                        fragment->buffers[index - 1].len >
                            TCP_FRAME_BYTES - ETHERNET_HEADER_BYTES - total) {
                        return -1;
                    }
#endif
                    OS_SetVptrLocal(&pointer, out);
                    OS_Vmemcpy(&pointer, &fragment->buffers[index - 1].vptr,
                               fragment->buffers[index - 1].len);
                    out += fragment->buffers[index - 1].len;
                    total += fragment->buffers[index - 1].len;
                }
                slot->frame[ETHERNET_TYPE_OFFSET] = ETHERTYPE_IP >> 8;
                slot->frame[ETHERNET_TYPE_OFFSET + 1] = ETHERTYPE_IP & 0xff;
                /* @bug logic: queues the frame with a length that counts the
                 * data left after this fragment, where this fragment and its
                 * IP header are meant. IP_Send reads buffers->total_len at
                 * 0x0cad79a8 and queues it at 0x0cad7a0a. IP_Recv takes the
                 * length in the IP header and never reads the queued one */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: it queues the length
                 * the frame holds, since its IP_Recv drops a datagram whose
                 * header claims more */
                size = total;
#endif
                tcp_que_regist(&tcp_que_tbl, size + ETHERNET_HEADER_BYTES, 0);
            } else {
                if (ARP_PktSend(fragment, next_hop, route->ifno)) {
                    return -1;
                }
            }
            offset += fragment_units;
            if (last == 1) {
                break;
            }
        }
    } else {
        Uint16 sum;
        Uint8 *checksum;
        header->fragment = 0;
        if (dont_fragment) {
            header->fragment = OS_Htons(IP_DF);
        }
        for (segment = 0, length = 0; segment < buffers->bufnum; segment++) {
            length = length + buffers->buffers[segment].len;
        }
        length += header_size;
        header->length = OS_Htons(length);
        header->checksum = 0;
        checksum = (Uint8 *)&header->checksum;
        sum = IP_Chksum((Uint8 *)header, IP_HEADER_BYTES / CHECKSUM_WORD_BYTES);
        *checksum = sum >> 8;
        checksum++;
        *checksum = sum;
        index = buffers->bufnum;
        OS_SetVptrLocal(&buffers->buffers[index].vptr, header);
        buffers->buffers[index].len = header_size;
        buffers->bufnum++;
        if (local) {
            struct queued_frame *slot;
            Uint8 *out;
            Sint32 size;
            AT_Vptr pointer;
            /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
            Sint32 total;
            if (!tcp_que_available(&tcp_que_tbl)) {
                tcpQueNotAvailCt++;
                return -1;
            }
            slot = tcp_que_wptr(&tcp_que_tbl);
            out = &slot->frame[ETHERNET_HEADER_BYTES];
            index = buffers->bufnum;
            size = buffers->total_len;
            total = 0;
            for (; index > 0; index--) {
#ifdef NONMATCHING
                /* this side refuses a buffer of negative length, or one that
                 * would run past the queued frame's TCP_FRAME_BYTES */
                if (buffers->buffers[index - 1].len < 0 ||
                    buffers->buffers[index - 1].len >
                        TCP_FRAME_BYTES - ETHERNET_HEADER_BYTES - total) {
                    return -1;
                }
#endif
                OS_SetVptrLocal(&pointer, out);
                OS_Vmemcpy(&pointer, &buffers->buffers[index - 1].vptr,
                           buffers->buffers[index - 1].len);
                out += buffers->buffers[index - 1].len;
                total += buffers->buffers[index - 1].len;
            }
            slot->frame[ETHERNET_TYPE_OFFSET] = ETHERTYPE_IP >> 8;
            slot->frame[ETHERNET_TYPE_OFFSET + 1] = ETHERTYPE_IP & 0xff;
            /* @bug logic: queues the frame with a length that leaves out the
             * IP header, since buffers->total_len doesn't count the header's
             * buffer. IP_Send reads buffers->total_len at 0x0cad7b10 and
             * queues it at 0x0cad7b70. IP_Recv takes the length in the IP
             * header and never reads the queued one */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it queues the length the
             * frame holds, since its IP_Recv drops a datagram whose header
             * claims more */
            size = total;
#endif
            tcp_que_regist(&tcp_que_tbl, size + ETHERNET_HEADER_BYTES, 0);
        } else {
            if (ARP_PktSend(buffers, next_hop, route->ifno)) {
                return -1;
            }
        }
    }
    return 0;
}

Sint32 IP_Recv(Uint8 *data, Sint32 size, Uint16 offset, Sint32 ifno)
{
    struct ip_header *header;
#ifdef NONMATCHING
    /* fixed_header holds this side's copy of the header, since nothing aligns
     * data + offset */
    struct ip_header fixed_header;
#endif
    Uint32 src;
    Uint32 dst;
    Uint16 header_size;
    Uint16 length;
    Uint16 fragment;
    Uint8 *payload;
    AT_MaxBuffs *buffers;
    AT_MaxBuffs *reply;
    int reassembled;
    int handled;
    int index;
    int result;
    struct net_icb *icb;

    if (DL_GetIfEntry(ifno) == NULL) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side drops a frame too short for the fixed header, and reads the
     * header from an aligned copy */
    if (size < offset || size - offset < (Sint32)sizeof(fixed_header)) {
        return -1;
    }
    memcpy(&fixed_header, data + offset, sizeof(fixed_header));
    header = &fixed_header;
#else
    header = (struct ip_header *)(data + offset);
#endif
    memcpy(&dst, data + offset + IP_DST_OFFSET, sizeof(dst));
    memcpy(&src, data + offset + IP_SRC_OFFSET, sizeof(src));
    header_size = (header->version_ihl & IP_IHL_MASK) * IP_WORD_BYTES;
    length = OS_Htons(header->length);
#ifdef NONMATCHING
    /* this side drops a header or a datagram longer than the frame's bytes past
     * offset, and finds the payload in the frame, not past the copy */
    if (header_size > size - offset || length > size - offset) {
        return -1;
    }
    payload = data + offset + header_size;
#else
    payload = (Uint8 *)header + header_size;
#endif
    if (header_size < IP_HEADER_BYTES) {
        return -1;
    }
    if ((header->version_ihl >> IP_VERSION_SHIFT) != IP_VERSION) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side checksums the header in the frame, since the copy holds no
     * options */
    if (IP_Chksum(data + offset, header_size / CHECKSUM_WORD_BYTES)) {
#else
    if (IP_Chksum((Uint8 *)header, header_size / CHECKSUM_WORD_BYTES)) {
#endif
        return -1;
    }
    if (length < header_size) {
        return -1;
    }
    if (DL_MatchMyIpaddr(dst) == -1 && !IP_IsBroadcast(dst, ifno) &&
        !IP_IsLocalloopback(dst, ifno)) {
        return -1;
    }
    fragment = OS_Htons(header->fragment);
    if (fragment & ~IP_DF) {
        Uint8 more;
        if (fragment & IP_DF) {
            return -1;
        }
        length -= header_size;
        offset += header_size;
        if (fragment & IP_MF) {
            more = 1;
        } else {
            more = 0;
        }
        fragment &= IP_OFFMASK;
        if ((buffers = receive_fragment(data, offset, length, ifno, header->ttl,
                                        OS_Htons(header->ident), src, fragment,
                                        more)) == NULL) {
            return 0;
        }
        reassembled = 1;
    } else {
        if ((buffers = OS_AllocBuffs()) == NULL) {
            return -1;
        }
        buffers->bufnum = 1;
        buffers->total_len = buffers->buffers[0].len = length + offset;
        offset += header_size;
        OS_SetVptrLocal(&buffers->buffers[0].vptr, data);
        reassembled = 0;
    }
    switch (header->protocol) {
    case IPPROTO_ICMP:
        handled = 1;
        if (ICMP_Recv(buffers, offset, src)) {
            if (reassembled == 1) {
                free_reassembled_buffers(buffers);
            }
            OS_FreeBuffs(buffers);
            return -1;
        }
        if (reassembled == 1) {
            free_reassembled_buffers(buffers);
        }
        OS_FreeBuffs(buffers);
        break;
    case IPPROTO_TCP:
        if (TCP_Recv(buffers, offset, src, dst)) {
            if (reassembled == 1) {
                free_reassembled_buffers(buffers);
            }
            OS_FreeBuffs(buffers);
            return -1;
        }
        if (reassembled == 1) {
            free_reassembled_buffers(buffers);
        }
        OS_FreeBuffs(buffers);
        return 0;
    case IPPROTO_UDP:
        if ((result = UDP_Recv(buffers, offset, src, dst)) != 0) {
            if (result == UDP_NO_SOCKET) {
                if ((reply = OS_AllocBuffs()) == NULL) {
                    /* @bug memory: returns without freeing buffers, or a
                     * reassembled datagram's pool buffers, where no list is
                     * free for the reply (0x0cad7dbc) */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above: it frees
                     * buffers, and a reassembled datagram's pool buffers */
                    if (reassembled == 1) {
                        free_reassembled_buffers(buffers);
                    }
                    OS_FreeBuffs(buffers);
#endif
                    return -1;
                }
                reply->bufnum = 1;
                reply->total_len = QUOTED_DATA_BYTES;
                reply->buffers[0].len = QUOTED_DATA_BYTES;
                OS_SetVptrLocal(&reply->buffers[0].vptr, payload);
                reply->bufnum++;
                reply->total_len += header_size;
                reply->buffers[1].len = header_size;
                OS_SetVptrLocal(&reply->buffers[1].vptr, header);
                if (ICMP_Send(ICMP_UNREACH, ICMP_UNREACH_PORT, src, reply) ==
                    -1) {
                    if (reassembled == 1) {
                        free_reassembled_buffers(buffers);
                    }
                    OS_FreeBuffs(buffers);
                    OS_FreeBuffs(reply);
                    return -1;
                } else {
                    if (reassembled == 1) {
                        free_reassembled_buffers(buffers);
                    }
                    OS_FreeBuffs(buffers);
                    OS_FreeBuffs(reply);
                    return 0;
                }
            }
            /* @hack redundant: the empty else */
            if (reassembled == 1) {
                free_reassembled_buffers(buffers);
            } else {
            }
            OS_FreeBuffs(buffers);
            return -1;
        }
        if (reassembled == 1) {
            free_reassembled_buffers(buffers);
        }
        OS_FreeBuffs(buffers);
        return 0;
    default:
        handled = 0;
        break;
    }
    for (index = 0; index < max_icb_entry; index++) {
        icb = &icb_table[index];
        if (icb->state == ICB_OPEN && (icb->addr == 0 || icb->addr == src) &&
            icb->protocol == header->protocol && icb->receiver) {
            /* @bug memory: hands a raw socket an ICMP message after its
             * buffers are freed (0x0cad7d7c, 0x0cad7e96) */
            API_IpRecv(buffers, src, icb->receiver, index);
            handled = 1;
        }
    }
    /* @bug memory: returns without freeing buffers, or a reassembled
     * datagram's pool buffers, for a datagram of another protocol that a raw
     * socket takes. API_IpRecv doesn't free them either (0x0cad7eae) */
    if (handled == 1) {
        return 0;
    }
    if ((reply = OS_AllocBuffs()) == NULL) {
        /* @bug memory: returns without freeing buffers, or a reassembled
         * datagram's pool buffers, where no list is free for the reply
         * (0x0cad7eba) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees buffers, and a
         * reassembled datagram's pool buffers */
        if (reassembled == 1) {
            free_reassembled_buffers(buffers);
        }
        OS_FreeBuffs(buffers);
#endif
        return -1;
    }
    reply->bufnum = 1;
    reply->total_len = QUOTED_DATA_BYTES;
    reply->buffers[0].len = QUOTED_DATA_BYTES;
    OS_SetVptrLocal(&reply->buffers[0].vptr, payload);
    reply->bufnum++;
    reply->total_len += header_size;
    reply->buffers[1].len = header_size;
    OS_SetVptrLocal(&reply->buffers[1].vptr, header);
    if (ICMP_Send(ICMP_UNREACH, ICMP_UNREACH_PROTOCOL, src, reply) == -1) {
        if (reassembled == 1) {
            free_reassembled_buffers(buffers);
        }
        OS_FreeBuffs(buffers);
        OS_FreeBuffs(reply);
        return -1;
    }
    /* @hack redundant: the empty else */
    if (reassembled == 1) {
        free_reassembled_buffers(buffers);
    } else {
    }
    OS_FreeBuffs(buffers);
    OS_FreeBuffs(reply);
    return -1;
}

Uint16 IP_Chksum(void *data, Sint32 count)
{
    Uint32 sum;
    Uint8 *p;
    int n;
    Uint16 word;

    n = count;
    p = data;
    for (sum = 0; n > 0; n--) {
        word = (p[0] << 8) + p[1];
        sum += word;
        p += 2;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;
    return ~sum;
}

Sint32 IP_IsBroadcast(Uint32 address, Sint32 interface_number)
{
    struct net_iface *iface;
    if ((iface = DL_GetIfEntry(interface_number)) == NULL) {
        return 0;
    }
    if (!iface->addr || address == INADDR_BROADCAST ||
        address == iface->broadcast ||
        (!(iface->broadcast & 1) && address == 0)) {
        return 1;
    }
    return 0;
}

Sint32 IP_IsLocalloopback(Uint32 ip, Sint32 ifno)
{
    if (DL_GetIfEntry(ifno) != NULL) {
        if ((OS_Htonl(ip) & IN_CLASSA_NET) ==
            (INADDR_LOOPBACK & IN_CLASSA_NET)) {
            return 1;
        }
    }
    return 0;
}

static AT_MaxBuffs *receive_fragment(Uint8 *data, Uint16 payload_offset,
                                     Sint32 length, Sint32 ifno, Uint8 ttl,
                                     Uint16 ident, Uint32 src,
                                     Uint16 fragment_offset, Uint8 more)
{
    struct ip_reassembly *reassembly;
    AT_MaxBuffs *buffers;
    for (reassembly = reassembly_head; reassembly;
         reassembly = reassembly->next) {
        if (reassembly->src == src && reassembly->ident == ident) {
            /* @hack spelling: the jump past start_reassembly, where a break
             * and a test of reassembly would do */
            goto found;
        }
    }
    if ((reassembly = start_reassembly(src, ifno, ident, ttl)) == NULL) {
        return NULL;
    }
found:
    if ((buffers = insert_fragment(reassembly, data, payload_offset,
                                   fragment_offset, length, more)) == NULL) {
        return NULL;
    }
    unlink_reassembly(src, ifno, ident);
    return buffers;
}

static AT_MaxBuffs *insert_fragment(struct ip_reassembly *reassembly,
                                    Uint8 *data, Uint16 payload_offset,
                                    Uint16 fragment_offset, Uint16 length,
                                    Uint8 more)
{
    struct ip_fragment *fragment;
    struct ip_fragment *p;
    struct ip_fragment *next;
    AT_Vptr source;
    AT_MaxBuffs *buffers;
    int index;

    if (!fragment_offset) {
        reassembly->payload_offset = payload_offset;
    } else {
        data += payload_offset;
    }
    if ((fragment = allocate_fragment()) == NULL) {
        discard_other_reassemblies(reassembly);
        if ((fragment = allocate_fragment()) == NULL) {
            return NULL;
        }
    }
    fragment->offset = fragment_offset;
    fragment->length = length;
    OS_SetVptrLocal(&source, data);
    if (!fragment_offset) {
        if (MP_Alloc(&fragment->vptr, payload_offset + length,
                     MP_IP_FRAGMENT)) {
            discard_other_reassemblies(reassembly);
            if (MP_Alloc(&fragment->vptr, payload_offset + length,
                         MP_IP_FRAGMENT)) {
                return NULL;
            }
        }
        OS_Vmemcpy(&fragment->vptr, &source, payload_offset + length);
    } else {
        if (MP_Alloc(&fragment->vptr, length, MP_IP_FRAGMENT)) {
            discard_other_reassemblies(reassembly);
            if (MP_Alloc(&fragment->vptr, length, MP_IP_FRAGMENT)) {
                return NULL;
            }
        }
        OS_Vmemcpy(&fragment->vptr, &source, length);
    }
    fragment->next = NULL;
    if (reassembly->head == NULL) {
        reassembly->head = reassembly->tail = fragment;
    } else if (fragment->offset > reassembly->tail->offset) {
        reassembly->tail->next = fragment;
        reassembly->tail = fragment;
    } else if (fragment->offset < reassembly->head->offset) {
        fragment->next = reassembly->head;
        reassembly->head = fragment;
    } else {
        for (p = reassembly->head, next = p->next; next;
             p = p->next, next = next->next) {
            if (fragment->offset > p->offset &&
                fragment->offset < next->offset) {
                fragment->next = next;
                p->next = fragment;
                break;
            }
            if (fragment->offset == p->offset) {
                OS_FreeMempool(&fragment->vptr);
                release_fragment(fragment);
                break;
            }
        }
        /* @bug memory: loses a fragment at the last one's offset, its record
         * and its pool buffer. the walk above stops before the last fragment,
         * so it neither links nor frees one there (0x0cad81d0) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees a fragment at the
         * last one's offset, as the walk frees one at another's. the offsets
         * rise along the list, so the walk runs to its end for that fragment
         * alone */
        if (!next) {
            OS_FreeMempool(&fragment->vptr);
            release_fragment(fragment);
        }
#endif
    }
    if (reassembly->last_received == 0) {
        if (more) {
            return NULL;
        }
        reassembly->last_received = 1;
    }
    if (fragments_missing(reassembly)) {
        return NULL;
    }
    if ((buffers = OS_AllocBuffs()) == NULL) {
        IP_CleanFragmentAll();
        return NULL;
    }
    buffers->bufnum = 0;
    buffers->total_len = 0;
    for (p = reassembly->head, index = 0; p; index++) {
        buffers->bufnum++;
        buffers->total_len += p->length;
        buffers->buffers[index].len = p->length;
        memcpy(&buffers->buffers[index].vptr, &p->vptr, sizeof(AT_Vptr));
        if (index == 0) {
            buffers->total_len += reassembly->payload_offset;
            buffers->buffers[index].len += reassembly->payload_offset;
        }
        fragment = p;
        p = p->next;
        release_fragment(fragment);
    }
    return buffers;
}

static int fragments_missing(struct ip_reassembly *reassembly)
{
    struct ip_fragment *p;
    Uint16 offset = 0;
    p = reassembly->head;
    while (p) {
        if (p->offset != offset) {
            return -1;
        }
        offset += p->length / FRAGMENT_UNIT_BYTES;
        p = p->next;
    }
    return 0;
}

static struct ip_reassembly *start_reassembly(Uint32 src, Sint32 ifno,
                                              Uint16 ident, Uint8 ttl)
{
    struct ip_reassembly *reassembly;
    if ((reassembly = allocate_reassembly()) == NULL) {
        IP_CleanFragmentAll();
        if ((reassembly = allocate_reassembly()) == NULL) {
            return NULL;
        }
    }
    reassembly->src = src;
    reassembly->ident = ident;
    reassembly->ifno = ifno;
    reassembly->last_received = 0;
    reassembly->ttl = ttl;
    if (reassembly_head == NULL) {
        reassembly->next = NULL;
        reassembly_tail = reassembly;
        reassembly_head = reassembly;
    } else {
        reassembly->next = NULL;
        reassembly_tail->next = reassembly;
        reassembly_tail = reassembly;
    }
    reassembly->tail = NULL;
    reassembly->head = NULL;
    return reassembly;
}

static int unlink_reassembly(Uint32 src, Sint32 ifno, Uint16 ident)
{
    struct ip_reassembly *reassembly;
    struct ip_reassembly *previous;
    struct ip_reassembly *removed;
    reassembly = reassembly_head;
    if (reassembly == NULL) {
        return -1;
    }
    removed = reassembly;
    if (reassembly->src == src && reassembly->ifno == ifno &&
        reassembly->ident == ident) {
        if (reassembly_tail == removed) {
            reassembly_head = reassembly_tail = NULL;
        } else {
            reassembly_head = removed->next;
        }
        release_reassembly(removed);
        return 0;
    } else {
        previous = reassembly;
        for (reassembly = removed->next; reassembly;
             reassembly = reassembly->next, previous = previous->next) {
            if (reassembly->src == src && reassembly->ifno == ifno &&
                reassembly->ident == ident) {
                /* @bug memory: releases the reassembly after the match, or
                 * none where the match is the last, in place of the match.
                 * the match stays linked, its head on fragment records
                 * insert_fragment released. IP_Timer, IP_CleanFragmentAll and
                 * discard_other_reassemblies then free those records' buffers
                 * again. the released one stays linked too, and
                 * start_reassembly's reuse of it cuts off every reassembly
                 * after it, which no path then frees (0x0cad8374) */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: it unlinks the match
                 * and releases it. where the match was the last,
                 * reassembly_tail moves to previous */
                previous->next = reassembly->next;
                if (reassembly_tail == reassembly) {
                    reassembly_tail = previous;
                }
                release_reassembly(reassembly);
#else
                removed = reassembly->next;
                release_reassembly(removed);
#endif
                return 0;
            }
        }
    }
    return -1;
}

static int free_reassembled_buffers(AT_MaxBuffs *buffers)
{
    int index;
    for (index = 0; index < buffers->bufnum; index++) {
        OS_FreeMempool(&buffers->buffers[index].vptr);
    }
    return 0;
}

static struct ip_fragment *allocate_fragment(void)
{
    int index;
    for (index = 0; index < FRAGMENTS; index++) {
        if (!fragmentarray[index].used) {
            fragmentarray[index].used = TRUE;
            return &fragmentarray[index];
        }
    }
    return NULL;
}

static struct ip_reassembly *allocate_reassembly(void)
{
    int index;
    for (index = 0; index < REASSEMBLIES; index++) {
        if (!fragpktarray[index].used) {
            fragpktarray[index].used = TRUE;
            return &fragpktarray[index];
        }
    }
    return NULL;
}

static int release_fragment(struct ip_fragment *fragment)
{
    int index;
    for (index = 0; index < FRAGMENTS; index++) {
        if (&fragmentarray[index] == fragment) {
            fragmentarray[index].used = FALSE;
            return 0;
        }
    }
    return -1;
}

static int release_reassembly(struct ip_reassembly *reassembly)
{
    int index;
    for (index = 0; index < REASSEMBLIES; index++) {
        if (&fragpktarray[index] == reassembly) {
            fragpktarray[index].used = FALSE;
            return 0;
        }
    }
    return -1;
}

void IP_Timer(void)
{
    struct ip_reassembly *p;
    struct ip_reassembly *previous;
    struct ip_reassembly *removed;
    struct ip_reassembly *next;
    struct ip_fragment *fragment;
    struct ip_fragment *old;
    icmp_timer_1sec = 0;
    if (reassembly_head == NULL) {
        return;
    }
    /* only reassembly_head counts down. each other reassembly waits until
     * those ahead of it complete or expire */
    p = reassembly_head;
    if (--p->ttl == 0) {
        removed = p;
        next = p->next;
        fragment = removed->head;
        while (fragment) {
            old = fragment;
            fragment = fragment->next;
            OS_FreeMempool(&old->vptr);
            release_fragment(old);
        }
        release_reassembly(removed);
        reassembly_head = next;
    } else {
        previous = reassembly_head;
        while (p) {
            if (p->ttl == 0) {
                removed = p;
                p = p->next;
                fragment = removed->head;
                while (fragment) {
                    old = fragment;
                    fragment = fragment->next;
                    OS_FreeMempool(&old->vptr);
                    release_fragment(old);
                }
                release_reassembly(removed);
                /* @bug memory: unlinks every reassembly between
                 * reassembly_head and the one it drops, which no path then
                 * frees. previous never leaves reassembly_head. a reassembly
                 * behind the head reaches a ttl of 0 only by starting with a
                 * TTL of 0 (0x0cad8516) */
                /* @bug memory: leaves reassembly_tail on the reassembly it
                 * releases, where that was the list's last. start_reassembly
                 * links the next one after it. no walk from reassembly_head
                 * finds that one, and no path frees it (0x0cad8510) */
#ifdef NONMATCHING
                /* this side repairs both bugs tagged above. previous follows
                 * the walk, in the else below, so only the reassembly dropped
                 * is unlinked. where that was the last, reassembly_tail moves
                 * to previous */
                if (reassembly_tail == removed) {
                    reassembly_tail = previous;
                }
#endif
                previous->next = p;
            } else {
#ifdef NONMATCHING
                /* this side's half of the repair of the first bug tagged
                 * above: previous follows p */
                previous = p;
#endif
                p = p->next;
            }
        }
    }
}

int IP_CleanFragmentAll(void)
{
    struct ip_reassembly *p;
    struct ip_reassembly *reassembly;
    struct ip_fragment *fragment;
    struct ip_fragment *old;
    for (p = reassembly_head; p;) {
        reassembly = p;
        p = p->next;
        fragment = reassembly->head;
        while (fragment) {
            old = fragment;
            fragment = fragment->next;
            OS_FreeMempool(&old->vptr);
            release_fragment(old);
        }
        release_reassembly(reassembly);
    }
    reassembly_head = NULL;
    return 0;
}

static int discard_other_reassemblies(struct ip_reassembly *keep)
{
    struct ip_reassembly *p;
    struct ip_reassembly *reassembly;
    struct ip_fragment *fragment;
    struct ip_fragment *old;
    if (!reassembly_head) {
        return -1;
    }
    p = reassembly_head;
    while (p) {
        if (p == keep) {
            reassembly_head = p;
            p = p->next;
            continue;
        }
        fragment = p->head;
        while (fragment) {
            old = fragment;
            fragment = fragment->next;
            OS_FreeMempool(&old->vptr);
            release_fragment(old);
        }
        reassembly = p;
        p = p->next;
        release_reassembly(reassembly);
    }
    /* @bug memory: leaves reassembly_tail on a released reassembly, where the
     * one kept wasn't the list's last. start_reassembly links the next one
     * after it. no walk from reassembly_head finds that one, and no path
     * frees it (0x0cad85cc) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the reassembly kept is the
     * list's last */
    reassembly_tail = reassembly_head;
#endif
    reassembly_head->next = NULL;
    return 0;
}

/* UDP's sockets and datagrams, and UDP_Chksum, the Internet checksum that TCP
 * and ICMP take too */
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <access/at_types.h>
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>

/* the Ethernet, IP and UDP headers a raw socket's datagram starts with, which
 * UDP_Send drops */
#define UDP_RAW_HEADERS                                                        \
    (ETHERNET_HEADER_BYTES + IP_HEADER_BYTES + (int)sizeof(struct udp_hdr))
/* the local ports udp_new_port counts through */
#define UDP_PORT_BASE 0x400
#define UDP_PORT_WRAP 0x7ff
/* the ASR events UDP_ComeIcmpUnreach gives a socket whose peer address ICMP
 * finds unreachable. one without a peer port hears ASR_ADDR_UNREACH for an
 * unreachable host or protocol. one whose peer port ICMP names hears
 * ASR_ADDR_PORT_UNREACH for an unreachable port or network */
#define ASR_ADDR_UNREACH 128
#define ASR_ADDR_PORT_UNREACH 129

Uint32 ucb_struct_size = sizeof(struct ucb);

static Uint16 udp_new_port(void);
static int udp_port_used(Uint16 port);

Sint32 UDP_Init(void)
{
    int index;
    struct ucb *control;

    for (index = 0; index < max_ucb_entry; index++) {
        control = &ucb_table[index];
        control->in_use = 0;
    }
    udp_port_num = 0;
    return 0;
}

Sint32 UDP_Open(Uint32 destination, Uint16 peer_port, Uint16 local_port,
                Sint8 raw, Uint8 ttl, Uint8 service, Uint8 dont_fragment,
                Sint16 options_size, void *options, Sint32 asr,
                AT_udp_receiver receiver)
{
    struct ucb *entry;
    struct ucb *slot;
    struct ucb *control;
    int found;
    int free_index;
    int index;

    found = 0;
    slot = NULL;
    for (index = 0; index < max_ucb_entry; index++) {
        entry = &ucb_table[index];
        if (entry->in_use == 0) {
            if (found == 0) {
                slot = entry;
                free_index = index;
                found = 1;
            }
        } else if (local_port && entry->hdr.sport == local_port) {
            return API_DUPSOCK;
        }
    }
    if (slot != NULL) {
        /* @hack spelling: control found again from free_index, where slot,
         * which holds it, would do */
        control = &ucb_table[free_index];
        control->in_use = 1;
        control->reserved = 0;
        control->raw = raw;
        if (local_port != 0) {
            control->hdr.sport = local_port;
        } else {
            control->hdr.sport = udp_new_port();
        }
        if (destination != 0) {
            control->has_faddr = 1;
            control->faddr = destination;
        } else {
            control->has_faddr = 0;
            control->faddr = destination;
        }
        if (peer_port != 0) {
            control->has_fport = 1;
            control->hdr.dport = peer_port;
        } else {
            control->has_fport = 0;
            control->hdr.dport = peer_port;
        }
        control->options_size = options_size;
        control->options = options;
        control->receiver = receiver;
        control->asr = asr;
        control->ttl = ttl;
        control->svctype = service;
        control->df_flag = dont_fragment;
        control->route.rt = NULL;
        return free_index;
    }
    return API_PORTFULL;
}

Sint32 UDP_Close(Sint32 socket)
{
    struct ucb *control;

    if (socket >= max_ucb_entry || socket < 0 ||
        ucb_table[socket].in_use == 0) {
        return API_INVALIDHANDLE;
    }
    control = &ucb_table[socket];
    IP_FreeRoute(&control->route);
    API_ASRCall(socket, control->asr, ASR_UDP, ASR_LAST);
    control->in_use = 0;
    return 0;
}

Sint32 UDP_Send(AT_MaxBuffs *packet, Uint32 destination, Uint16 port,
                Sint32 socket)
{
    struct ucb *control;
    RtEntry *route;
    Uint32 dst;
    struct tcp_pseudo pseudo;
    Uint16 sum;
    Uint8 *p;
    int n;

    if (DL_CheckSend() == -1) {
        return -1;
    }

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: the socket is checked before
     * ucb_table[socket].raw is read. the packet must have room for one more
     * buffer, the UDP header's */
    if (socket < 0 || socket >= max_ucb_entry || !ucb_table[socket].in_use ||
        packet->bufnum < 0 || packet->bufnum >= AT_MAX_BUFFERS) {
        return -1;
    }
#endif
    /* @bug memory: reads ucb_table[socket].raw before the test below finds
     * socket in range (0x0cade8e6) */
    control = &ucb_table[socket];
    if (control->raw) {
#ifdef NONMATCHING
        /* this side refuses a raw datagram with no buffer, or too short to hold
         * the UDP_RAW_HEADERS it strips */
        if (packet->bufnum == 0 || packet->total_len < UDP_RAW_HEADERS ||
            packet->buffers[packet->bufnum - 1].len < UDP_RAW_HEADERS) {
            return -1;
        }
#endif
        packet->total_len -= UDP_RAW_HEADERS;
        packet->buffers[packet->bufnum - 1].len -= UDP_RAW_HEADERS;
        OS_IncVptr(&packet->buffers[packet->bufnum - 1].vptr, UDP_RAW_HEADERS);
    }
    if (socket >= max_ucb_entry || socket < 0 ||
        ucb_table[socket].in_use == 0) {
        return -1;
    }
    /* @hack redundant: control found again, as it was above */
    control = &ucb_table[socket];
    if (control->has_faddr == 0) {
        control->faddr = destination;
    }
    dst = control->faddr;
    if (control->has_fport == 0) {
        control->hdr.dport = port;
    }
    if ((route = IP_RouteSearch(dst)) == NULL) {
        return -1;
    }
    control->hdr.len = OS_Htons(packet->total_len + sizeof(struct udp_hdr));
    n = packet->bufnum;
    packet->buffers[n].len = sizeof(struct udp_hdr);
    packet->total_len += sizeof(struct udp_hdr);
    OS_SetVptrLocal(&packet->buffers[n].vptr, &control->hdr);
    packet->bufnum++;
    if (dst == OS_Htonl(INADDR_LOOPBACK)) {
        pseudo.src = OS_Htonl(INADDR_LOOPBACK);
    } else {
        pseudo.src = IP_GetMyaddrRoute(route);
    }
    pseudo.dst = dst;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_UDP;
    pseudo.len = OS_Htons(packet->total_len);
    control->hdr.sum = 0;
    p = (Uint8 *)&control->hdr.sum;
    sum = UDP_Chksum(&pseudo, packet, 1);
    *p = sum >> 8;
    p++;
    *p = sum;
    /* where control->raw is 1, IP_Send takes the Ethernet and IP headers from
     * the last buffer. that buffer is now the UDP header's 8 bytes, so IP_Send
     * reads past them and leaves the buffer's length at -26. no caller opens
     * a raw socket. dns.c, API_UDP_Open's only caller, clears reserve1, which
     * API_UDP_Open passes to UDP_Open as raw */
    if (IP_Send(packet, dst, control->raw, control->ttl, control->svctype,
                control->df_flag, 0, 0, IPPROTO_UDP, 0)) {
        return -1;
    }
    return 0;
}

Sint32 UDP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_size, Uint32 src,
                Uint32 dst)
{
    struct udp_hdr hdr;
    struct tcp_pseudo pseudo;
    AT_Vptr local;
    AT_Vptr saved;
    struct udp_hdr *header;
    struct ucb *control;
    int index;

#ifdef NONMATCHING
    /* this side drops a datagram with no buffer or more than AT_MAX_BUFFERS, or
     * whose first buffer is too short for the IP and UDP headers */
    if (packet->bufnum <= 0 || packet->bufnum > AT_MAX_BUFFERS ||
        packet->buffers[0].len <
            (Sint32)(ip_header_size + sizeof(struct udp_hdr)) ||
        packet->total_len < (Sint32)(ip_header_size + sizeof(struct udp_hdr))) {
        return -1;
    }
#endif
    packet->total_len -= ip_header_size;
    packet->buffers[0].len -= ip_header_size;
    memcpy(&saved, &packet->buffers[0].vptr, sizeof(AT_Vptr));
    OS_IncVptr(&packet->buffers[0].vptr, ip_header_size);

#ifdef NONMATCHING
    /* this side copies the header out of every kind of buffer. a local buffer
     * may hold it at an unaligned address */
    OS_SetVptrLocal(&local, &hdr);
    OS_Vmemcpy(&local, &packet->buffers[0].vptr, sizeof(hdr));
    header = &hdr;
#else
    /* @hack type: the type read as a Sint8, where the Uint8 field would do */
    if ((Sint8)packet->buffers[0].vptr.type == AT_PTR_TYPE_LOCAL) {
        header = OS_GetVptrLocal(&packet->buffers[0].vptr);
    } else {
        OS_SetVptrLocal(&local, &hdr);
        OS_Vmemcpy(&local, &packet->buffers[0].vptr, sizeof(struct udp_hdr));
        header = &hdr;
    }
#endif
    if (header->sum != 0) {
        pseudo.src = src;
        pseudo.dst = dst;
        pseudo.zero = 0;
        pseudo.proto = IPPROTO_UDP;
        pseudo.len = OS_Htons(packet->total_len);
        if (UDP_Chksum(&pseudo, packet, 0) != 0) {
            return -1;
        }
    }
    for (index = 0; index < max_ucb_entry; index++) {
        control = &ucb_table[index];
        if (control->in_use != 0 && control->hdr.sport == header->dport &&
            (control->has_faddr == 0 || control->faddr == src) &&
            (control->has_fport == 0 || control->hdr.dport == header->sport)) {
            /* a raw socket takes the datagram with its IP and UDP headers */
            if (control->raw) {
                packet->total_len += ip_header_size;
                packet->buffers[0].len += ip_header_size;
                memcpy(&packet->buffers[0].vptr, &saved, sizeof(AT_Vptr));
            } else {
                packet->total_len -= sizeof(struct udp_hdr);
                packet->buffers[0].len -= sizeof(struct udp_hdr);
                OS_IncVptr(&packet->buffers[0].vptr, sizeof(struct udp_hdr));
            }
            if (API_UdpRecv(packet, src, header->sport, control->receiver,
                            index)) {
                return -1;
            }
            return 0;
        }
    }
    if (IP_IsBroadcast(dst, 0) == 1) {
        return 0;
    }
    return UDP_NO_SOCKET;
}

Uint16 UDP_Chksum(struct tcp_pseudo *pseudo, AT_MaxBuffs *buffers,
                  Sint32 reverse)
{
    Uint32 sum;
    Uint8 *p;
    int n;
    /* @hack spelling: w, which holds each word for the sum, where adding the
     * word would do */
    Uint16 w;

    sum = 0;
    if (pseudo != NULL) {
        p = (Uint8 *)pseudo;

        for (n = 0; n < (int)(sizeof(struct tcp_pseudo) / CHECKSUM_WORD_BYTES);
             n++) {
            w = (p[0] << 8) + p[1];
            sum += w;
            p += 2;
        }
    }
    sum = OS_ChksumBuffs(sum, buffers, reverse);
    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;
    return ~sum;
}

static Uint16 udp_new_port(void)
{
    if (udp_port_num == 0) {
        udp_port_num =
            (TIMER_GetCounter() & (UDP_PORT_BASE - 1)) | UDP_PORT_BASE;
    }
    while (udp_port_used(OS_Htons(++udp_port_num)) == 1) {
        if (udp_port_num >= UDP_PORT_WRAP) {
            udp_port_num = UDP_PORT_BASE;
        }
    }
    return OS_Htons(udp_port_num);
}

static int udp_port_used(Uint16 port)
{
    struct ucb *control;
    int index;

    for (index = 0; index < max_ucb_entry; index++) {
        control = &ucb_table[index];
        if (control->in_use && port == control->hdr.sport) {
            return 1;
        }
    }
    return 0;
}

Sint32 UDP_ComeIcmpUnreach(Uint16 port, Uint32 address, Uint8 code)
{
    struct ucb *control;
    int index;

    for (index = 0; index < max_ucb_entry; index++) {
        control = &ucb_table[index];
        if (control->in_use == 0) {
            continue;
        }
        if (control->has_faddr && address == control->faddr) {
            if (control->has_fport != 0) {
                if (port == control->hdr.dport) {
                    if (code == ICMP_UNREACH_PORT || code == ICMP_UNREACH_NET) {
                        API_ASRCall(index, control->asr, ASR_UDP,
                                    ASR_ADDR_PORT_UNREACH);
                    }
                }
            } else if (code == ICMP_UNREACH_HOST ||
                       code == ICMP_UNREACH_PROTOCOL) {
                API_ASRCall(index, control->asr, ASR_UDP, ASR_ADDR_UNREACH);
            }
        }
    }
    return 0;
}

Sint32 UDP_ApiGetaddr(Sint32 socket, struct sock_addr *address)
{
    struct ucb *control;

    if (socket >= max_ucb_entry || socket < 0 ||
        ucb_table[socket].in_use == 0) {
        return API_INVALIDHANDLE;
    }
    control = &ucb_table[socket];
    address->addr = control->has_faddr ? control->faddr : 0;
    address->local_port = control->hdr.sport;
    address->peer_port = control->has_fport ? control->hdr.dport : 0;
    return 0;
}

Sint32 UDP_SetASR(Sint32 socket, Sint32 asr)
{
    if (socket >= max_ucb_entry || socket < 0 ||
        ucb_table[socket].in_use == 0) {
        return API_INVALIDHANDLE;
    }
    ucb_table[socket].asr = asr;
    return 0;
}

/* ICMP: echo replies, and the redirects and unreachable ports it passes to IP
 * and UDP. ICMP_GetSubnetMask asks for the subnet mask */
#include <access/at_types.h>
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>
#define ICMP_ECHOREPLY 0
#define ICMP_SOURCEQUENCH 4
#define ICMP_REDIRECT 5
#define ICMP_ECHO 8
#define ICMP_TIMXCEED 11
#define ICMP_PARAMPROB 12
#define ICMP_TSTAMP 13
#define ICMP_TSTAMPREPLY 14
#define ICMP_MASKREQ 17
#define ICMP_MASKREPLY 18
/* the codes of a redirect for a network rather than a host */
#define ICMP_REDIRECT_NET 0
#define ICMP_REDIRECT_TOSNET 2

#define ICMP_HEADER_BYTES 8
#define ICMP_MASK_BYTES 12
#define ICMP_MASK_ID 0x123
#define ICMP_MASK_SEQ 1
#define ICMP_MASK_TRIES 3

struct icmp_header {
    Uint8 type;
    Uint8 code;
    Uint16 cksum;
    union {
        Uint8 gateway[IP_ADDRESS_BYTES];
        Uint32 unused;
        struct {
            Uint16 id;
            Uint16 seq;
        } echo;
    } un;
    Uint32 mask;
};

/* the IP header an ICMP error quotes, its addresses read as words */
struct ip_header {
    Uint8 version_ihl;
    Uint8 tos;
    Uint16 length;
    Uint16 ident;
    Uint16 fragment;
    Uint8 ttl;
    Uint8 protocol;
    Uint16 checksum;
    Uint32 src;
    Uint32 dst;
    Uint8 options[IP_WORD_BYTES];
};

struct icmp_mask_reply {
    Uint16 id;
    Uint16 seq;
    Uint32 mask;
};

static Uint32 icmp_network(Uint32 addr);

/* -1 while ICMP_GetSubnetMask waits for a reply, until IP_Timer, which runs
 * once a second, sets it to 0 */
volatile Sint32 icmp_timer_1sec;
/* the first mask reply since its id was cleared, or an id of 0 for none */
static struct icmp_mask_reply mask_reply;

Sint32 ICMP_Recv(AT_MaxBuffs *buffers, Uint16 offset, Uint32 source)
{
    struct icmp_header *icmp;
#ifdef NONMATCHING
    /* this side copies the header, and the mask a reply may hold, into header.
     * length is the bytes the first buffer holds past offset, which bounds each
     * read below */
    struct icmp_header header;
    AT_Vptr header_pointer;
    Sint32 length;
#else
    void *data;
    Uint8 header[ICMP_HEADER_BYTES];
#endif
    AT_MaxBuffs *reply;
    Sint32 i;
    Sint32 j;

#ifdef NONMATCHING
    /* this side refuses a missing list, a bufnum outside 1 to AT_MAX_BUFFERS,
     * and a message shorter than offset and an ICMP header. the matching side
     * reads the header unchecked */
    if (!buffers || buffers->bufnum <= 0 || buffers->bufnum > AT_MAX_BUFFERS ||
        buffers->buffers[0].len < (Sint32)(offset + ICMP_HEADER_BYTES) ||
        buffers->total_len < (Sint32)(offset + ICMP_HEADER_BYTES)) {
        return -1;
    }
#endif
    buffers->total_len -= offset;
    buffers->buffers[0].len -= offset;
    OS_IncVptr(&buffers->buffers[0].vptr, offset);
#ifdef NONMATCHING
    /* this side's half of the repair of the bug tagged at ICMP_MASKREPLY: the
     * header and any mask are copied out of every kind of buffer. a local
     * buffer may hold them at an unaligned address */
    length = buffers->buffers[0].len;
    if (length > buffers->total_len) {
        length = buffers->total_len;
    }
    OS_SetVptrLocal(&header_pointer, &header);
    OS_Vmemcpy(&header_pointer, &buffers->buffers[0].vptr,
               length < ICMP_MASK_BYTES ? ICMP_HEADER_BYTES : ICMP_MASK_BYTES);
    icmp = &header;
#else
    /* @hack type: the type read as a Sint8, where the Uint8 field would do */
    if ((Sint8)buffers->buffers[0].vptr.type == AT_PTR_TYPE_LOCAL) {
        data = OS_GetVptrLocal(&buffers->buffers[0].vptr);
    } else {
        AT_Vptr vp;

        OS_SetVptrLocal(&vp, header);
        OS_Vmemcpy(&vp, &buffers->buffers[0].vptr, ICMP_HEADER_BYTES);
        data = header;
    }
    icmp = data;
#endif
    /* @bug logic: a message whose checksum fails is read as one that passes.
     * no branch reads the checksum's test (0x0cada286) */
    if (UDP_Chksum(0, buffers, 0) != 0) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a message whose checksum
         * fails is dropped */
        return -1;
#endif
    }
    switch (icmp->type) {
    case ICMP_ECHOREPLY:
        break;
    case ICMP_ECHO:
        if ((reply = OS_AllocBuffs()) == NULL) {
            return -1;
        }
        reply->bufnum = buffers->bufnum;
        reply->total_len = buffers->total_len;
        for (i = 0, j = buffers->bufnum; i < buffers->bufnum; i++) {
            j--;
            reply->buffers[i].len = buffers->buffers[j].len;
            memcpy(&reply->buffers[i].vptr, &buffers->buffers[j].vptr,
                   sizeof(AT_Vptr));
        }
        if (ICMP_Send(ICMP_ECHOREPLY, 0, source, reply) == -1) {
            OS_FreeBuffs(reply);
            return -1;
        }
        OS_FreeBuffs(reply);
        return 0;
    case ICMP_UNREACH: {
        struct ip_header ip;
        struct udp_hdr udp;
        AT_Vptr vp;

#ifdef NONMATCHING
        /* this side drops an error too short to quote an IP header, which the
         * matching side copies past the message's end */
        if (length < ICMP_HEADER_BYTES + IP_HEADER_BYTES) {
            return -1;
        }
#endif
        OS_IncVptr(&buffers->buffers[0].vptr, ICMP_HEADER_BYTES);
        OS_SetVptrLocal(&vp, &ip);
        OS_Vmemcpy(&vp, &buffers->buffers[0].vptr, IP_HEADER_BYTES);
        offset = (ip.version_ihl & IP_IHL_MASK) * IP_WORD_BYTES;
#ifdef NONMATCHING
        /* this side drops a quoted IP header shorter than IP_HEADER_BYTES or
         * longer than the message, and a UDP error too short to quote the UDP
         * header the matching side copies */
        if (offset < IP_HEADER_BYTES || offset > length - ICMP_HEADER_BYTES ||
            (ip.protocol == IPPROTO_UDP &&
             length < (Sint32)(ICMP_HEADER_BYTES + offset + sizeof(udp)))) {
            return -1;
        }
#endif
        OS_IncVptr(&buffers->buffers[0].vptr, offset);
        switch (ip.protocol) {
        case IPPROTO_UDP:
            OS_SetVptrLocal(&vp, &udp);
            OS_Vmemcpy(&vp, &buffers->buffers[0].vptr, sizeof(struct udp_hdr));
            UDP_ComeIcmpUnreach(udp.dport, ip.dst, icmp->code);
            break;
        case IPPROTO_TCP:
            break;
        }
        return 0;
    }
    case ICMP_SOURCEQUENCH:
        break;
    case ICMP_REDIRECT: {
        struct ip_header ip;
        AT_Vptr vp;
        Uint32 gateway;
        Uint32 host;
        Uint32 net;

#ifdef NONMATCHING
        /* this side drops a redirect too short to quote an IP header, which the
         * matching side copies past the message's end */
        if (length < ICMP_HEADER_BYTES + IP_HEADER_BYTES) {
            return -1;
        }
#endif
        OS_IncVptr(&buffers->buffers[0].vptr, ICMP_HEADER_BYTES);
        OS_SetVptrLocal(&vp, &ip);
        OS_Vmemcpy(&vp, &buffers->buffers[0].vptr, IP_HEADER_BYTES);
        if (icmp->code == ICMP_REDIRECT_NET ||
            icmp->code == ICMP_REDIRECT_TOSNET) {
            memcpy(&gateway, icmp->un.gateway, sizeof(gateway));
            net = icmp_network(ip.dst);
            IP_ComeIcmpRedirect(net, gateway, RTF_GATEWAY, source);
        } else {
            memcpy(&host, icmp->un.gateway, sizeof(host));
            IP_ComeIcmpRedirect(ip.dst, host, RTF_GATEWAY | RTF_HOST, source);
        }
        return 0;
    }
    case ICMP_TSTAMPREPLY:
        break;
    case ICMP_MASKREPLY:
        /* @bug memory: the mask's read below faults or runs past its copy,
         * whatever buffer holds the reply. a local buffer is the queued frame,
         * where the mask lies 14 + 4 * IHL + 8 bytes in. that is 2 mod 4, and
         * a 32-bit load faults there. from a pool or external buffer, header
         * copies ICMP_HEADER_BYTES bytes alone, and the read runs past them
         * (0x0cada446) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. it drops a reply too short to
         * hold the mask, and the mask it reads is header's, aligned, which the
         * copy above filled */
        if (length < ICMP_MASK_BYTES) {
            return -1;
        }
#endif
        if (mask_reply.id == 0) {
            mask_reply.seq = icmp->un.echo.seq;
            mask_reply.id = icmp->un.echo.id;
            mask_reply.mask = icmp->mask;
        }
        return 0;
    case ICMP_TSTAMP:
        break;
    case ICMP_MASKREQ:
        break;
    case ICMP_TIMXCEED:
        break;
    case ICMP_PARAMPROB:
        break;
    default:
        return -1;
    }
    return 0;
}

Sint32 ICMP_Send(Uint8 type, Uint8 code, Uint32 destination,
                 AT_MaxBuffs *buffers)
{
    struct icmp_header *icmp;
    Uint8 *p;
    struct icmp_header header;
    AT_Vptr vp;
    Uint8 *mask;
    Uint16 cksum;

#ifdef NONMATCHING
    /* this side refuses a missing list, and one with no room for the buffer
     * each case below adds at bufnum */
    if (!buffers || buffers->bufnum < 0 || buffers->bufnum >= AT_MAX_BUFFERS) {
        return -1;
    }
#endif
    icmp = &header;
    switch (type) {
    case ICMP_ECHOREPLY:
#ifdef NONMATCHING
        if (buffers->bufnum == 0 ||
            buffers->buffers[buffers->bufnum - 1].len < ICMP_HEADER_BYTES) {
            return -1;
        }
        /* the header is copied out of every kind of buffer and sent from
         * header. a local buffer may hold it at an unaligned address */
        {
#else
        /* @hack type: the type read as a Sint8, where the Uint8 field would
         * do */
        if ((Sint8)buffers->buffers[buffers->bufnum - 1].vptr.type ==
            AT_PTR_TYPE_LOCAL) {
            icmp = OS_GetVptrLocal(&buffers->buffers[buffers->bufnum - 1].vptr);
        } else {
#endif
            OS_SetVptrLocal(&vp, &header);
            OS_Vmemcpy(&vp, &buffers->buffers[buffers->bufnum - 1].vptr,
                       ICMP_HEADER_BYTES);
            OS_IncVptr(&buffers->buffers[buffers->bufnum - 1].vptr,
                       ICMP_HEADER_BYTES);
            buffers->buffers[buffers->bufnum - 1].len -=
                (buffers->buffers[buffers->bufnum].len = ICMP_HEADER_BYTES);
            OS_SetVptrLocal(&buffers->buffers[buffers->bufnum].vptr, &header);
            buffers->bufnum++;
        }
        break;
    case ICMP_UNREACH:
        header.un.unused = 0;
        OS_SetVptrLocal(&buffers->buffers[buffers->bufnum].vptr, &header);
        buffers->buffers[buffers->bufnum].len = ICMP_HEADER_BYTES;
        buffers->total_len += ICMP_HEADER_BYTES;
        buffers->bufnum++;
        break;
    case ICMP_MASKREQ:
        header.un.echo.id = ICMP_MASK_ID;
        header.un.echo.seq = ICMP_MASK_SEQ;
        mask = (Uint8 *)&header.mask;
        mask[0] = mask[1] = mask[2] = mask[3] = 0;
        OS_SetVptrLocal(&buffers->buffers[buffers->bufnum].vptr, &header);
        buffers->buffers[buffers->bufnum].len = ICMP_MASK_BYTES;
        buffers->total_len += ICMP_MASK_BYTES;
        buffers->bufnum++;
        break;
    default:
        return -1;
    }
    icmp->type = type;
    icmp->code = code;
    icmp->cksum = 0;
    p = (Uint8 *)&icmp->cksum;
    cksum = UDP_Chksum(0, buffers, 1);
    *p = cksum >> 8;
    p++;
    *p = cksum;
    if (IP_Send(buffers, destination, 0, 0, 0, 0, 0, 0, IPPROTO_ICMP, 0) != 0) {
        return -1;
    }
    return 0;
}

/* the network of a redirect for addr, in network order. it is addr's class
 * network, or its subnet where the interface shares that network */
static Uint32 icmp_network(Uint32 addr)
{
    Uint32 host;
    Uint32 local;
    Uint32 net;
    struct net_iface *entry;

    host = OS_Htonl(addr);
    entry = DL_GetIfEntry(0);
    if (entry == NULL) {
        return 0;
    }
    local = OS_Htonl(entry->addr);

    /* @bug logic: takes the interface's network by addr's class, where the
     * interface's own is meant, past class A. only an addr in 0.0.0.0/8 with
     * an interface outside class A comes out wrong. that network is then 0,
     * as addr's is. the result is then addr under the interface's mask, where
     * 0 is meant (0x0cada6ae, 0x0cada6b8) */
    if (IN_CLASSA(local)) {
        local &= IN_CLASSA_NET;
    } else if (IN_CLASSB(host)) {
        local &= IN_CLASSB_NET;
    } else if (IN_CLASSC(host)) {
        local &= IN_CLASSC_NET;
    } else {
        local = 0;
    }
    if (IN_CLASSA(host)) {
        net = host & IN_CLASSA_NET;
    } else if (IN_CLASSB(host)) {
        net = host & IN_CLASSB_NET;
    } else if (IN_CLASSC(host)) {
        net = host & IN_CLASSC_NET;
    } else {
        return 0;
    }
    if (net == local) {
        /* @bug logic: masks host, in host order, with the interface's mask,
         * which is in network order (0x0cada718) */
        host &= entry->mask;
        net = host;
    }
    return OS_Htonl(net);
}

Sint32 ICMP_GetSubnetMask(Uint32 *mask, Uint32 destination)
{
    AT_MaxBuffs packet;
    Sint32 tries;

    mask_reply.id = 0;
    mask_reply.seq = 0;
    mask_reply.mask = 0;
    for (tries = 0; tries < ICMP_MASK_TRIES; tries++) {
        icmp_timer_1sec = -1;
        packet.total_len = 0;
        packet.bufnum = 0;
        ICMP_Send(ICMP_MASKREQ, 0, destination, &packet);
        /* ICMP_Recv records a reply only while mask_reply.id is 0. no reply
         * comes during the send: the critical section held here masks the
         * vblank interrupt, which alone runs ICMP_Recv */
        mask_reply.id = 0;
        /* @hack redundant: the test of the cleared id, which never holds */
        if (mask_reply.id != 0) {
            *mask = mask_reply.mask;
            return 0;
        }
        OS_CriticalUnLock();
        while (icmp_timer_1sec == -1) {
            OS_Wait(1);
        }
        OS_CriticalLock();
        if (mask_reply.id == ICMP_MASK_ID && mask_reply.seq == ICMP_MASK_SEQ) {
            *mask = mask_reply.mask;
            return 0;
        }
    }
    return -1;
}

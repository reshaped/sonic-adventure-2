/* TCP's output: the segments a connection sends, its timers, and the
 * connections a listening tcb accepts */
#include <access/cardprobe.h>
#include <mdmAPI.h>
#include "../network/ppp_modem.h"
#include <access/ip_types.h>
#include <access/at_types.h>
#include <access/at_api.h>
#include <access/packet_queue.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>

/* the local ports TCP_GenarateMyPort counts through */
#define TCP_PORT_BASE 0x400
#define TCP_PORT_WRAP 0x7ff
/* with a window of TCP_SMALL_WINDOW bytes, TCP_Rawsend sends once it has
 * half a window's bytes */
#define TCP_SMALL_WINDOW 0x800

static const Uint8 const_tcp_initopt[TCPOLEN_MAXSEG] = {TCPOPT_MAXSEG,
                                                        TCPOLEN_MAXSEG, 0, 0};

Sint32 tcpnodelack = 1;
static Sint16 tcp_keepidle = TCPTV_KEEP_IDLE;
static Sint16 tcp_keepintvl = TCPTV_KEEPINTVL;
Uint32 tcb_struct_size = sizeof(struct tcb);
Uint32 tcprexmtthresh = 3;
/* the flags a segment carries in each TCPS_ state */
static Uint8 tcp_outflags[TCPS_TIME_WAIT + 1] = {
    TH_RST | TH_ACK, 0,      TH_SYN,          TH_SYN | TH_ACK,
    TH_ACK,          TH_ACK, TH_FIN | TH_ACK, TH_FIN | TH_ACK,
    TH_FIN | TH_ACK, TH_ACK, TH_ACK};
static Sint32 tcp_backoff[TCP_MAXRXTSHIFT + 1] = {1,  2,  4,  8,  16, 32, 64,
                                                  64, 64, 64, 64, 64, 64};

struct tcb *last_tcbp;
Sint32 tcp_maxidle;
Uint32 tcp_iss;
Uint8 tcp_initopt[TCPOLEN_MAXSEG];
Uint16 win_size;
Sint32 dropsocket;

static int tcp_port_used(struct tcb *connection, Uint16 port);
static void tcp_setpersist(struct tcb *connection);
int TCP_Init(void)
{
    int index;

    dropsocket = 0;
    memcpy(tcp_initopt, const_tcp_initopt, sizeof(tcp_initopt));
    for (index = 0; index < max_tcb_entry; index++) {
        tcb_table[index].listener = TCB_NO_LISTENER;
        tcb_table[index].in_use = 0;
    }
    win_size = tcp_win_size;
    tcp_iss = TIMER_GetCounter();
    tcp_port_num = 0;
    return 0;
}

int TCP_Dooptions(struct tcb *connection, Uint8 *options, int length,
                  struct tcphdr *header)
{
    /* @hack redundant: register on cursor, where a plain local would do */
    register Uint8 *cursor;
    int opt;
    int option_length;
    int remaining;

    cursor = options;
    remaining = length;
    for (; remaining > 0; remaining -= option_length, cursor += option_length) {
        opt = cursor[0];
        if (opt == TCPOPT_EOL) {
            break;
        }
        if (opt == TCPOPT_NOP) {
            option_length = 1;
        } else {
#ifdef NONMATCHING
            /* this side stops at an option whose length byte, or whose length,
             * runs past the options */
            if (remaining < 2 || cursor[1] > remaining) {
                break;
            }
#endif
            option_length = cursor[1];
            if (option_length <= 0) {
                break;
            }
        }
        switch (opt) {
        default:
            continue;
        case TCPOPT_MAXSEG:
            if (option_length != TCPOLEN_MAXSEG) {
                continue;
            }
            if (!(header->th_flags & TH_SYN)) {
                continue;
            }
            /* @bug memory: an MSS after an odd count of NOPs lies at an odd
             * address, where its halfword read faults (0x0cad8fbe) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it reads the MSS a byte
             * at a time */
            {
                Uint16 offered = (Uint16)((cursor[2] << 8) | cursor[3]);
                Uint16 limit = TCP_Mss(connection);
                connection->t_maxseg = TCP_MIN(offered, limit);
            }
#else
            connection->t_maxseg =
                TCP_MIN(OS_Htons(*(Uint16 *)(cursor + TCPOPT_VALUE_OFFSET)),
                        TCP_Mss(connection));
#endif
            break;
        }
    }
    return 0;
}

struct tcb *TCP_ComeSynListen(struct tcb *listener, const Uint16 *peer_port,
                              Uint32 faddr)
{
    int socket;
    struct tcb *connection;
    int index;
    int found = 0;

    socket = TCP_Socket();
    if (socket == -1) {
        return 0;
    }
    connection = &tcb_table[socket];
    for (index = 0; index < max_tcb_entry; index++) {
        /* @hack redundant: the test of the new socket's in_use, which never
         * holds. the listener is in use, so the walk finds it without the
         * test */
#ifdef NONMATCHING
        /* this side tests the walked entry's in_use, as the walk means. the
         * result is the same, since the listener it looks for is in use */
        if (tcb_table[index].in_use == 0) {
#else
        if (tcb_table[socket].in_use == 0) {
#endif
            continue;
        }
        if (&tcb_table[index] == listener) {
            connection->listener = index;
            found = 1;
            break;
        }
    }
    if (!found) {
#ifdef NONMATCHING
        /* this side frees the socket TCP_Socket took where no listener holds
         * it, which the matching side leaves in use */
        connection->in_use = 0;
#endif
        return 0;
    }
    connection->t_template.th_sport = listener->t_template.th_sport;
    if (listener->t_template.th_dport == 0) {
        connection->t_template.th_dport = *peer_port;
    } else {
        connection->t_template.th_dport = listener->t_template.th_dport;
    }
    if (listener->faddr == 0) {
        connection->faddr = faddr;
    } else {
        connection->faddr = listener->faddr;
    }
    connection->ip_options = listener->ip_options;
    connection->ip_optlen = listener->ip_optlen;
    connection->ip_tos = listener->ip_tos;
    connection->ip_df = listener->ip_df;
    connection->ip_ttl = listener->ip_ttl;
    connection->asr = listener->asr;
    if (TCP_SendWindowAlloc(connection) != 0) {
        /* @bug memory: the socket TCP_Socket took stays in use where its
         * send window can't be allocated, and no path frees it (0x0cad9100) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees the socket */
        connection->in_use = 0;
#endif
        return 0;
    }
    if (TCP_RecvWindowAlloc(connection) != 0) {
        /* @bug memory: the socket and its send window stay in use where its
         * receive window can't be allocated, and no path frees them
         * (0x0cad910a) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees the send window and
         * the socket */
        TCP_SendWindowFree(connection);
        connection->in_use = 0;
#endif
        return 0;
    }
    return connection;
}

Uint16 TCP_Mss(struct tcb *connection)
{
    return ETHERNET_MTU - IP_HEADER_BYTES - sizeof(struct tcphdr);
}

int TCP_TcbDelete(struct tcb *connection)
{
    if (connection) {
        IP_FreeRoute(&connection->route);
        if (connection->flags & TF_CLOSED) {
            TCP_SendWindowFree(connection);
            TCP_RecvWindowFree(connection);
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_LAST);
            connection->in_use = 0;
            return 0;
        }
        connection->state = TCPS_CLOSED;
        TCP_CancelAllTimer(connection);
    }
    return 0;
}

Uint16 TCP_GenarateMyPort(struct tcb *connection)
{
    if (tcp_port_num == 0) {
        tcp_port_num =
            (TIMER_GetCounter() & (TCP_PORT_BASE - 1)) | TCP_PORT_BASE;
    }
    while (tcp_port_used(connection, OS_Htons(++tcp_port_num)) == 1) {
        if (tcp_port_num >= TCP_PORT_WRAP) {
            tcp_port_num = TCP_PORT_BASE;
        }
    }
    return OS_Htons(tcp_port_num);
}

static int tcp_port_used(struct tcb *connection, Uint16 port)
{
    int index;

    for (index = 0; index < max_tcb_entry; index++) {
        if (tcb_table[index].in_use == 0) {
            continue;
        }
        if (&tcb_table[index] == connection) {
            continue;
        }
        if (port == tcb_table[index].t_template.th_sport) {
            return 1;
        }
    }
    return 0;
}

int TCP_SendDirect(Uint32 dst, Uint16 sport, Uint16 dport, Uint32 seq,
                   Uint32 ack, Uint8 flags)
{
    struct tcp_pseudo pseudo;
    struct tcphdr header;
    Uint8 buf[sizeof(struct tcphdr)];
    RtEntry *route;
    AT_MaxBuffs *buffers;
    Uint8 *p;
    Uint16 sum;

    if ((route = IP_RouteSearch(dst)) == 0) {
        return -1;
    }
    header.th_sport = sport;
    header.th_dport = dport;
    header.th_seq = seq;
    header.th_ack = ack;
    header.th_off = TH_OFF(sizeof(struct tcphdr));
    header.th_flags = flags;
    header.th_win = header.th_sum = header.th_urp = 0;
    if ((buffers = OS_AllocBuffs()) == 0) {
        return -1;
    }
    buffers->bufnum = 1;
    buffers->total_len = sizeof(buf);
    buffers->buffers[0].len = sizeof(buf);
    memcpy(buf, &header.th_sport, 2);
    memcpy(buf + 2, &header.th_dport, 2);
    memcpy(buf + 4, &header.th_seq, 4);
    memcpy(buf + 8, &header.th_ack, 4);
    memcpy(buf + 12, &header.th_off, 1);
    memcpy(buf + 13, &header.th_flags, 1);
    memcpy(buf + 14, &header.th_win, 2);
    memcpy(buf + 16, &header.th_sum, 2);
    memcpy(buf + 18, &header.th_urp, 2);
    OS_SetVptrLocal(&buffers->buffers[0].vptr, buf);
    if (dst == OS_Htonl(INADDR_LOOPBACK)) {
        pseudo.src = OS_Htonl(INADDR_LOOPBACK);
    } else {
        pseudo.src = IP_GetMyaddrRoute(route);
    }
    pseudo.dst = dst;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_TCP;
    pseudo.len = OS_Htons(buffers->total_len);
    p = (Uint8 *)&header.th_sum;
    sum = UDP_Chksum(&pseudo, buffers, 1);
    *p = sum >> 8;
    p++;
    *p = sum;
    memcpy(buf + 16, &header.th_sum, 2);
    if (IP_Send(buffers, dst, 0, 0, 0, 0, 0, 0, IPPROTO_TCP, 0)) {
        OS_FreeBuffs(buffers);
        return -1;
    }
    OS_FreeBuffs(buffers);
    return 0;
}

int TCP_Rawsend(struct tcb *connection)
{
    /* @hack redundant: register on len and win, where plain locals would do */
    register long len;
    register Uint16 win;
    long off;
    int flags;
    int idle;
    int sendalot;
    struct tcphdr *header;
    Uint8 *opt;
    AT_MaxBuffs *buffers;
    RtEntry *route;
    struct tcp_pseudo pseudo;
    Uint8 *p;
    Uint16 sum;
    long send_count;

    header = &connection->t_template;
    idle = connection->snd_max == connection->snd_una;
again:
    if (DL_CheckSend() == -1) {
        last_tcbp = connection;
        return -1;
    }
    last_tcbp = 0;
    sendalot = 0;
    off = connection->snd_nxt - connection->snd_una;
    win = TCP_MIN(connection->snd_wnd, connection->snd_cwnd);
    if (connection->t_force) {
        if (win == 0) {
            win = 1;
        } else {
            connection->t_timer[TCPT_PERSIST] = 0;
            connection->t_rxtshift = 0;
        }
    }
    len = TCP_MIN((Uint32)TCP_SendWindowCc(connection), win) - off;
    flags = tcp_outflags[connection->state];
    if (len < 0) {
        len = 0;
        if (win == 0) {
            connection->t_timer[TCPT_REXMT] = 0;
            connection->snd_nxt = connection->snd_una;
        }
    }
    if ((Uint32)len > connection->t_maxseg) {
        len = connection->t_maxseg;
        sendalot = 1;
    }
    if (SEQ_LT(connection->snd_nxt + len - connection->snd_una,
               TCP_SendWindowCc(connection))) {
        flags &= ~TH_FIN;
    }
    win = TCP_RecvWindowSpace(connection);
    if ((flags & TH_FIN) && ((connection->flags & TF_SENTFIN) == 0 ||
                             connection->snd_nxt == connection->snd_una)) {
        goto send;
    }
    if (connection->flags & TF_ACKNOW) {
        goto send;
    }
    if (flags & (TH_SYN | TH_RST)) {
        goto send;
    }
    if (SEQ_GT(connection->snd_up, connection->snd_una)) {
        goto send;
    }
    if (len) {
        if (len == connection->t_maxseg) {
            goto send;
        }
        if (sendalot || len >= tcp_win_size) {
            goto send;
        }
        if (tcp_win_size == TCP_SMALL_WINDOW && len >= TCP_SMALL_WINDOW / 2) {
            goto send;
        }
        if (connection->external_send == 1) {
            send_count = TCP_SendWindowCc(connection);
        } else {
            send_count = TCP_MIN(TCP_SendWindowCc(connection), win_size);
        }
        if (idle && off + len >= send_count) {
            goto send;
        }
        if (connection->t_force) {
            goto send;
        }
        if ((Uint32)len >= connection->max_sndwnd / 2) {
            goto send;
        }
        if (connection->snd_nxt < connection->snd_max) {
            goto send;
        }
    }
    if (win > 0) {
        long adv = win - (long)(connection->rcv_adv - connection->rcv_nxt);

        if ((Uint32)adv >= 2 * connection->t_maxseg) {
            goto send;
        }
        if (2 * adv >= (long)TCP_RecvWindowSize(connection)) {
            goto send;
        }
    }
    if (TCP_SendWindowCc(connection) && connection->t_timer[TCPT_REXMT] == 0 &&
        connection->t_timer[TCPT_PERSIST] == 0) {
        connection->t_rxtshift = 0;
        tcp_setpersist(connection);
    }
    return 0;

send:
    /* @hack redundant: the chain of tests on len, t_force, the sequence
     * numbers and the flags, whose branches are all empty */
    if (len) {
        if (connection->t_force && len == 1) {
        } else if (connection->snd_nxt < connection->snd_max) {
        } else {
        }
    } else if (connection->flags & TF_ACKNOW) {
    } else if (flags & (TH_SYN | TH_FIN | TH_RST)) {
    } else if (connection->snd_up > connection->snd_una) {
    } else {
    }
    if ((flags & TH_FIN) && (connection->flags & TF_SENTFIN) &&
        connection->snd_nxt == connection->snd_max) {
        connection->snd_nxt--;
    }
    header->th_seq = OS_Htonl(connection->snd_nxt);
    header->th_ack = OS_Htonl(connection->rcv_nxt);
    opt = 0;
    if (flags & TH_SYN) {
        opt = tcp_initopt;
        *(Uint16 *)(opt + TCPOPT_VALUE_OFFSET) =
            OS_Htons(TCP_MIN(win_size, TCP_Mss(connection)));
    }
    /* @hack redundant: the test of opt, whose branch is empty */
    if (opt) {
    }
    header->th_flags = flags;
    if ((Uint32)win < TCP_RecvWindowSize(connection) / 4 &&
        (Uint32)win < connection->t_maxseg) {
        win = 0;
    }
    if (win < (long)(connection->rcv_adv - connection->rcv_nxt)) {
        win = connection->rcv_adv - connection->rcv_nxt;
    }
    if (win > win_size) {
        win = win_size;
    }
    header->th_win = OS_Htons(win);
    if (SEQ_GT(connection->snd_up, connection->snd_nxt)) {
        header->th_urp = OS_Htons(connection->snd_up - connection->snd_nxt);
        flags |= TH_URG;
    } else {
        connection->snd_up = connection->snd_una;
    }
    if (len && off + len == TCP_SendWindowCc(connection)) {
        flags |= TH_PUSH;
    }
    if ((route = IP_RouteSearch(connection->faddr)) == 0) {
        API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                    ASR_NO_ROUTE);
        return -1;
    }
    header->th_sum = 0;
    header->th_flags = flags;
    if (opt) {
        header->th_off = TH_OFF(sizeof(struct tcphdr) + TCPOLEN_MAXSEG);
    } else {
        header->th_off = TH_OFF(sizeof(struct tcphdr));
    }
    if ((buffers = OS_AllocBuffs()) == 0) {
        return -1;
    }
    buffers->bufnum = 0;
    buffers->total_len = 0;
    if (TCP_GetSendWindow(off, len, buffers, connection) == -1) {
        buffers->bufnum = 0;
        buffers->total_len = 0;
    }
    if (opt) {
        buffers->bufnum++;
        buffers->total_len += TCPOLEN_MAXSEG;
        buffers->buffers[buffers->bufnum - 1].len = TCPOLEN_MAXSEG;
        OS_SetVptrLocal(&buffers->buffers[buffers->bufnum - 1].vptr, opt);
    }
    buffers->bufnum++;
    buffers->total_len += sizeof(struct tcphdr);
    buffers->buffers[buffers->bufnum - 1].len = sizeof(struct tcphdr);
    OS_SetVptrLocal(&buffers->buffers[buffers->bufnum - 1].vptr, header);
    if (connection->faddr == OS_Htonl(INADDR_LOOPBACK)) {
        pseudo.src = OS_Htonl(INADDR_LOOPBACK);
    } else {
        pseudo.src = IP_GetMyaddrRoute(route);
    }
    pseudo.dst = connection->faddr;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_TCP;
    pseudo.len = OS_Htons(buffers->total_len);
    p = (Uint8 *)&header->th_sum;
    sum = UDP_Chksum(&pseudo, buffers, 1);
    *p = sum >> 8;
    p++;
    *p = sum;
    if (connection->t_force == 0 || connection->t_timer[TCPT_PERSIST] == 0) {
        Uint32 startseq = connection->snd_nxt;

        if (flags & TH_SYN) {
            connection->snd_nxt++;
        }
        if (flags & TH_FIN) {
            connection->snd_nxt++;
            connection->flags |= TF_SENTFIN;
        }
        connection->snd_nxt += len;
        if (connection->snd_nxt > connection->snd_max) {
            connection->snd_max = connection->snd_nxt;
            if (connection->t_rtt == 0) {
                connection->t_rtt = 1;
                connection->t_rtseq = startseq;
            }
        }
        if (connection->t_timer[TCPT_REXMT] == 0 &&
            connection->snd_nxt != connection->snd_una) {
            connection->t_timer[TCPT_REXMT] = connection->t_rxtcur;
            if (connection->t_timer[TCPT_PERSIST]) {
                connection->t_timer[TCPT_PERSIST] = 0;
                connection->t_rxtshift = 0;
            }
        }
    } else if (connection->snd_nxt + len - connection->snd_max > 0) {
        connection->snd_max = connection->snd_nxt + len;
    }
    if (IP_Send(buffers, connection->faddr, 0, connection->ip_ttl,
                connection->ip_tos, connection->ip_df, connection->ip_optlen,
                connection->ip_options, IPPROTO_TCP, 0)) {
        OS_FreeBuffs(buffers);
        return -1;
    }
    OS_FreeBuffs(buffers);
    if (win > 0 && SEQ_GT(connection->rcv_nxt + win, connection->rcv_adv)) {
        connection->rcv_adv = connection->rcv_nxt + win;
    }
    connection->flags &= ~(TF_ACKNOW | TF_DELACK);
    if (sendalot) {
        if (AT_CardType == AT_CARD_TYPE_PPP) {
            mdmServicePort(ppp_modem_port[0]);
        }
        goto again;
    }
    return 0;
}

static void tcp_setpersist(struct tcb *connection)
{
    int rexmt = TCP_REXMTVAL(connection);

    /* @hack redundant: the test of the retransmit timer, whose branch is
     * empty */
    if (connection->t_timer[TCPT_REXMT]) {
    }
    connection->t_timer[TCPT_PERSIST] =
        rexmt * tcp_backoff[connection->t_rxtshift];
    if (connection->t_timer[TCPT_PERSIST] < TCPTV_PERSMIN) {
        connection->t_timer[TCPT_PERSIST] = TCPTV_PERSMIN;
    } else if (connection->t_timer[TCPT_PERSIST] > TCPTV_PERSMAX) {
        connection->t_timer[TCPT_PERSIST] = TCPTV_PERSMAX;
    }
    if (connection->t_rxtshift < TCP_MAXRXTSHIFT) {
        connection->t_rxtshift++;
    }
}

void TCP_CancelAllTimer(struct tcb *connection)
{
    int index;

    for (index = 0; index < TCPT_NTIMERS; index++) {
        connection->t_timer[index] = 0;
    }
}

void TCP_Timer(void)
{
    /* @hack redundant: register on index and j, where plain locals would do */
    register int index;
    register int j;

    tcp_maxidle = tcp_keepintvl * TCPTV_KEEPCNT;
    for (index = 0; index < max_tcb_entry; index++) {
        if (tcb_table[index].in_use == 0) {
            continue;
        }
        if (tcb_table[index].in_use != 1) {
            continue;
        }
        for (j = 0; j < TCPT_NTIMERS; j++) {
            if (tcb_table[index].t_timer[j] &&
                --tcb_table[index].t_timer[j] == 0) {
                TCP_TimerExpire(&tcb_table[index], j);
            }
        }
        tcb_table[index].t_idle++;
        if (tcb_table[index].t_rtt) {
            tcb_table[index].t_rtt++;
        }
    }
    tcp_iss += TCP_ISSINCR / PR_SLOWHZ;
}

int TCP_TimerExpire(struct tcb *connection, int timer)
{
    /* @hack redundant: register on rexmt, where a plain local would do */
    register int rexmt;
    int win;

    switch (timer) {
    case TCPT_2MSL:
        if (connection->state != TCPS_TIME_WAIT &&
            connection->t_idle <= tcp_maxidle) {
            connection->t_timer[TCPT_2MSL] = tcp_keepintvl;
        } else {
            TCP_TcbDelete(connection);
        }
        break;
    case TCPT_REXMT:
        if (++connection->t_rxtshift > TCP_MAXRXTSHIFT) {
            connection->t_rxtshift = TCP_MAXRXTSHIFT;
            TCP_Drop(connection, TCP_ETIMEDOUT);
            break;
        }
        rexmt = TCP_REXMTVAL(connection) * tcp_backoff[connection->t_rxtshift];
        connection->t_rxtcur = rexmt;
        if (connection->t_rxtcur < TCPTV_MIN) {
            connection->t_rxtcur = TCPTV_MIN;
        } else if (connection->t_rxtcur > TCPTV_REXMTMAX) {
            connection->t_rxtcur = TCPTV_REXMTMAX;
        }
        connection->t_timer[TCPT_REXMT] = connection->t_rxtcur;
        if (connection->t_rxtshift > TCP_MAXRXTSHIFT / 4) {
            connection->t_rttvar += connection->t_srtt >> 2;
            connection->t_srtt = 0;
        }
        connection->snd_nxt = connection->snd_una;
        connection->t_rtt = 0;
        win = TCP_MIN(connection->snd_wnd, connection->snd_cwnd) / 2 /
              connection->t_maxseg;
        if (win < 2) {
            win = 2;
        }
        connection->snd_cwnd = connection->t_maxseg;
        connection->snd_ssthresh = win * connection->t_maxseg;
        TCP_Rawsend(connection);
        break;
    case TCPT_PERSIST:
        tcp_setpersist(connection);
        connection->t_force = 1;
        TCP_Rawsend(connection);
        connection->t_force = 0;
        break;
    case TCPT_KEEP:
        if (connection->state < TCPS_ESTABLISHED) {
            goto dropit;
        }
        if (connection->keepidle && connection->state <= TCPS_CLOSE_WAIT) {
            if (connection->t_idle >=
                connection->keepidle +
                    TCP_MIN(tcp_maxidle,
                            connection->keepidle * TCPTV_KEEPCNT)) {
                goto dropit;
            }
            TCP_SendDirect(connection->faddr, connection->t_template.th_sport,
                           connection->t_template.th_dport,
                           OS_Htonl(connection->snd_una - 1),
                           OS_Htonl(connection->rcv_nxt), TH_ACK);
            connection->t_timer[TCPT_KEEP] =
                TCP_MIN(tcp_keepintvl, connection->keepidle);
        } else if (connection->keepidle) {
            connection->t_timer[TCPT_KEEP] = connection->keepidle;
        } else {
            connection->t_timer[TCPT_KEEP] = tcp_keepidle;
        }
        break;
    dropit:
        TCP_Drop(connection, TCP_ETIMEDOUT);
        break;
    }
    return 0;
}

int TCP_GetErrcode(int socket)
{
    return tcb_table[socket].errcode;
}

int TCP_Drop(struct tcb *connection, int err)
{
    int socket = connection - tcb_table;
    Sint32 len =
        connection->rcv_up - connection->rcv_nxt + connection->receive_done;

    if (connection->state >= TCPS_SYN_RECEIVED) {
        connection->state = TCPS_CLOSED;
        TCP_Rawsend(connection);
    }
    /* @hack redundant: the empty else */
    else {
    }
    connection->errcode = err;
    if (err == TCP_ETIMEDOUT) {
        API_SendNotify(socket, connection->write_request, API_TIMEOUT);
        API_RecvNotify(socket, connection->read_request, API_TIMEOUT, len);
        API_ASRCall(socket, connection->asr, ASR_TCP, ASR_TIMEOUT);
    } else {
        API_RecvNotify(socket, connection->read_request, API_CONNRESET, len);
    }
    connection->flags |= TF_CLOSED;
    return TCP_TcbDelete(connection);
}

/* TCP's input: the segments IP_Recv hands up */
#include <access/ip_types.h>
#include <access/at_types.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <string.h>

static Sint32 received_flags;

Sint32 TCP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_length, Uint32 src,
                Uint32 dst)
{
    struct tcb *connection;
    struct tcphdr *header;
    int flags;
    Uint16 payload_length;
    Uint32 sequence;
    Uint16 header_length;
    int option_length;
    int dropped;
    int rst;
    int need_output;
    int syn;
    Uint32 offset;
    Uint8 *segment;
    Uint8 *header_bytes;
    Uint8 *options;
    Uint32 iss;
    int index;
    struct tcb *child;
    int trim;
    int acknowledged;
    /* @hack redundant: register on fin_acknowledged, send_count, delta and
     * incr, where plain locals would do */
    register int fin_acknowledged;
    register int send_count;
    Uint16 win;
    Uint32 saved_next;
    register Sint16 delta;
    register Uint16 incr;
    int cwnd;
    Uint16 len;
    struct tcp_pseudo pseudo;
    struct tcphdr decoded;

    need_output = 0;
    iss = 0;
    dropped = 0;
#ifdef NONMATCHING
    /* the copy below reads the TCP header's fixed fields, past the IP header,
     * from the first buffer */
    if (packet->bufnum <= 0 || packet->bufnum > AT_MAX_BUFFERS ||
        packet->total_len <
            (Sint32)(ip_header_length + sizeof(struct tcphdr)) ||
        packet->buffers[0].len <
            (Sint32)(ip_header_length + sizeof(struct tcphdr))) {
        return -1;
    }
#endif
    packet->total_len -= ip_header_length;
    packet->buffers[0].len -= ip_header_length;
    OS_IncVptr(&packet->buffers[0].vptr, ip_header_length);
    segment = OS_GetVptrLocal(&packet->buffers[0].vptr);
    header_bytes = (Uint8 *)&decoded;
    memcpy(header_bytes, segment, 2);
    memcpy(header_bytes + 2, segment + 2, 2);
    memcpy(header_bytes + 4, segment + 4, 4);
    memcpy(header_bytes + 8, segment + 8, 4);
    memcpy(header_bytes + 12, segment + 12, 1);
    memcpy(header_bytes + 13, segment + 13, 1);
    memcpy(header_bytes + 14, segment + 14, 2);
    memcpy(header_bytes + 16, segment + 16, 2);
    memcpy(header_bytes + 18, segment + 18, 2);
    header = (struct tcphdr *)header_bytes;
    received_flags = flags = header->th_flags;
#ifdef NONMATCHING
    /* this side reads the length from th_off's high 4 bits alone, where
     * TH_OFF_BYTES counts the low 4 too, and drops a header shorter than a
     * tcphdr or longer than the first buffer */
    header_length = (header->th_off >> 4) * 4;
    if (header_length < sizeof(struct tcphdr) ||
        header_length > packet->buffers[0].len) {
        return -1;
    }
#else
    header_length = TH_OFF_BYTES(header->th_off);
#endif

    /* @hack spelling: sizeof(struct tcphdr) as the distance between two
     * addresses, where the sizeof alone would do */
    if ((Uint32)packet->total_len <
        (Uint32)(header_bytes + sizeof(struct tcphdr)) - (Uint32)header_bytes) {
        return -1;
    }

    /* @hack redundant: the step of header by flags and header_length, which
     * never runs */
    if (0) {
        header += flags + header_length;
    }
    pseudo.src = src;
    pseudo.dst = dst;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_TCP;
    pseudo.len = OS_Htons(packet->total_len);
    if (UDP_Chksum(&pseudo, packet, 0) != 0) {
        return 0;
    }
    option_length = header_length - sizeof(struct tcphdr);
    if (option_length != 0) {
        options = (Uint8 *)OS_GetVptrLocal(&packet->buffers[0].vptr) +
                  sizeof(struct tcphdr);
    } else {
        options = 0;
    }

    /* @hack spelling: the test of 2.0F, which always holds, ahead of
     * header_length's, where header_length's alone would do */
    /* NOLINTNEXTLINE(clang-diagnostic-literal-conversion) */
    if (2.0F && header_length < sizeof(struct tcphdr)) {
        goto drop;
    }
    if ((Uint16)packet->total_len < header_length) {
        return 0;
    }
    memcpy(&sequence, &header->th_seq, sizeof(sequence));
    sequence = OS_Htonl(sequence);
    memcpy(&header->th_seq, &sequence, sizeof(sequence));
    memcpy(&sequence, &header->th_ack, sizeof(sequence));
    sequence = OS_Htonl(sequence);
    memcpy(&header->th_ack, &sequence, sizeof(sequence));
    header->th_win = OS_Htons(header->th_win);
    header->th_urp = OS_Htons(header->th_urp);
    payload_length = packet->total_len - header_length;

findpcb:
    connection = 0;
    for (index = 0; index < max_tcb_entry; index++) {
        if (tcb_table[index].in_use == 0) {
            continue;
        }
        if (tcb_table[index].state != TCPS_LISTEN &&
            tcb_table[index].faddr == src &&
            tcb_table[index].t_template.th_dport == header->th_sport &&
            tcb_table[index].t_template.th_sport == header->th_dport) {
            connection = &tcb_table[index];
            break;
        }
    }
    if (connection == 0) {
        for (index = 0; index < max_tcb_entry; index++) {
            if (tcb_table[index].in_use == 0) {
                continue;
            }
            if (tcb_table[index].state == TCPS_LISTEN &&
                tcb_table[index].t_template.th_sport == header->th_dport &&
                (tcb_table[index].faddr == src ||
                 tcb_table[index].faddr == 0) &&
                (tcb_table[index].t_template.th_dport == header->th_sport ||
                 tcb_table[index].t_template.th_dport == 0)) {
                connection = &tcb_table[index];
                break;
            }
        }
    }
    rst = flags & TH_RST;
    syn = flags & TH_SYN;
    if (connection == 0) {
        goto dropwithreset;
    }
    if (connection->state == TCPS_CLOSED) {
        if (connection->flags & TF_CLOSED) {
            goto drop;
        } else if (connection->shut & SHUT_RECV) {
            goto drop;
        }
    }
    if (connection->state != TCPS_LISTEN) {
        connection->t_idle = 0;
        connection->t_timer[TCPT_KEEP] = connection->keepidle;
    }
    if (options && connection->state != TCPS_LISTEN) {
        TCP_Dooptions(connection, options, option_length, header);
    }
    connection->rcv_wnd = connection->rcv_adv - connection->rcv_nxt;

    switch (connection->state) {
    case TCPS_LISTEN:
        if (rst) {
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_RESET);
            goto drop;
        }
        if (flags & TH_ACK) {
            goto dropwithreset;
        }
        if (!syn) {
            goto drop;
        }
        if (IP_IsBroadcast(dst, 0) == 1) {
            goto drop;
        }
        if ((child = TCP_ComeSynListen(connection, &header->th_sport, src)) ==
            0) {
            goto drop;
        }
        if (options) {
            TCP_Dooptions(child, options, option_length, header);
        }
        if (iss) {
            child->iss = iss;
        } else {
            child->iss = tcp_iss;
        }
        tcp_iss += TCP_ISSINCR_CONNECTION;
        child->irs = header->th_seq;
        child->snd_una = child->snd_nxt = child->snd_max = child->snd_up =
            child->iss;
        child->rcv_adv = child->rcv_nxt = child->irs + 1;
        child->flags |= TF_ACKNOW;
        child->state = TCPS_SYN_RECEIVED;
        child->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
        child->snd_cwnd = TCP_SendWindowSpace(child);
        dropsocket = 0;
        API_AcceptNotify(connection - tcb_table, connection->accept_req,
                         child - tcb_table, child->faddr,
                         child->t_template.th_sport,
                         child->t_template.th_dport);
        connection = child;
        goto trimthenstep6;

    case TCPS_SYN_SENT:
        if (((flags & TH_ACK) && connection->iss >= header->th_ack) ||
            header->th_ack > connection->snd_max) {
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_REFUSED);
            goto dropwithreset;
        }
        if (rst) {
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_RESET);
            if (flags & TH_ACK) {
                TCP_Drop(connection, TCP_ECONNRESET);
            }
            goto drop;
        }
        if (!syn) {
            goto drop;
        }
        if (flags & TH_ACK) {
            connection->snd_una = header->th_ack;
            if (connection->snd_nxt < connection->snd_una) {
                connection->snd_nxt = connection->snd_una;
            }
        }
        connection->t_timer[TCPT_REXMT] = 0;
        connection->irs = header->th_seq;
        connection->rcv_adv = connection->rcv_nxt = connection->irs + 1;
        connection->flags |= TF_ACKNOW;
        if ((flags & TH_ACK) && connection->snd_una > connection->iss) {
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_CONNECTED);
            connection->state = TCPS_ESTABLISHED;
            connection->t_maxseg =
                TCP_MIN(connection->t_maxseg, TCP_Mss(connection));
            if (connection->t_rtt) {
                connection->t_srtt = connection->t_rtt << TCP_RTT_SHIFT;
                connection->t_rttvar = connection->t_rtt
                                       << (TCP_RTTVAR_SHIFT - 1);
                connection->t_rxtcur = TCP_REXMTVAL(connection);
                if (connection->t_rxtcur < TCPTV_MIN) {
                    connection->t_rxtcur = TCPTV_MIN;
                } else if (connection->t_rxtcur > TCPTV_REXMTMAX) {
                    connection->t_rxtcur = TCPTV_REXMTMAX;
                }
                connection->t_rtt = 0;
            }
        } else {
            connection->state = TCPS_SYN_RECEIVED;
        }

    trimthenstep6:
        header->th_seq++;
        if (payload_length > connection->rcv_wnd) {
            trim = payload_length - connection->rcv_wnd;
            payload_length = connection->rcv_wnd;
            flags &= ~TH_FIN;
        }
        connection->snd_wl1 = header->th_seq - 1;
        connection->rcv_up = header->th_seq;
        goto step6;
    }

    trim = connection->rcv_nxt - header->th_seq;
    if (trim > 0) {
        if (syn) {
            flags &= ~TH_SYN;
            header->th_seq++;
            if (header->th_urp > 1) {
                header->th_urp--;
            } else {
                flags &= ~TH_URG;
            }
            trim--;
        }
        if (trim > payload_length ||
            (trim == payload_length && (flags & TH_FIN) == 0)) {
            if ((flags & TH_FIN) && trim == payload_length + 1) {
                trim = payload_length;
                flags &= ~TH_FIN;
                connection->flags |= TF_ACKNOW;
            } else {
                goto dropafterack;
            }
        }
        /* @hack redundant: the empty else */
        else {
        }
        dropped += trim;
        header->th_seq += trim;
        payload_length -= trim;
        if (header->th_urp > (Uint16)trim) {
            header->th_urp -= trim;
        } else {
            flags &= ~TH_URG;
            header->th_urp = 0;
        }
    }

    trim = header->th_seq + payload_length - connection->rcv_nxt -
           connection->rcv_wnd;
    if (trim > 0) {
        if (trim >= payload_length) {
            if ((flags & TH_SYN) && connection->state == TCPS_TIME_WAIT &&
                header->th_seq > connection->rcv_nxt) {
                iss = connection->rcv_nxt + TCP_ISSINCR;
                API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                            ASR_CLOSED);
                connection->flags |= TF_CLOSED;
                TCP_TcbDelete(connection);
                goto findpcb;
            }
            if (connection->rcv_wnd == 0 &&
                header->th_seq == connection->rcv_nxt) {
                connection->flags |= TF_ACKNOW;
            } else {
                goto dropafterack;
            }
        }
        payload_length -= trim;
        flags &= ~(TH_PUSH | TH_FIN);
    }

    if (flags & TH_RST) {
        API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                    ASR_RESET);
        switch (connection->state) {
        case TCPS_SYN_RECEIVED:
        case TCPS_ESTABLISHED:
        case TCPS_FIN_WAIT_1:
        case TCPS_FIN_WAIT_2:
        case TCPS_CLOSE_WAIT:
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_CLOSED);
            TCP_Drop(connection, TCP_ECONNRESET);
            goto drop;
        case TCPS_CLOSING:
        case TCPS_LAST_ACK:
        case TCPS_TIME_WAIT:
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_CLOSED);
            connection->flags |= TF_CLOSED;
            TCP_TcbDelete(connection);
            goto drop;
        }
    }

    if (flags & TH_SYN) {
        TCP_Drop(connection, TCP_ESYNINWINDOW);
        goto dropwithreset;
    }

    if ((flags & TH_ACK) == 0) {
        goto drop;
    }

    switch (connection->state) {
    case TCPS_SYN_RECEIVED:
        if (connection->snd_una > header->th_ack ||
            header->th_ack > connection->snd_max) {
            goto dropwithreset;
        }
        API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                    ASR_CONNECTED);
        connection->state = TCPS_ESTABLISHED;
        connection->t_maxseg =
            TCP_MIN(connection->t_maxseg, TCP_Mss(connection));
        connection->snd_wl1 = header->th_seq - 1;
        /* fall through */
    case TCPS_ESTABLISHED:
    case TCPS_FIN_WAIT_1:
    case TCPS_FIN_WAIT_2:
    case TCPS_CLOSE_WAIT:
    case TCPS_CLOSING:
    case TCPS_LAST_ACK:
    case TCPS_TIME_WAIT:
        if (SEQ_LEQ(header->th_ack, connection->snd_una)) {
            if (payload_length == 0 && header->th_win == connection->snd_wnd) {
                if (connection->t_timer[TCPT_REXMT] == 0 ||
                    header->th_ack != connection->snd_una) {
                    connection->t_dupacks = 0;
                } else if (++connection->t_dupacks == (Uint8)tcprexmtthresh) {
                    saved_next = connection->snd_nxt;
                    win = TCP_MIN(connection->snd_wnd, connection->snd_cwnd) /
                          2 / connection->t_maxseg;
                    if (win < 2) {
                        win = 2;
                    }
                    connection->snd_ssthresh = win * connection->t_maxseg;
                    connection->t_timer[TCPT_REXMT] = 0;
                    connection->t_rtt = 0;
                    connection->snd_nxt = header->th_ack;
                    connection->snd_cwnd = connection->t_maxseg;
                    TCP_Rawsend(connection);
                    if (saved_next > connection->snd_nxt) {
                        connection->snd_nxt = saved_next;
                    }
                    goto drop;
                }
            } else {
                connection->t_dupacks = 0;
            }
            break;
        }
        connection->t_dupacks = 0;
        if (header->th_ack > connection->snd_max) {
            goto dropafterack;
        }
        acknowledged = header->th_ack - connection->snd_una;
        if (connection->t_rtt && SEQ_GT(header->th_ack, connection->t_rtseq)) {
            if (connection->t_srtt != 0) {
                delta = connection->t_rtt - 1 -
                        (connection->t_srtt >> TCP_RTT_SHIFT);
                if ((connection->t_srtt += delta) <= 0) {
                    connection->t_srtt = 1;
                }
                if (delta < 0) {
                    delta = -delta;
                }
                delta -= connection->t_rttvar >> TCP_RTTVAR_SHIFT;
                if ((connection->t_rttvar += delta) <= 0) {
                    connection->t_rttvar = 1;
                }
            } else {
                connection->t_srtt = connection->t_rtt << TCP_RTT_SHIFT;
                connection->t_rttvar = connection->t_rtt
                                       << (TCP_RTTVAR_SHIFT - 1);
            }
            connection->t_rtt = 0;
            connection->t_rxtshift = 0;
            connection->t_rxtcur = TCP_REXMTVAL(connection);
            if (connection->t_rxtcur < TCPTV_MIN) {
                connection->t_rxtcur = TCPTV_MIN;
            } else if (connection->t_rxtcur > TCPTV_REXMTMAX) {
                connection->t_rxtcur = TCPTV_REXMTMAX;
            }
        }
        if (header->th_ack == connection->snd_max) {
            connection->t_timer[TCPT_REXMT] = 0;
            need_output = 1;
        } else if (connection->t_timer[TCPT_PERSIST] == 0) {
            connection->t_timer[TCPT_REXMT] = connection->t_rxtcur;
        }
        incr = connection->t_maxseg;
        if (connection->snd_cwnd > connection->snd_ssthresh) {
            incr = TCP_MAX(incr * incr / connection->snd_cwnd, 1);
        }
        connection->snd_cwnd = (cwnd = connection->snd_cwnd + incr) < TCP_MAXWIN
                                   ? cwnd
                                   : TCP_MAXWIN;
        if (acknowledged > TCP_SendWindowCc(connection)) {
            send_count = TCP_SendWindowCc(connection);
            connection->snd_wnd -= send_count;
            connection->snd_wnd -= acknowledged;
            TCP_FreeSendWindow(send_count, connection);
            fin_acknowledged = 1;
        } else {
            TCP_FreeSendWindow(acknowledged, connection);
            connection->snd_wnd -= acknowledged;
            fin_acknowledged = 0;
        }
        connection->snd_una = header->th_ack;
        if (connection->snd_nxt < connection->snd_una) {
            connection->snd_nxt = connection->snd_una;
        }

        switch (connection->state) {
        case TCPS_FIN_WAIT_1:
            if (fin_acknowledged) {
                connection->t_timer[TCPT_2MSL] = tcp_maxidle;
                connection->state = TCPS_FIN_WAIT_2;
            }
            break;
        case TCPS_CLOSING:
            if (fin_acknowledged) {
                connection->state = TCPS_TIME_WAIT;
                if (TCP_SendWindowCc(connection) == 0) {
                    TCP_SendWindowFree(connection);
                }
                if (TCP_RecvWindowCc(connection) == 0) {
                    TCP_RecvWindowFree(connection);
                }
                TCP_CancelAllTimer(connection);
                connection->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
            }
            break;
        case TCPS_LAST_ACK:
            if (fin_acknowledged) {
                API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                            ASR_CLOSED);
                TCP_TcbDelete(connection);
                goto drop;
            }
            break;
        case TCPS_TIME_WAIT:
            connection->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
            goto dropafterack;
        }
    }

step6:
    if ((flags & TH_ACK) && (SEQ_LT(connection->snd_wl1, header->th_seq) ||
                             (connection->snd_wl1 == header->th_seq &&
                              (SEQ_LT(connection->snd_wl2, header->th_ack) ||
                               (connection->snd_wl2 == header->th_ack &&
                                header->th_win > connection->snd_wnd))))) {
        /* @hack redundant: the test of payload_length, snd_wl2 and th_win,
         * whose branch is empty */
        if (payload_length == 0 && connection->snd_wl2 == header->th_ack &&
            header->th_win > connection->snd_wnd) {
        }
        connection->snd_wnd = header->th_win;
        connection->snd_wl1 = header->th_seq;
        connection->snd_wl2 = header->th_ack;
        if (connection->snd_wnd > connection->max_sndwnd) {
            connection->max_sndwnd = connection->snd_wnd;
        }
        need_output = 1;
    }

    if ((flags & TH_URG) && header->th_urp &&
        connection->state < TCPS_TIME_WAIT) {
        if (header->th_urp + (Sint32)TCP_RecvWindowSpace(connection) > SB_MAX) {
            header->th_urp = 0;
            flags &= ~TH_URG;
            goto dodata;
        }
        if (header->th_seq + header->th_urp > connection->rcv_up) {
            API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP,
                        ASR_URGENT);
            connection->rcv_up = header->th_seq + header->th_urp;
        }
    } else if (connection->rcv_nxt != connection->rcv_up) {
        connection->rcv_up = connection->rcv_nxt;
    }

dodata:
    if ((payload_length || (flags & TH_FIN)) &&
        connection->state < TCPS_TIME_WAIT) {
        if (header->th_seq == connection->rcv_nxt &&
            (connection->state == TCPS_ESTABLISHED ||
             (connection->shut & SHUT_DONE))) {
            connection->rcv_nxt += payload_length;
            flags = received_flags & TH_FIN;
            offset = header_length + dropped;
            if ((connection->shut & SHUT_RECV) == 0) {
                TCP_PutRecvWindow(packet, offset, connection);
            }
        } else {
            flags &= ~TH_FIN;
        }
        if (tcpnodelack == 0) {
            connection->flags |= TF_DELACK;
        } else {
            connection->flags |= TF_ACKNOW;
        }
        len = (int)TCP_RecvWindowSize(connection) -
              (int)(connection->rcv_adv - connection->rcv_nxt);
        if (len > connection->max_rcvd) {
            connection->max_rcvd = len;
        }
    } else {
        flags &= ~TH_FIN;
    }

    if (flags & TH_FIN) {
        API_ASRCall(connection - tcb_table, connection->asr, ASR_TCP, ASR_FIN);
        if (connection->state < TCPS_TIME_WAIT) {
            TCP_FinNotify(connection);
            connection->flags |= TF_ACKNOW;
            connection->rcv_nxt++;
        }
        switch (connection->state) {
        case TCPS_SYN_RECEIVED:
        case TCPS_ESTABLISHED:
            connection->state = TCPS_CLOSE_WAIT;
            break;
        case TCPS_FIN_WAIT_1:
            connection->state = TCPS_CLOSING;
            break;
        case TCPS_FIN_WAIT_2:
            connection->state = TCPS_TIME_WAIT;
            if (TCP_SendWindowCc(connection) == 0) {
                TCP_SendWindowFree(connection);
            }
            if (TCP_RecvWindowCc(connection) == 0) {
                TCP_RecvWindowFree(connection);
            }
            TCP_CancelAllTimer(connection);
            connection->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
            break;
        case TCPS_TIME_WAIT:
            connection->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
            break;
        }
    }

    switch (connection->write_end) {
    case TCP_END_CLOSE:
        connection->shut |= SHUT_CLOSED;
        connection->read_request = 0;
        /* fall through */
    case TCP_END_SHUTDOWN:
        connection->shut |= SHUT_DONE;
        TCP_CloseSub(connection - tcb_table);
        connection->write_end = 0;
        return 0;
    case TCP_END_ABORT:
        TCP_Abort(connection - tcb_table);
        goto drop;
    }
    switch (connection->read_end) {
    case TCP_END_CLOSE:
        connection->shut |= SHUT_CLOSED;
        /* fall through */
    case TCP_END_SHUTDOWN:
        connection->shut |= SHUT_DONE;
        connection->read_end = 0;
        if (connection->write_request != 0) {
            break;
        }
        TCP_CloseSub(connection - tcb_table);
        goto drop;
    case TCP_END_ABORT:
        TCP_Abort(connection - tcb_table);
        goto drop;
    }

    if (need_output || (connection->flags & TF_ACKNOW)) {
        TCP_Rawsend(connection);
    }
    goto drop;

dropafterack:
    if (flags & TH_RST) {
        goto drop;
    }
    connection->flags |= TF_ACKNOW;
    TCP_Rawsend(connection);
    goto drop;

dropwithreset:
    if (flags & TH_RST) {
        return 0;
    }
    if (IP_IsBroadcast(dst, 0) == 1) {
        return 0;
    }
    if (flags & TH_ACK) {
        if (TCP_SendDirect(src, header->th_dport, header->th_sport,
                           OS_Htonl(header->th_ack), 0, TH_RST)) {
            return -1;
        }
    } else {
        if (flags & TH_SYN) {
            payload_length++;
        }
        offset = header->th_seq + payload_length;
        if (TCP_SendDirect(src, header->th_dport, header->th_sport, 0,
                           OS_Htonl(offset), TH_RST | TH_ACK)) {
            return -1;
        }
    }

drop:
    return 0;
}

/* the TCP calls the API makes on a socket, an index into tcb_table */
#include <access/ip_types.h>
#include <access/at_types.h>
#include <access/at_api.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>

/* t_maxseg until the peer's SYN offers an MSS */
#define TCP_DEFAULT_MSS 536
/* the size of SOCKOPT_KEEPIDLE's value, a count of seconds */
#define KEEPIDLE_BYTES ((int)sizeof(Uint16))

static int sock_check(int socket);
static void tcb_init(int socket);

static void tcb_clear(char *p)
{
    int n;

    for (n = sizeof(struct tcb); n != 0; n--) {
        *p++ = 0;
    }
}

int TCP_Socket(void)
{
    int index;
    struct tcb *connection;

    for (index = 0; index < max_tcb_entry; index++) {
        connection = &tcb_table[index];
        if (connection->in_use == 0) {
            tcb_clear((char *)connection);
            tcb_init(index);
            return index;
        }
    }
    for (index = 0; index < max_tcb_entry; index++) {
        connection = &tcb_table[index];
        if (connection->state == TCPS_TIME_WAIT &&
            (connection->flags & TF_CLOSED)) {
            TCP_TcbDelete(connection);
            tcb_clear((char *)connection);
            tcb_init(index);
            return index;
        }
    }
    return -1;
}

int TCP_DelTimeWaitTcb(void)
{
    int index;
    struct tcb *connection;

    for (index = 0; index < max_tcb_entry; index++) {
        connection = &tcb_table[index];
        if (connection->state == TCPS_TIME_WAIT &&
            (connection->flags & TF_CLOSED)) {
            TCP_TcbDelete(connection);
            tcb_clear((char *)connection);
            return index;
        }
    }
    return -1;
}

static void tcb_init(int socket)
{
    struct tcb *connection = &tcb_table[socket];

    connection->in_use = 1;
    connection->state = TCPS_CLOSED;
    connection->t_maxseg = TCP_DEFAULT_MSS;
    connection->ip_df = 0;
    connection->flags = 0;
    connection->shut = 0;
    /* t_srtt is 0 until a round trip is measured. a deviation of
     * TCPTV_SRTTDFLT makes TCP_REXMTVAL 2 * TCPTV_SRTTDFLT, t_rxtcur's start */
    connection->t_srtt = 0;
    connection->t_rttvar = TCPTV_SRTTDFLT << TCP_RTTVAR_SHIFT;
    connection->t_rxtcur = 2 * TCPTV_SRTTDFLT;
    /* @hack redundant: the clamp of t_rxtcur to TCPTV_MIN and TCPTV_REXMTMAX,
     * which the timeout above lies within */
    if (connection->t_rxtcur < TCPTV_MIN) {
        connection->t_rxtcur = TCPTV_MIN;
    } else if (connection->t_rxtcur > TCPTV_REXMTMAX) {
        connection->t_rxtcur = TCPTV_REXMTMAX;
    }
    connection->snd_ssthresh = TCP_MAXWIN;
    connection->listener = TCB_NO_LISTENER;
    TCP_CancelAllTimer(connection);
    connection->ip_ttl = IP_DEFAULT_TTL;
    connection->ip_tos = 0;
    connection->ip_optlen = 0;
    connection->ip_options = 0;
    connection->asr = 0;
    connection->route.rt = 0;
}

int TCP_Bind(int socket, Uint16 local_port, Uint32 faddr, Uint16 peer_port)
{
    struct tcb *connection;
    struct tcb *other;
    int index;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if ((connection->t_template.th_sport = local_port) == 0) {
        connection->t_template.th_sport = TCP_GenarateMyPort(connection);
        return 0;
    }
    for (index = 0; index < max_tcb_entry; index++) {
        other = &tcb_table[index];
        if (other->in_use && index != socket &&
            other->t_template.th_sport == local_port &&
            ((faddr == 0 && other->state == TCPS_LISTEN) || other->faddr == 0 ||
             other->faddr == faddr) &&
            ((peer_port == 0 && other->state == TCPS_LISTEN) ||
             other->t_template.th_dport == 0 ||
             other->t_template.th_dport == peer_port)) {
            if (other->state == TCPS_TIME_WAIT && (other->flags & TF_CLOSED)) {
                TCP_TcbDelete(other);
                break;
            }
            return API_DUPSOCK;
        }
    }
    return 0;
}

int TCP_Accept(int socket, Sint32 request, Sint32 asr)
{
    struct tcb *connection;
    struct tcb *child;
    int index;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (connection->state != TCPS_LISTEN) {
        return API_INVALIDHANDLE;
    }
    for (index = 0; index < max_tcb_entry; index++) {
        child = &tcb_table[index];
        if (child->in_use == 0) {
            continue;
        }
        if (child->listener == (Uint16)socket &&
            child->state < TCPS_TIME_WAIT && child->state > TCPS_CLOSED) {
            API_AcceptNotify(socket, request, index, child->faddr,
                             child->t_template.th_sport,
                             child->t_template.th_dport);
            child->asr = asr;
            child->listener = TCB_NO_LISTENER;
            return 0;
        }
    }
    connection->accept_req = request;
    connection->asr = asr;
    return 0;
}

int TCP_AcceptNow(int socket)
{
    tcb_table[socket].listener = TCB_NO_LISTENER;
    return 0;
}

int TCP_AcceptRetryNotify(int socket, Sint32 request, Sint32 asr)
{
    struct tcb *connection = &tcb_table[socket];

    connection->accept_req = request;
    connection->asr = asr;
    return 0;
}

int TCP_AcceptEndNotify(int socket, int close)
{
    struct tcb *connection = &tcb_table[socket];

    connection->accept_req = 0;
    if (close) {
        API_ASRCall(socket, connection->asr, ASR_TCP, ASR_LAST);
        connection->in_use = 0;
    }
    return 0;
}

int TCP_Connect(int socket, Uint32 faddr, Uint16 peer_port)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    connection->faddr = faddr;
    connection->t_template.th_dport = peer_port;
    connection->state = TCPS_SYN_SENT;
    connection->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
    connection->keepidle = 0;
    connection->snd_una = connection->snd_nxt = connection->snd_max =
        connection->snd_up = connection->iss = tcp_iss;
    tcp_iss += TCP_ISSINCR_CONNECTION;
    if (TCP_SendWindowAlloc(connection) != 0) {
        return API_NOBUF;
    }
    if (TCP_RecvWindowAlloc(connection) != 0) {
        return API_NOBUF;
    }
    connection->snd_cwnd = TCP_SendWindowSpace(connection);
    /* @hack redundant: the test of TCP_Rawsend's result, whose branch is
     * empty */
    if (TCP_Rawsend(connection)) {
    }
    return 0;
}

int TCP_Listen(int socket, Uint32 faddr, Uint16 peer_port)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    connection->faddr = faddr;
    connection->t_template.th_dport = peer_port;
    connection->state = TCPS_LISTEN;
    return 0;
}

int TCP_InitSockopt(int socket, Uint8 ttl, Uint8 tos, Uint8 df,
                    Sint16 options_size, void *options, Sint32 asr)
{
    struct tcb *connection = &tcb_table[socket];

    connection->ip_ttl = ttl;
    connection->ip_tos = tos;
    connection->ip_df = df;
    connection->ip_optlen = options_size;
    connection->ip_options = options;
    connection->asr = asr;
    return 0;
}

int TCP_SetSockopt(int socket, int opt, void *value, int value_length)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    switch (opt) {
    case SOCKOPT_KEEPIDLE:
        if (value_length < KEEPIDLE_BYTES) {
            return API_BADOPT;
        }
        connection->t_timer[TCPT_KEEP] = connection->keepidle =
            *(Uint16 *)value << PR_SLOWHZ_SHIFT;
        connection->t_idle = 0;
        break;
    default:
        return API_BADOPT;
    }
    return 0;
}

int TCP_GetSockopt(int socket, int opt, void *value, int *value_length)
{
    struct tcb *connection;
    /* @hack spelling: keepidle, a copy of value, where value cast at the
     * store would do */
    Sint16 *keepidle;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    switch (opt) {
    case SOCKOPT_KEEPIDLE:
        if (*value_length < KEEPIDLE_BYTES) {
            return API_BADOPT;
        }
        keepidle = value;
        *keepidle = connection->keepidle >> PR_SLOWHZ_SHIFT;
        *value_length = KEEPIDLE_BYTES;
        break;
    default:
        return API_BADOPT;
    }
    return 0;
}

int TCP_Close(int socket)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    connection->flags |= TF_CLOSED;
    connection->shut |= SHUT_DONE;
    TCP_CloseSub(socket);
    return 0;
}

int TCP_CloseSub(int socket)
{
    struct tcb *connection = &tcb_table[socket];
    struct tcb *child;
    struct tcb *end;

    switch (connection->state) {
    case TCPS_LISTEN:
        end = &tcb_table[max_tcb_entry];
        for (child = tcb_table; child < end; child++) {
            if (child->in_use == 0) {
                continue;
            }
            if ((Uint16)socket == child->listener) {
                child->flags |= TF_CLOSED;
                TCP_TcbDelete(child);
            }
        }
        /* fall through */
    case TCPS_SYN_RECEIVED:
    case TCPS_CLOSED:
    case TCPS_SYN_SENT:
        connection->state = TCPS_CLOSED;
        TCP_TcbDelete(connection);
        return 0;
    case TCPS_ESTABLISHED:
        connection->state = TCPS_FIN_WAIT_1;
        break;
    case TCPS_CLOSE_WAIT:
        connection->state = TCPS_LAST_ACK;
        break;
    }
    TCP_Rawsend(connection);
    return 0;
}

int TCP_Shutdown(int socket, Uint8 how)
{
    struct tcb *connection;
    /* @hack spelling: mode, a copy of how, where how would do */
    int mode = how;

    if (mode > TCP_SHUT_RDWR || sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (mode == TCP_SHUT_RD) {
        connection->shut |= SHUT_RECV;
        return 0;
    }
    if (mode == TCP_SHUT_RDWR) {
        connection->shut |= SHUT_DONE;
    }
    if (mode == TCP_SHUT_WR) {
        connection->shut |= SHUT_SEND;
    }
    return TCP_CloseSub(socket);
}

int TCP_Abort(int socket)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (connection->state < TCPS_ESTABLISHED) {
        connection->state = TCPS_CLOSED;
    } else {
        /* TCPS_CLOSED's segment carries RST, which resets the peer */
        connection->state = TCPS_CLOSED;
        TCP_Rawsend(connection);
    }
    connection->flags |= TF_CLOSED;
    TCP_TcbDelete(connection);
    return 0;
}

int TCP_ApiGetaddr(int socket, struct sock_addr *addr)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    addr->addr = connection->faddr;
    addr->local_port = connection->t_template.th_sport;
    addr->peer_port = connection->t_template.th_dport;
    return 0;
}

int TCP_SockStat(int socket, struct tcp_stat *stat)
{
    struct tcb *connection;
    struct tcb *child;
    int pending;
    int index;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (connection->state == TCPS_LISTEN) {
        for (index = 0, pending = 0; index < max_tcb_entry; index++) {
            child = &tcb_table[index];
            if (child->in_use == 0) {
                continue;
            }
            if (child->listener == socket && child->state < TCPS_TIME_WAIT &&
                child->state > TCPS_CLOSED) {
                pending++;
            }
        }
        stat->pending = pending;
    } else {
        stat->pending = 0;
    }
    stat->snd_space = TCP_SendWindowSpace(connection);
    stat->rcv_cc = TCP_RecvWindowCc(connection);
    return connection->state;
}

int TCP_Write(int socket, AT_MaxBuffs *buffers, Sint32 request, Uint8 urgent)
{
    struct tcb *connection;
    int error;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (connection->state <= TCPS_LISTEN) {
        return API_BADOPT;
    }
    if (connection->state > TCPS_CLOSE_WAIT && (connection->shut & SHUT_SEND)) {
        return API_SHUTDOWN;
    }
    /* @bug logic: drops a write TCP_PutSendWindow refuses, and returns as if
     * it took it. TCP_PutSendWindow refuses one while a TCP_Writef's bytes are
     * unacknowledged (0x0cadd172) */
    TCP_PutSendWindow(buffers, connection, request);
    if (urgent) {
        connection->snd_up = connection->snd_nxt + buffers->total_len;
    }
    if (connection->state < TCPS_ESTABLISHED) {
        error = 0;
    } else {
        error = TCP_Rawsend(connection);
    }
    /* freeing no bytes copies the write into the send window, if the window has
     * room for it all */
    TCP_FreeSendWindow(0, connection);
    if (error) {
        return -1;
    }
    return 0;
}

int TCP_WriteRetryNotify(int socket, AT_MaxBuffs *buffers, Sint32 request,
                         Uint8 urgent)
{
    struct tcb *connection = &tcb_table[socket];

    if (connection->state > TCPS_CLOSE_WAIT && (connection->shut & SHUT_SEND)) {
        return API_SHUTDOWN;
    }
    TCP_PutSendWindow(buffers, connection, request);
    /* @hack redundant: the test of urgent, whose branch is empty */
    if (urgent) {
    }
    return TCP_SendWindowCc(connection);
}

Sint16 TCP_WriteEndNotify(int socket, Uint8 end)
{
    struct tcb *connection = &tcb_table[socket];

    connection->write_request = 0;
    connection->write_end = end;
    return 0;
}

int TCP_Writef(int socket, const Uint32 *data, Sint16 len, Sint32 request,
               Uint8 urgent)
{
    struct tcb *connection;
    int error;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (connection->state <= TCPS_LISTEN) {
        return API_BADOPT;
    }
    if (connection->state > TCPS_CLOSE_WAIT && (connection->shut & SHUT_SEND)) {
        return API_SHUTDOWN;
    }
    /* @bug logic: drops a write TCP_PutSendfWindow refuses, and returns as if
     * it took it. TCP_PutSendfWindow refuses one while a TCP_Write's bytes are
     * unacknowledged, or while every slot of the send window is in use
     * (0x0cadd2d4) */
    TCP_PutSendfWindow(data, len, connection, request);
    if (urgent) {
        connection->snd_up = connection->snd_nxt + len;
    }
    if (connection->state < TCPS_ESTABLISHED) {
        error = 0;
    } else {
        error = TCP_Rawsend(connection);
    }
    if (error) {
        return -1;
    }
    return 0;
}

int TCP_Read(int socket, AT_Vptr *buffer, Uint32 size, Sint32 request)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    switch (connection->state) {
    case TCPS_ESTABLISHED:
    case TCPS_FIN_WAIT_1:
    case TCPS_FIN_WAIT_2:
    case TCPS_CLOSE_WAIT:
    case TCPS_CLOSING:
    case TCPS_TIME_WAIT:
    case TCPS_CLOSED:
    case TCPS_LAST_ACK:
        if (connection->shut & SHUT_RECV) {
            if (connection->receive_window.type == AT_PTR_TYPE_NONE ||
                TCP_RecvWindowCc(connection) == 0) {
                return API_SHUTDOWN;
            }
        }
        break;
    }
    if (TCP_SetRecvReq(connection, buffer, size, request)) {
        return -1;
    }
    return 0;
}

int TCP_ReadRetryNotify(int socket, AT_Vptr *buffer, Uint32 size,
                        Sint32 request)
{
    TCP_SetRecvReqEnt(&tcb_table[socket], buffer, size, request);
    return 0;
}

Sint16 TCP_ReadEndNotify(int socket, Uint8 end)
{
    tcb_table[socket].read_request = 0;
    tcb_table[socket].read_end = end;
    return 0;
}

int TCP_SetASR(int socket, Sint32 asr)
{
    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    tcb_table[socket].asr = asr;
    return 0;
}

int TCP_Cancel(int socket, Uint16 what)
{
    struct tcb *connection;

    if (sock_check(socket)) {
        return API_INVALIDHANDLE;
    }
    connection = &tcb_table[socket];
    if (what & ~(CANCEL_SEND | CANCEL_RECEIVE | CANCEL_ACCEPT)) {
        return API_BADOPT;
    }
    if (connection->state == TCPS_LISTEN) {
        if (what & CANCEL_ACCEPT) {
            connection->accept_req = 0;
        }
    } else {
        if (what & CANCEL_SEND) {
            connection->write_request = 0;
        }
        if (what & CANCEL_RECEIVE) {
            connection->read_request = 0;
        }
    }
    return 0;
}

static int sock_check(int socket)
{
    if (socket >= max_tcb_entry || socket < 0 ||
        tcb_table[socket].in_use != 1) {
        return API_INVALIDHANDLE;
    }
    return 0;
}

/* the ACCESS TCP/IP library's connection records and the calls between its
 * layers. a struct tcb is one TCP connection: its socket's send and receive
 * windows, and its TCPT_NTIMERS timers */
#ifndef ACCESS_TCP_PRIVATE_H
#define ACCESS_TCP_PRIVATE_H
#include <access/at_types.h>
#include <access/ip_types.h>
#include <sg_xpt.h>
#include <access/stack_globals.h> /* IWYU pragma: export */

#define TCPT_NTIMERS 4
#define TCPT_REXMT 0
#define TCPT_PERSIST 1
#define TCPT_KEEP 2
#define TCPT_2MSL 3
#define TCP_MAXRXTSHIFT 12
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TF_ACKNOW 0x01
#define TF_DELACK 0x02
#define TF_SENTFIN 0x10
#define TF_CLOSED 0x80
#define TCPOPT_EOL 0
#define TCPOPT_NOP 1
#define TCPOPT_MAXSEG 2
#define TCPOLEN_MAXSEG 4
/* the bytes of an option's kind and length, ahead of its value */
#define TCPOPT_VALUE_OFFSET 2
#define IPPROTO_TCP 6
#define IPPROTO_ICMP 1
#define IPPROTO_UDP 17
/* TCP_Drop's err, which it keeps in the tcb's errcode. TCP_ECONNRESET is for
 * a RST that came in, and TCP_ESYNINWINDOW for a SYN in the window. no call
 * passes TCP_EUNREACHABLE, so errcode never holds it */
#define TCP_ECONNRESET 1
#define TCP_ESYNINWINDOW 2
#define TCP_ETIMEDOUT 4
#define TCP_EUNREACHABLE 6
#define SEQ_LT(a, b) ((int)((a) - (b)) < 0)
#define SEQ_GT(a, b) ((int)((a) - (b)) > 0)
#define TCP_MIN(a, b) ((a) < (b) ? (a) : (b))
/* TCP_Timer's ticks a second, the shift that turns seconds into ticks, and
 * a tick's milliseconds. the TCPTV_ times count ticks */
#define PR_SLOWHZ 2
#define PR_SLOWHZ_SHIFT 1
#define TCP_TICK_MS (1000 / PR_SLOWHZ)
#define TCPTV_MSL 60
#define TCPTV_SRTTDFLT 6
#define TCPTV_KEEP_INIT 150
#define TCPTV_MIN 2
#define TCPTV_REXMTMAX 128
#define TCPTV_PERSMIN 10
#define TCPTV_PERSMAX 120
#define TCPTV_KEEP_IDLE 14400
#define TCPTV_KEEPINTVL 150
#define TCPTV_KEEPCNT 8
/* t_srtt holds the smoothed round trip, shifted left by TCP_RTT_SHIFT.
 * t_rttvar holds its mean deviation, shifted left by TCP_RTTVAR_SHIFT.
 * TCP_REXMTVAL is the retransmit timeout they give: the round trip and twice
 * its deviation, in ticks */
#define TCP_RTT_SHIFT 3
#define TCP_RTTVAR_SHIFT 2
#define TCP_REXMTVAL(connection)                                               \
    ((((connection)->t_srtt >> (TCP_RTT_SHIFT - TCP_RTTVAR_SHIFT + 1)) +       \
      (connection)->t_rttvar) >>                                               \
     (TCP_RTTVAR_SHIFT - 1))
#define SHUT_RECV 0x04
#define SHUT_SEND 0x08
#define SHUT_DONE 0x0c
#define SHUT_CLOSED 0x80
/* TCP_Shutdown's how: the side to shut, as BSD's shutdown numbers them */
#define TCP_SHUT_RD 0
#define TCP_SHUT_WR 1
#define TCP_SHUT_RDWR 2
#define TCP_ISSINCR (125 * 1024)
/* tcp_iss's step for each connection TCP_Connect opens or a listening tcb
 * takes in. TCP_Timer steps it by TCP_ISSINCR a second */
#define TCP_ISSINCR_CONNECTION (TCP_ISSINCR / 2)
#define TCP_MAXWIN 65535
#define SB_MAX 0x10000
/* API_ASRCall's protocol: the kind of socket whose ASR hears the event */
#define ASR_TCP 3
#define ASR_UDP 2
#define ASR_IP 1
/* the events a socket's ASR hears */
#define ASR_CONNECTED 1
#define ASR_FIN 2
#define ASR_URGENT 3
#define ASR_CLOSED 4
#define ASR_RESET 10
#define ASR_TIMEOUT 11
#define ASR_NO_ROUTE 12
#define ASR_REFUSED 14
/* the last event a socket's ASR hears, as the socket closes */
#define ASR_LAST 255
#define SEQ_LEQ(a, b) ((int)((a) - (b)) <= 0)
#define TCP_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SOCKOPT_KEEPIDLE 8
enum {
    TCPS_CLOSED,
    TCPS_LISTEN,
    TCPS_SYN_SENT,
    TCPS_SYN_RECEIVED,
    TCPS_ESTABLISHED,
    TCPS_CLOSE_WAIT,
    TCPS_FIN_WAIT_1,
    TCPS_CLOSING,
    TCPS_LAST_ACK,
    TCPS_FIN_WAIT_2,
    TCPS_TIME_WAIT
};

struct tcphdr {
    Uint16 th_sport;
    Uint16 th_dport;
    Uint32 th_seq;
    Uint32 th_ack;
    Uint8 th_off;
    Uint8 th_flags;
    Uint16 th_win;
    Uint16 th_sum;
    Uint16 th_urp;
};
/* th_off holds the header's length in 32-bit words in its high 4 bits.
 * TH_OFF is th_off for a header of that many bytes. TH_OFF_BYTES is the
 * length th_off gives while its low 4 bits are 0 */
#define TH_OFF(bytes) (((bytes) / 4) << 4)
#define TH_OFF_BYTES(off) ((off) >> 2)

struct tcb {
    AT_Vptr send_window;
    Sint32 send_count;
    Uint32 send_head;
    Uint32 send_tail;
    AT_MaxBuffs pending_send;
    AT_Vptr receive_window;
    Uint32 receive_count;
    Uint32 receive_head;
    Uint32 receive_tail;
    AT_Vptr receive_buffer;
    Uint32 receive_size;
    Sint32 receive_done;
    Sint32 write_request;
    Sint32 read_request;
    unsigned int external_send : 1;
    unsigned int receive_disabled : 1;
    Uint32 snd_una;
    Uint32 snd_nxt;
    Uint32 snd_up;
    Uint32 snd_wl1;
    Uint32 snd_wl2;
    Uint32 iss;
    Uint16 snd_wnd;
    Uint16 rcv_wnd;
    Uint32 rcv_nxt;
    Uint32 rcv_up;
    Uint32 irs;
    Uint32 rcv_adv;
    Uint32 snd_max;
    Uint16 snd_cwnd;
    Uint16 snd_ssthresh;
    Sint16 t_idle;
    Sint16 t_rtt;
    Uint32 t_rtseq;
    Sint16 t_srtt;
    Sint16 t_rttvar;
    Uint16 max_rcvd;
    Uint16 max_sndwnd;
    Sint16 t_rxtcur;
    Sint16 t_rxtshift;
    Sint16 t_timer[TCPT_NTIMERS];
    Uint16 t_maxseg;
    Uint8 pad_12e[2];
    RtCache route;
    Uint16 in_use;
    Uint16 state;
    Uint16 listener;
    Uint8 shut;
    Uint8 flags;
    Uint8 t_force;
    Uint8 t_dupacks;
    Uint8 errcode;
    Uint8 pad_143;
    struct tcphdr t_template;
    Uint32 faddr;
    Sint16 ip_optlen;
    Uint8 pad_15e[2];
    void *ip_options;
    Sint32 accept_req;
    Sint32 asr;
    Sint16 keepidle;
    Uint8 ip_tos;
    Uint8 ip_df;
    Uint8 ip_ttl;
    Uint8 read_end;
    Uint8 write_end;
    Uint8 pad_173;
};
/* listener holds the index of the listening tcb a connection came in through,
 * or TCB_NO_LISTENER */
#define TCB_NO_LISTENER 0xffff

struct tcp_stat {
    Uint16 pending;
    Uint16 snd_space;
    Uint16 rcv_cc;
};

extern struct tcb *last_tcbp;
extern Sint32 tcp_maxidle;
extern Uint32 tcp_iss;
extern Uint8 tcp_initopt[4];
extern Uint16 win_size;
extern Sint32 dropsocket;
extern Uint32 tcb_struct_size;
extern Sint32 tcpnodelack;
extern Uint32 tcprexmtthresh;

Sint32 API_AcceptNotify(Sint32 socket, Sint32 request, Sint32 child,
                        Uint32 peer, Uint16 local_port, Uint16 peer_port);
int API_ASRCall(int socket, Sint32 asr, int protocol, int event);
Sint32 DL_CheckSend(void);

/* a page of the buffer pool, which MP_Alloc cuts into up to eight fragments. a
 * fragment's owner is -1 while it is free and 0 while it holds no bytes.
 * otherwise it is the kind MP_Alloc gave it. a pool pointer is an AT_Vptr of
 * AT_PTR_TYPE_POOL. its top 8 bits index the page, and the next 8 the fragment.
 * its low 16 bits are a byte offset into the fragment */
struct pool_fragment {
    Sint16 owner;
    Uint16 offset;
    Uint16 size;
};
struct memory_pool {
    Uint8 *base;
    Sint32 unowned_fragments;
    Sint32 free_size;
    struct pool_fragment fragments[8];
};
/* a pool pointer's page, its fragment and its byte offset */
#define MP_PAGE_MASK 0xff000000
#define MP_PAGE_SHIFT 24
#define MP_FRAGMENT_MASK 0x00ff0000
#define MP_FRAGMENT_SHIFT 16
#define MP_OFFSET_MASK 0x0000ffff
/* TcpTaskCount is 1 while the TCP/IP daemon runs. setting TcpTaskTerminateReq
 * stops the daemon. OS_Create yields a frame at a time until the count reaches
 * 1. OS_Destroy yields until it falls to 0 */
extern volatile Sint32 TcpTaskCount;
extern volatile Sint32 TcpTaskTerminateReq;

Sint32 OS_Init(void);
AT_MaxBuffs *OS_AllocBuffs(void);
void OS_FreeBuffs(AT_MaxBuffs *buffs);
Uint32 OS_Htonl(Uint32 value);
Uint16 OS_Htons(Uint16 value);
void OS_SetVptrExternal(AT_Vptr *pointer, void *address);
void *OS_GetVptrLocal(AT_Vptr *pointer);
/* seed plus the bytes of buffs's buffers as big-endian 16-bit words, without
 * folding the carries. the words run on across the buffers. an odd byte at the
 * end is a word's high byte. reverse 1 takes the buffers from the last, the
 * wire's order for a list handed to IP. any other reverse takes them from
 * buffers[0], the wire's order for a received list */
Uint32 OS_ChksumBuffs(Uint32 seed, AT_MaxBuffs *buffs, Sint32 reverse);
Sint32 OS_FreeMempool(AT_Vptr *pointer);
/* add count big-endian 16-bit words at address to *seed, without folding the
 * carries */
void sum_cul(Uint32 *seed, void *address, Sint16 count);
void OS_Create(void);
void OS_Destroy(void);
void OS_Yield(void);
void OS_CriticalLock(void);
void OS_CriticalUnLock(void);
void OS_Wait(Sint32 delay);
void OS_RcvTskWakeup(void);
void OS_Pause(void);
Sint32 TIMER_GetCounter(void);
void OS_Vmemcpy(AT_Vptr *destination, AT_Vptr *source, Uint16 size);
void OS_SetVptrLocal(AT_Vptr *pointer, void *address);
void OS_IncVptr(AT_Vptr *pointer, Sint32 size);
extern Uint32 mcb_page_size;
extern Uint32 mcb_struct_size;
extern Uint8 *memplbuf;
/* the owners MP_Alloc takes: a TCP socket's window, an IP fragment, and the
 * ARP table's hold */
#define MP_TCP_WINDOW 1
#define MP_IP_FRAGMENT 2
#define MP_ARP_HOLD 3
Sint32 MP_Init(void);
Sint32 MP_Alloc(AT_Vptr *pointer, Sint32 size, Sint32 kind);
Sint32 MP_Free(AT_Vptr *pointer);

/* finish a read on socket. result is the bytes read or a negative error, held
 * in a Uint32. it is 0 once the peer has closed. a notify function in request
 * hears a 0 as API_RECVFIN. with request 0, the call keeps result and available
 * for a reader that waits */
Sint32 API_RecvNotify(Sint32 socket, Sint32 request, Uint32 result,
                      Sint32 available);
Sint32 API_SendNotify(Sint32 socket, Sint32 request, Sint32 result);
Sint32 API_SendfNotify(Sint32 socket, Sint32 request, Sint32 result,
                       AT_Vptr *buffer);
int TCP_PutSendWindow(AT_MaxBuffs *buffers, struct tcb *connection,
                      Sint32 request);
int TCP_PutSendfWindow(const Uint32 *address, Sint16 len,
                       struct tcb *connection, Sint32 request);
int TCP_GetSendWindow(int offset, int len, AT_MaxBuffs *buffs,
                      struct tcb *connection);
int TCP_FreeSendWindow(int len, struct tcb *connection);
int TCP_PutRecvWindow(AT_MaxBuffs *bufs, Uint16 offset, struct tcb *connection);
int TCP_FinNotify(struct tcb *connection);
int TCP_SetRecvReq(struct tcb *connection, AT_Vptr *buffer, Uint32 size,
                   Sint32 request);
Sint16 TCP_SetRecvReqEnt(struct tcb *connection, AT_Vptr *buffer, Uint32 size,
                         Sint32 request);
Uint32 TCP_SendWindowSpace(struct tcb *connection);
Uint32 TCP_RecvWindowSpace(struct tcb *connection);
int TCP_SendWindowCc(struct tcb *connection);
int TCP_RecvWindowCc(struct tcb *connection);
Uint32 TCP_RecvWindowSize(struct tcb *connection);
int TCP_SendWindowAlloc(struct tcb *connection);
int TCP_RecvWindowAlloc(struct tcb *connection);
int TCP_SendWindowFree(struct tcb *connection);
int TCP_RecvWindowFree(struct tcb *connection);
int TCP_Init(void);
int TCP_Dooptions(struct tcb *connection, Uint8 *options, int length,
                  struct tcphdr *header);
struct tcb *TCP_ComeSynListen(struct tcb *listener, const Uint16 *peer_port,
                              Uint32 faddr);
Uint16 TCP_Mss(struct tcb *connection);
int TCP_TcbDelete(struct tcb *connection);
Uint16 TCP_GenarateMyPort(struct tcb *connection);
int TCP_SendDirect(Uint32 dst, Uint16 sport, Uint16 dport, Uint32 seq,
                   Uint32 ack, Uint8 flags);
int TCP_Rawsend(struct tcb *connection);
void TCP_CancelAllTimer(struct tcb *connection);
void TCP_Timer(void);
int TCP_TimerExpire(struct tcb *connection, int timer);
int TCP_GetErrcode(int socket);
int TCP_Drop(struct tcb *connection, int err);
Sint32 TCP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_length, Uint32 src,
                Uint32 dst);
int TCP_Socket(void);
int TCP_DelTimeWaitTcb(void);
int TCP_Bind(int socket, Uint16 local_port, Uint32 faddr, Uint16 peer_port);
int TCP_Accept(int socket, Sint32 request, Sint32 asr);
int TCP_AcceptNow(int socket);
int TCP_AcceptRetryNotify(int socket, Sint32 request, Sint32 asr);
int TCP_AcceptEndNotify(int socket, int close);
int TCP_Connect(int socket, Uint32 faddr, Uint16 peer_port);
int TCP_Listen(int socket, Uint32 faddr, Uint16 peer_port);
int TCP_InitSockopt(int socket, Uint8 ttl, Uint8 tos, Uint8 df,
                    Sint16 options_size, void *options, Sint32 asr);
int TCP_SetSockopt(int socket, int opt, void *value, int value_length);
int TCP_GetSockopt(int socket, int opt, void *value, int *value_length);
int TCP_Close(int socket);
int TCP_CloseSub(int socket);
int TCP_Shutdown(int socket, Uint8 how);
int TCP_Abort(int socket);
int TCP_ApiGetaddr(int socket, struct sock_addr *addr);
int TCP_SockStat(int socket, struct tcp_stat *stat);
int TCP_Write(int socket, AT_MaxBuffs *buffers, Sint32 request, Uint8 urgent);
int TCP_WriteRetryNotify(int socket, AT_MaxBuffs *buffers, Sint32 request,
                         Uint8 urgent);
Sint16 TCP_WriteEndNotify(int socket, Uint8 end);
/* the ends TCP_WriteEndNotify and TCP_ReadEndNotify keep for TCP_Recv. it
 * closes the socket, shuts both its sides, or aborts it. 0 asks for none */
#define TCP_END_CLOSE 1
#define TCP_END_SHUTDOWN 2
#define TCP_END_ABORT 3
int TCP_Writef(int socket, const Uint32 *data, Sint16 len, Sint32 request,
               Uint8 urgent);
int TCP_Read(int socket, AT_Vptr *buffer, Uint32 size, Sint32 request);
int TCP_ReadRetryNotify(int socket, AT_Vptr *buffer, Uint32 size,
                        Sint32 request);
Sint16 TCP_ReadEndNotify(int socket, Uint8 end);
int TCP_SetASR(int socket, Sint32 asr);
int TCP_Cancel(int socket, Uint16 what);
#endif

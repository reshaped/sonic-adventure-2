/* AT_apiCall, which runs a request's handler, and the TCP/IP task, which
 * takes in frames and runs the timers */
#include <access/api_handlers.h>
#include <access/at_api.h>
#include <access/ip_private.h>
#include <access/mib.h>
#include <access/packet_queue.h>
#include <access/pktdrv.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include "../network/daemons.h"

/* a request's code is REQUEST_CODES, under REQUEST_CODES_MASK, plus its
 * handler's index in commands, under REQUEST_INDEX_MASK. AT_apiCall's test
 * admits an index below REQUEST_INDEXES */
#define REQUEST_CODES 0x4100
#define REQUEST_CODES_MASK 0xff00
#define REQUEST_INDEX_MASK 0x00ff
#define REQUEST_INDEXES 64
/* the milliseconds between a timer's runs, as TIMER_GetCounter counts them */
#define IP_TIMER_MS 1000
#define TCP_TIMER_MS TCP_TICK_MS
#define ARP_TIMER_MS 60000

typedef Sint32 (*dispatch_handler)(void *input, void *output);

static Sint32 DSP_CallGetConfig(void *input, void *output)
{
    AT_configptr *reply = output;
    reply->config_ptr = ConfigBlock;
    return 0;
}

static Sint32 DSP_API_ErrorProcess(void *input, void *output)
{
    return 0;
}

/* @hack spelling: result, where returning service's value would do */
#define FORWARD(name, service)                                                 \
    static Sint32 name(void *input, void *output)                              \
    {                                                                          \
        Sint32 result = service(input);                                        \
        return result;                                                         \
    }

FORWARD(DSP_CallSetASR, API_SetASR)

static Sint32 DSP_TerminateProcess(void *input, void *output)
{
    return 0;
}

static Sint32 DSP_GetMibVarProcess(void *input, void *output)
{
    void **reply = output;
    *reply = mib_addrs.first;
    return 0;
}

FORWARD(DSP_CallIfConfigUp, API_IfConfigUp)
FORWARD(DSP_CallIfConfigDown, API_IfConfigDown)
FORWARD(DSP_API_SetSockopt, API_SetSockopt)
FORWARD(DSP_API_GetSockopt, API_GetSockopt)
FORWARD(DSP_CallTCP_Open, API_TCP_Open)
FORWARD(DSP_CallTCP_POpen, API_TCP_POpen)

static Sint32 DSP_CallTCP_Accept(void *input, void *output)
{
    Uint32 address;
    Uint16 local_port;
    Uint16 peer_port;
    Sint32 result;
    AT_getaddr_r *reply;

    /* @bug uninitialized: reply takes the address and ports as the stack
     * left them where API_TCP_Accept fails or leaves the accept pending
     * (0x0cad4ebc) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the reply's address and ports are
     * 0 where the accept leaves them unset */
    address = 0;
    local_port = 0;
    peer_port = 0;
#endif
    result = API_TCP_Accept(input, &address, &local_port, &peer_port);
    if (output) {
        reply = output;
        reply->ipaddr.u32 = address;
        reply->myport.u16 = local_port;
        reply->yourport.u16 = peer_port;
    }
    return result;
}

FORWARD(DSP_CallTCP_Close, API_TCP_Close)
FORWARD(DSP_CallTCP_Shutdown, API_TCP_Shutdown)
FORWARD(DSP_CallTCP_Abort, API_TCP_Abort)

static Sint32 DSP_CallTCP_GetAddr(void *input, void *output)
{
    AT_getaddr_r *address = output;
    /* @hack spelling: result, where returning API_TCP_GetAddr's value would do
     */
    Sint32 result =
        API_TCP_GetAddr(input, &address->ipaddr.u32, &address->myport.u16,
                        &address->yourport.u16);
    return result;
}

static Sint32 DSP_CallTCP_Stat(void *input, void *output)
{
    AT_stat_r *stats = output;
    /* @hack spelling: result, where returning API_TCP_Stat's value would do */
    Sint32 result = API_TCP_Stat(input, &stats->stat, &stats->reserve,
                                 &stats->sendwin, &stats->recvwin);
    return result;
}

FORWARD(DSP_CallTCP_Send, API_TCP_Send)

static Sint32 DSP_CallTCP_Receive(void *input, void *output)
{
    Sint16 length;
    /* @hack spelling: result, where returning API_TCP_Receive's value would do
     */
    Sint32 result = API_TCP_Receive(input, &length);
    return result;
}

FORWARD(DSP_CallTCP_Cancel, API_TCP_Cancel)
FORWARD(DSP_CallTCP_Sendf, API_TCP_Sendf)

static Sint32 DSP_CallTCP_Receivef(void *input, void *output)
{
    Sint16 length;
    /* @hack spelling: result, where returning API_TCP_Receivef's value would do
     */
    Sint32 result = API_TCP_Receivef(input, &length);
    return result;
}

FORWARD(DSP_CallUDP_Open, API_UDP_Open)
FORWARD(DSP_CallUDP_Send, API_UDP_Send)
FORWARD(DSP_CallIP_Open, API_IP_Open)
FORWARD(DSP_CallIP_Send, API_IP_Send)
FORWARD(DSP_CallRouteAdd, API_RouteAdd)
FORWARD(DSP_CallRouteDel, API_RouteDel)
FORWARD(DSP_CallRouteList, API_RouteList)
FORWARD(DSP_CallArpAdd, API_ArpAdd)
FORWARD(DSP_CallArpDel, API_ArpDel)
FORWARD(DSP_CallArpList, API_ArpList)

/* the handler of each code from REQUEST_CODES up. DSP_API_ErrorProcess
 * serves each code that names no request */
static const dispatch_handler commands[] = {
    /* 0x4100 */
    DSP_API_ErrorProcess,
    DSP_CallGetConfig,
    DSP_CallSetASR,
    DSP_TerminateProcess,
    DSP_GetMibVarProcess,
    DSP_CallIfConfigUp,
    DSP_CallIfConfigDown,
    DSP_API_ErrorProcess,
    /* 0x4108 */
    DSP_API_SetSockopt,
    DSP_API_GetSockopt,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    /* 0x4110 */
    DSP_CallTCP_Open,
    DSP_CallTCP_POpen,
    DSP_CallTCP_Accept,
    DSP_CallTCP_Close,
    DSP_CallTCP_Shutdown,
    DSP_CallTCP_Abort,
    DSP_CallTCP_GetAddr,
    DSP_CallTCP_Stat,
    /* 0x4118 */
    DSP_CallTCP_Send,
    DSP_CallTCP_Receive,
    DSP_CallTCP_Cancel,
    DSP_CallTCP_Sendf,
    DSP_CallTCP_Receivef,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    /* 0x4120 */
    DSP_CallUDP_Open,
    DSP_CallUDP_Send,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_CallIP_Open,
    DSP_CallIP_Send,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    /* 0x4128 */
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    DSP_API_ErrorProcess,
    /* 0x4130 */
    DSP_CallRouteAdd,
    DSP_CallRouteDel,
    DSP_CallRouteList,
    DSP_API_ErrorProcess,
    DSP_CallArpAdd,
    DSP_CallArpDel,
    DSP_CallArpList,
    DSP_API_ErrorProcess,
};

Sint16 AT_apiCall(Uint16 code, void *input, void *output)
{
    Uint16 index;
    Sint16 result;
    OS_CriticalLock();
    if ((code & REQUEST_CODES_MASK) != REQUEST_CODES) {
        OS_CriticalUnLock();
        return API_PENDING;
    }
    index = code & REQUEST_INDEX_MASK;
    /* @bug memory: calls through a word past commands' 56 handlers for a code
     * from 0x4138 to 0x413f, which the test lets through (0x0cad50ae,
     * 0x0cad50ca) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it refuses an index past
     * commands' last handler */
    if (index >= sizeof(commands) / sizeof(commands[0])) {
#else
    if (index >= REQUEST_INDEXES) {
#endif
        OS_CriticalUnLock();
        return API_PENDING;
    }
#ifdef NONMATCHING
    /* this side refuses a null input to each handler that reads one, and a null
     * output to those that write one: DSP_CallGetConfig, DSP_GetMibVarProcess,
     * and TCP_GETADDR's and TCP_STAT's handlers */
    if ((!input && commands[index] != DSP_API_ErrorProcess &&
         commands[index] != DSP_TerminateProcess && index != 1 && index != 4) ||
        (!output && (index == 1 || index == 4 || code == TCP_GETADDR ||
                     code == TCP_STAT))) {
        OS_CriticalUnLock();
        return API_GENERAL;
    }
#endif
    result = commands[index](input, output);
    OS_CriticalUnLock();
    return result;
}

/* the tables INI_AllocateBuffer hands the stack */
static struct tcb tcp_connections[AT_TCP_SOCKETS];
static struct ucb udp_sockets[AT_UDP_SOCKETS];
static struct net_icb ip_sockets[AT_IP_SOCKETS];
static struct memory_pool pools[AT_MEMORY_POOLS];
static Uint8 stream_buffers[AT_MEMORY_POOLS][AT_POOL_BYTES];
/* when each timer last ran, as TIMER_GetCounter counts */
static Uint32 last_arp_update, last_tcp_update, last_ip_update;

static void INI_AllocateBuffer(void)
{
    tcb_table = tcp_connections;
    ucb_table = udp_sockets;
    icb_table = ip_sockets;
    mcb_table = pools;
    _streambuf = stream_buffers[0];
    max_tcb_entry = AT_TCP_SOCKETS;
    max_ucb_entry = AT_UDP_SOCKETS;
    max_icb_entry = AT_IP_SOCKETS;
    max_mcb_entry = AT_MEMORY_POOLS;
}

/* @hack type: TcpTaskCount and TcpTaskTerminateReq under second names
 * without their volatile, which symbols.txt places at the two, where their
 * own names would do */
#ifdef NONMATCHING
/* this side reads the two by their own volatile names, the plain form the hack
 * tag above gives. the second names are symbols.txt's alone */
#define TASK_COUNT TcpTaskCount
#define TERMINATE_REQUEST TcpTaskTerminateReq
#else
extern Sint32 tcp_dispatch_task_count;
extern Sint32 tcp_dispatch_terminate_request;
#define TASK_COUNT tcp_dispatch_task_count
#define TERMINATE_REQUEST tcp_dispatch_terminate_request
#endif

void TcpIntervalTimerTaskFunc(void)
{
    struct queued_frame *slot;
    Uint32 now;
    TASK_COUNT++;
    OS_CriticalLock();
    now = TIMER_GetCounter();
    last_arp_update = now;
    last_tcp_update = now;
    last_ip_update = now;
    tcp_que_init(&tcp_que_tbl);
    OS_CriticalUnLock();
    while (!TERMINATE_REQUEST) {
        OS_Pause();
        OS_CriticalLock();
        while (!tcp_que_empty(&tcp_que_tbl)) {
            slot = tcp_que_rptr(&tcp_que_tbl);
            if (slot->ready == 1) {
                DL_Recv(slot->frame, slot->length, slot->interface_number);
            }
            tcp_que_release(&tcp_que_tbl);
        }
        OS_CriticalUnLock();
        now = TIMER_GetCounter();
        if (now - last_ip_update >= IP_TIMER_MS) {
            last_ip_update = now;
            OS_CriticalLock();
            IP_Timer();
            OS_CriticalUnLock();
        }
        if (now - last_tcp_update >= TCP_TIMER_MS) {
            last_tcp_update = now;
            OS_CriticalLock();
            TCP_Timer();
            OS_CriticalUnLock();
        }
        if (now - last_arp_update >= ARP_TIMER_MS) {
            last_arp_update = now;
            OS_CriticalLock();
            ARP_UpdateTable();
            OS_CriticalUnLock();
        }
    }
    TASK_COUNT--;
    for (;;) {
        OS_Pause();
    }
}

Sint16 AT_init(void *argument)
{
    INI_AllocateBuffer();
    OS_Create();
    initialize_all(0);
    return 0;
}

Sint16 AT_disp(void *argument)
{
    pd_ftp_terminate();
    OS_Destroy();
    return 0;
}

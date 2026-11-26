#include <access/at_api.h>
#include <access/aveppp.h>
#include <access/avetcp.h>
#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include <string.h>
#include "ppp_modem.h"
#include "ppp_system.h"

static char *external_tx, *external_rx;
static int external_tx_size, external_rx_size;
/* the word AvetcpInitialize passes to AT_init, which never reads it */
static Uint32 tcp_init_argument = 0x01204e9d;

#pragma section ATA
static char empty_argument[1];
struct startup_args {
    char *argv[1];
};
static struct startup_args startup_args = {{empty_argument}};
static char empty_argument[1] = {0};
#pragma section

int ppp_modem_buffer_size(int query)
{
    MDM handle = ppp_modem_port[0];
    int size;
    switch (query) {
    case PPP_RX_AVAILABLE:
        size = mdmRxAvailSize(handle);
        break;
    case PPP_RX_FREE:
        size = mdmRxFreeSize(handle);
        break;
    case PPP_TX_AVAILABLE:
        size = mdmTxAvailSize(handle);
        break;
    case PPP_TX_FREE:
        size = mdmTxFreeSzie(handle);
        break;
    default:
        size = -1;
        break;
    }
    return size;
}
int ppp_set_modem_device(MD_DEVICE device)
{
    ppp_modem_device = device;
    return 0;
}
short AvepppInitialize2(char *tx, char *rx, int tx_size, int rx_size)
{
    struct startup_args args;
    args = startup_args;
#ifdef NONMATCHING
    /* this side empties the script by storing its NUL, the plain form the hack
     * tag below gives */
    store_login_script3[0] = '\0';
#else
    /* @hack spelling: _builtin_strcpy, where strcpy would do */
    _builtin_strcpy(store_login_script3, empty_argument);
#endif
    PPP_ConnectionStatus = 0;
    PPP_OpenRequest = 0;
    PPP_CloseRequest = 0;
    PPP_Executing = 0;
    PPP_LastError = 0;
    PPP_ComUsing = 0;
    ppp_ini_recv_buff();
    PP_main(1, args.argv);
    PP_OS_Initialize();
    PP_ParamMyIp = 0;
    PP_ParamHisIp = 0;
    PP_ParamMyDns1 = -1;
    PP_ParamMyDns2 = -1;
    PP_ParamHisDns1 = -1;
    PP_ParamHisDns2 = -1;
    external_tx = tx;
    external_rx = rx;
    external_tx_size = tx_size;
    external_rx_size = rx_size;
    /* the matching side passes a negative size on to mdmInit */
#ifdef NONMATCHING
    /* this side takes the library's own buffers for a negative size */
    if (external_tx && external_rx && external_tx_size > 0 &&
        external_rx_size > 0) {
#else
    if (external_tx && external_rx && external_tx_size && external_rx_size) {
#endif
        mdmInit(external_tx, external_rx, external_tx_size, external_rx_size);
    } else {
        mdmInit(ppp_tx_storage, ppp_rx_storage, sizeof(ppp_tx_storage),
                sizeof(ppp_rx_storage));
    }
    return 0;
}
short AvepppInitialize(void)
{
    return AvepppInitialize2(NULL, NULL, 0, 0);
}
void AvepppTerminate(void)
{
    PP_OS_Terminate();
}
void AvepppSetDisconnectScript(char *script)
{
#ifdef NONMATCHING
    /* this side copies at most store_login_script3's room, and ends it with a
     * NUL */
    size_t length = 0;
    while (length < sizeof(store_login_script3) - 1 && script[length]) {
        length++;
    }
    memcpy(store_login_script3, script, length);
    store_login_script3[length] = '\0';
#else
    _builtin_strcpy(store_login_script3, script);
#endif
}
short AvepppOpen(const AvepppOpenParam *param)
{
    return pppcall(PPPCALL_OPEN, param, NULL);
}
short AvepppClose(short handle)
{
    return pppcall(PPPCALL_CLOSE, NULL, NULL);
}
short AvepppGetStatus(AvepppStatus *status)
{
    return pppcall(PPPCALL_STATUS, NULL, status);
}
int AvetcpInitialize(void)
{
    /* @hack redundant: init's reserved and reserved2 around argument, where a
     * local of its own would do */
    struct {
        Sint16 reserved;
        void *argument;
        Sint32 reserved2;
    } init;
    memcpy(&init.argument, &tcp_init_argument, sizeof(tcp_init_argument));
    AT_init(init.argument);
    return 0;
}
Sint16 AvetcpTerminate(void)
{
    return AT_disp(NULL);
}
Sint16 AvetcpOpen(Uint32 address, Uint16 remote_port, Uint16 local_port)
{
    AT_openparam request;
    request.youraddr.u32 = address;
    request.yourport.u16 = remote_port;
    request.myport.u16 = local_port;
    /* the matching side leaves reserve1 unset. API_TCP_Open never reads it */
#ifdef NONMATCHING
    /* this side clears it, so no field of the request is unset */
    request.reserve1 = 0;
#endif
    request.ttl = 0;
    request.svctype = 0;
    request.df_flag = 0;
    request.reserve2 = 0;
    request.reserve3 = NULL;
    request.asraddr = NULL;
    return AT_apiCall(TCP_OPEN, &request, NULL);
}
Sint16 AvetcpClose(Sint16 handle)
{
    AT_nhparam request;
    request.nh = handle;
    return AT_apiCall(TCP_CLOSE, &request, NULL);
}
Sint16 AvetcpAbort(Sint16 handle)
{
    AT_nhparam request;
    request.nh = handle;
    return AT_apiCall(TCP_ABORT, &request, NULL);
}
Sint16 AvetcpStat(Sint16 handle, AT_stat_r *status)
{
    AT_nhparam request;
    request.nh = handle;
    return AT_apiCall(TCP_STAT, &request, status);
}
Sint16 AvetcpSend(Sint16 handle, Uint8 *buffer, Sint16 count)
{
    AT_sendparam request;
    request.nh = handle;
    request.notifyaddr = NULL;
    request.reserve = 0;
    request.bufnum = 1;
    request.buffers[0].len = count;
    request.buffers[0].buf = buffer;
    return AT_apiCall(TCP_SEND, &request, NULL);
}
Sint16 AvetcpReceive(Sint16 handle, Uint8 *buffer, Sint16 count)
{
    AT_rcvparam request;
    request.nh = handle;
    request.len = count;
    request.buf = buffer;
    request.notifyaddr = NULL;
    return AT_apiCall(TCP_RECEIVE, &request, NULL);
}

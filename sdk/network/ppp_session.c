#include <access/aveppp.h>
#include <machine.h>
#include <sg_xpt.h>
#include <string.h>

#include "ppp_fsm.h"
#include "ppp_link.h"
#ifdef DREAMCAST
#else
#include "ppp_modem.h"
#endif
#include "ppp_route.h"
#include "ppp_script.h"
#include "ppp_system.h"
long PPP_ConnectionStatus = 0;
PPP_SHARED int PPP_OpenRequest = 0;
PPP_SHARED int PPP_CloseRequest = 0;
PPP_SHARED int PPP_Executing = 0;
long PPP_LastError = 0;
long PPP_ComUsing = 0;
static long connect_started = 0;
static Bool interface_up = 0;

#ifdef DREAMCAST
static Uint8 pull_word[4], pull_byte;
#endif
static Uint8 peer_address[4];
static AvepppSerialParam saved_serial;
static AvepppTelephoneParam saved_telephone;
static char saved_outside_number[64];
static AvepppConnectionParam saved_link;
static AvepppOpenParam saved_request;
static char connect_script[512];
static char login_script[1024];
char store_login_script3[512];
static char telephone_number[64], user_name[64], password[64];
long PP_ParamMyIp, PP_ParamHisIp, PP_ParamMyDns1, PP_ParamMyDns2,
    PP_ParamHisDns1, PP_ParamHisDns2;
#ifdef DREAMCAST
/* SHC places an uninitialized variable where it first meets its name.
 * ppp_modem.h names store_login_script3 and the PP_Param addresses, so it
 * follows them */
#include "ppp_modem.h"
#endif

/* the ACCM the link asks for, LCP's map of the control characters the peer
 * escapes: XON and XOFF */
#define ACCM_XON_XOFF (1UL << 0x11 | 1UL << 0x13)
/* 255.255.255.0, its first byte lowest, as local_ip holds an address */
#define SUBNET_MASK 0x00ffffff

struct session_counters {
    long sent_bytes, sent_packets;
    short ipcp_sent, send_status, receive_status;
    long received_bytes, received_packets;
    short ipcp_received, unknown, discarded, overflow, bad_frames,
        allocation_errors;
};
static int request_open(const AvepppOpenParam *parameters);
static int request_close(void);
static int read_status(AvepppStatus *status);
static int read_counters(struct session_counters *counters);

int pppcall(int command, const AvepppOpenParam *parameters, void *result)
{
    int status;
    switch (command) {
    case PPPCALL_OPEN:
        status = request_open(parameters);
        break;
    case PPPCALL_CLOSE:
        status = request_close();
        break;
    case PPPCALL_STATUS:
        PP_OS_CriticalLock();
        status = read_status(result);
        PP_OS_CriticalUnLock();
        break;
    case PPPCALL_COUNTERS:
        PP_OS_CriticalLock();
        status = read_counters(result);
        PP_OS_CriticalUnLock();
        break;
    default:
        status = -99;
        break;
    }
    return status;
}
#ifdef NONMATCHING
static char signup_name[64];

/* whether text is a string that fits in capacity bytes with its NUL */
static int fits_string(const char *text, unsigned int capacity)
{
    unsigned int i;
    if (!text) {
        return 0;
    }
    for (i = 0; i < capacity; i++) {
        if (!text[i]) {
            return 1;
        }
    }
    return 0;
}
#define COPY_STRING(destination, source) strcpy((destination), (source))
#else
#define COPY_STRING(destination, source)                                       \
    _builtin_strcpy((destination), (source))
#endif
static int request_open(const AvepppOpenParam *parameters)
{
    const AvepppOpenParam *request;
#ifdef NONMATCHING
    /* this side refuses a request with a part missing, and any string too long
     * for the buffer it is saved in */
    if (!parameters || !parameters->serial || !parameters->telephone ||
        !parameters->pppconection ||
        !fits_string(parameters->telephone->outside_number,
                     sizeof(saved_outside_number)) ||
        !fits_string(parameters->login_script1, sizeof(connect_script)) ||
        !fits_string(parameters->login_script2, sizeof(login_script)) ||
        !fits_string(parameters->telephone_number, sizeof(telephone_number)) ||
        !fits_string(parameters->login, sizeof(user_name)) ||
        !fits_string(parameters->password, sizeof(password)) ||
        (parameters->signup &&
         !fits_string(parameters->signup, sizeof(signup_name)))) {
        return -1;
    }
#endif
    PP_OS_CriticalLock();
    if (!PPP_Executing) {
        PPP_Executing = 1;
        PPP_LastError = AvepppErrorNon;
        request = parameters;
        saved_serial = *request->serial;
        saved_telephone = *request->telephone;
        COPY_STRING(saved_outside_number, request->telephone->outside_number);
        saved_telephone.outside_number = saved_outside_number;
        saved_link = *request->pppconection;
        COPY_STRING(connect_script, request->login_script1);
        COPY_STRING(login_script, request->login_script2);
        COPY_STRING(telephone_number, request->telephone_number);
        COPY_STRING(user_name, request->login);
        COPY_STRING(password, request->password);
        saved_request.serial = &saved_serial;
        saved_request.telephone = &saved_telephone;
        saved_request.pppconection = &saved_link;
        saved_request.login_script1 = connect_script;
        saved_request.login_script2 = login_script;
        saved_request.telephone_number = telephone_number;
        saved_request.login = user_name;
        saved_request.password = password;
#ifdef NONMATCHING
        /* this side saves the signup name with the other strings, an empty one
         * where the request has none */
        COPY_STRING(signup_name, request->signup ? request->signup : "");
        saved_request.signup = signup_name;
#endif
        PPP_ConnectionStatus = AvepppConnectOpenStandby;
        PPP_OpenRequest = 1;
    }
    PP_OS_CriticalUnLock();
    return 0;
}
int ppp_is_abort(void)
{
#ifdef NONMATCHING
    /* this side aborts while no request is saved, whose telephone the test
     * below reads */
    if (!saved_request.telephone) {
        return 1;
    }
#endif
    return PPP_ConnectionStatus == AvepppConnectDisconnected ||
           PPP_CloseRequest ||
           (unsigned int)PP_GetTimeLow() - (unsigned int)connect_started >
               (unsigned int)(saved_request.telephone->timeout * 1000);
}
#ifdef NONMATCHING
/* this side's authentication for ppp_open_sub below: by the method the request
 * names, or for PPP_AUTH_AUTO by the one the peer asks for */
static short authenticate(const AvepppOpenParam *parameters,
                          const struct ppp_connection *connection)
{
    short method = parameters->pppconection->recognize & PPP_AUTH_METHOD;
    if (method == PPP_AUTH_AUTO) {
        if (connection->authentication == (PPP_AUTH_PEER | PPP_AUTH_CHAP)) {
            _PP_delay(100);
        } else if (connection->authentication !=
                   (PPP_AUTH_PEER | PPP_AUTH_PAP)) {
            return 0;
        }
        method = connection->authentication & PPP_AUTH_METHOD;
    }
    switch (method) {
    case PPP_AUTH_PAP:
        return PP_pkd_pap_open(parameters->login, parameters->password);
    case PPP_AUTH_CHAP:
        return PP_pkd_chap_open(parameters->login, parameters->password);
    default:
        return 0;
    }
}
#endif
int ppp_open_sub(void)
{
    struct ppp_connection connection;
    short result;
    short i;
    int init_result;
    unsigned int timeout;
    unsigned int started;
    unsigned long local_ip;
#ifdef NONMATCHING
#else
    unsigned long peer_ip;
#endif
    unsigned long mask;
    AvepppOpenParam *parameters;
    void (*lock)(void);
    void (*unlock)(void);
#ifdef NONMATCHING
    /* this side sets the two calls directly, the plain forms the hack tags
     * below give */
    lock = PP_OS_CriticalLock;
    unlock = PP_OS_CriticalUnLock;
#else
    /* @hack spelling: unlock set through release, where
     * unlock = PP_OS_CriticalUnLock would do */
    void (*release)(void);

    /* @hack redundant: a store of 0 to interface_up, in a block no path
     * reaches */
    if (0) {
        interface_up = 0;
    }
    lock = PP_OS_CriticalLock;
    release = PP_OS_CriticalUnLock;
    unlock = release;
    /* @hack redundant: a store to PPP_LastError, in a block no path
     * reaches */
    if (0) {
        PPP_LastError = AvepppErrorNoDialTone;
    }
#endif
    parameters = &saved_request;
    PPP_OpenRequest = 0;
    interface_up = 0;
    PPP_ConnectionStatus = AvepppConnectDialing;
    init_result = PP_pkd_ppp_up(
        0, 0, parameters->serial->parity, parameters->serial->stop,
        parameters->serial->speed, parameters->serial->databit);
    if (init_result != 0) {
        PPP_LastError = AvepppErrorModem;
        goto failure;
#ifdef NONMATCHING
#else
        /* @hack redundant: a second goto, which no path reaches */
        goto failure;
#endif
    }
    unlock();
    PP_OS_WaitMs(2000);
    lock();
    init_result =
        PP_GoScript((const Uint8 *)parameters->login_script1, parameters);
    if (init_result != 0) {
        PPP_LastError = AvepppErrorModem;
        goto failure;
    }
    result = PP_GoScript((const Uint8 *)parameters->login_script2, parameters);
    if (result != 0) {
        PPP_LastError = result;
        goto failure;
    }
    timeout = parameters->telephone->timeout * 1000;
    started = PP_GetTimeLow();
    for (;;) {
        if (PP_ComLineSence()) {
            break;
        }
        if (PPP_CloseRequest ||
            (unsigned int)(PP_GetTimeLow() - started) > timeout) {
            PPP_LastError = AvepppErrorNoDialTone;
            goto failure;
#ifdef NONMATCHING
#else
            /* @hack redundant: a second goto, which no path reaches */
            goto failure;
#endif
        }
        unlock();
        PP_OS_Yield();
        lock();
    }
    unlock();
    PP_OS_WaitMs(500);
    lock();
    connect_started = PP_GetTimeLow();
    PPP_ConnectionStatus = AvepppConnectAuthen;
    connection.character_map = ACCM_XON_XOFF;
    connection.address_compression = parameters->pppconection->acfcomp;
    connection.authentication = parameters->pppconection->recognize;
    connection.magic_number = parameters->pppconection->magic_number;
    connection.mru = parameters->pppconection->mru;
    connection.protocol_compression = parameters->pppconection->protocomp;
    connection.login = parameters->login;
    connection.password = parameters->password;
    init_result = PP_pkd_lcp_open(&connection);
    if (init_result != 0) {
        PPP_LastError = AvepppErrorLcp;
        goto auth_failed;
    }
    PP_pkd_ipcp_standby(parameters->pppconection->vjcomp, PP_ParamMyIp,
                        PP_ParamHisIp, PP_ParamMyDns1, PP_ParamMyDns2,
                        PP_ParamHisDns1, PP_ParamHisDns2);
#ifdef NONMATCHING
    /* this side runs the same steps straight, with authenticate above, where
     * the matching side jumps into its nested blocks with gotos */
    result = authenticate(parameters, &connection);
    if (result != 0) {
        PPP_LastError =
            result == PPP_AUTH_REJECTED ? AvepppErrorAuth : AvepppErrorLcp;
        goto failure;
    }
    if (PP_pkd_ipcp_open() != 0) {
        PPP_LastError = AvepppErrorIpcp;
        goto failure;
    }
    get_ppp_info();
    local_ip = 0;
    for (i = 3; i >= 0; i--) {
        local_ip = (local_ip << 8) | _ppp_info.local_ip[i];
    }
    mask = SUBNET_MASK;
    unlock();
    result = PP_ifconfig(local_ip, mask);
    lock();
    if (result != 0) {
        PPP_LastError = AvepppErrorTcp;
        goto failure;
    }
    memcpy(peer_address, _ppp_info.peer_ip, sizeof(peer_address));
    unlock();
    PP_getconfig();
    PP_addroute(0);
    lock();
    interface_up = 1;
    PPP_ConnectionStatus = AvepppConnectEstablished;
    return 0;
auth_failed:
failure:
    PPP_ConnectionStatus = AvepppConnectFail;
    return 0;
#else
    if ((parameters->pppconection->recognize & PPP_AUTH_METHOD) ==
        PPP_AUTH_PAP) {
        result = PP_pkd_pap_open(parameters->login, parameters->password);
    } else if ((parameters->pppconection->recognize & PPP_AUTH_METHOD) ==
               PPP_AUTH_CHAP) {
        result = PP_pkd_chap_open(parameters->login, parameters->password);
    } else {
        goto negotiated;
    }
    if (result == PPP_AUTH_REJECTED) {
        goto auth_rejected;
    } else if (result == 0) {
        goto authenticated;
    }
    PPP_LastError = AvepppErrorLcp;
    goto auth_failed;
negotiated:
    if ((parameters->pppconection->recognize & PPP_AUTH_METHOD) ==
        PPP_AUTH_NONE) {
        goto authenticated;
    } else if ((parameters->pppconection->recognize & PPP_AUTH_METHOD) ==
               PPP_AUTH_AUTO) {
        if (connection.authentication == (PPP_AUTH_PEER | PPP_AUTH_PAP)) {
            result = PP_pkd_pap_open(parameters->login, parameters->password);
            goto auth_result;
        } else if (connection.authentication ==
                   (PPP_AUTH_PEER | PPP_AUTH_CHAP)) {
            _PP_delay(100);
            result = PP_pkd_chap_open(parameters->login, parameters->password);
        } else {
            goto authenticated;
        }
    } else {
        goto authenticated;
    }
auth_result:
    if (result == PPP_AUTH_REJECTED) {
    auth_rejected:
        PPP_LastError = AvepppErrorAuth;
        goto rejected_exit;
    } else if (result != 0) {
        PPP_LastError = AvepppErrorLcp;
        goto auth_failed;
        /* @hack redundant: a second goto and a return, which no path
         * reaches */
        goto auth_failed;
        return 0;
    } else {
    authenticated:
        init_result = PP_pkd_ipcp_open();
        if (init_result != 0) {
            /* @hack redundant: braces around one statement */
            {
                PPP_LastError = AvepppErrorIpcp;
            }
            goto failure;
        } else {
            get_ppp_info();
            local_ip = 0;
            for (i = 3; i >= 0; i--) {
                local_ip <<= 8;
                local_ip |= _ppp_info.local_ip[i];
            }
            mask = SUBNET_MASK;
            unlock();
            result = PP_ifconfig(local_ip, mask);
            lock();
            if (result != 0) {
                PPP_LastError = AvepppErrorTcp;
            rejected_exit:
            auth_failed:
            failure:
                PPP_ConnectionStatus = AvepppConnectFail;
            } else {
                /* @hack redundant: peer_ip, built from _ppp_info and never
                 * read */
                peer_ip = 0;
                for (i = 3; i >= 0; i--) {
                    peer_ip <<= 8;
                    peer_ip |= _ppp_info.peer_ip[i];
                }
                (void)peer_ip;
                for (i = 0; i <= 3; i++) {
                    peer_address[i] = _ppp_info.peer_ip[i];
                }
                unlock();
                PP_getconfig();
                PP_addroute(0);
                lock();
                interface_up = 1;
                PPP_ConnectionStatus = AvepppConnectEstablished;
            }
            return 0;
        }
    }
#endif
}
static int request_close(void)
{
    PP_OS_CriticalLock();
    if (PPP_Executing) {
        PPP_CloseRequest = 1;
        PPP_ConnectionStatus = AvepppConnectCloseStandby;
    }
    PP_OS_CriticalUnLock();
    return 0;
}
short ppp_close_sub(void)
{
    int i;
    unsigned long peer_ip;
    unsigned long local_ip;
    unsigned long mask;
    PPP_CloseRequest = 0;
    PPP_ConnectionStatus = AvepppConnectOnhooking;
    for (peer_ip = 0, i = 3; i >= 0; i--) {
        peer_ip <<= 8;
        peer_ip |= _ppp_info.peer_ip[i];
    }
    local_ip = 0;
    for (i = 3; i >= 0; i--) {
        local_ip <<= 8;
        local_ip |= _ppp_info.local_ip[i];
    }
    mask = SUBNET_MASK;
    if (interface_up) {
        interface_up = 0;
        PP_OS_CriticalUnLock();
        PP_delroute(peer_ip);
        PP_ifconfigdown(local_ip, mask);
        PP_OS_CriticalLock();
    }
    PP_pkd_ppp_close();
    if (strlen(store_login_script3)) {
        PP_GoScript((const Uint8 *)store_login_script3, &saved_request);
    }
    if (PPP_ComUsing) {
        PP_ComClose();
        PPP_ComUsing = 0;
    }
    PPP_Executing = 0;
    PPP_OpenRequest = 0;
    PPP_CloseRequest = 0;
    PPP_ConnectionStatus = AvepppConnectUnused;
    return 0;
}
static int read_status(AvepppStatus *status)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    struct ppp_ipcp *ipcp = fsm->context;
#ifdef NONMATCHING
    /* this side refuses a missing status, and gives 0 for each address while
     * IPCP has no context */
    if (!status) {
        return -1;
    }
    status->his_ip = ipcp ? ipcp->local.want.peer_address : 0;
    status->my_ip = ipcp ? ipcp->local.want.address : 0;
#else
    status->his_ip = ipcp->local.want.peer_address;
    status->my_ip = ipcp->local.want.address;
#endif
    status->phase = session->phase == PPP_DEAD ? AvepppConnectDisconnected
                                               : PPP_ConnectionStatus;
    status->baud_rate = PP_ConnectRate;
    status->last_error = PPP_LastError;
    status->port_inuse = PPP_Executing;
    return 0;
}
static int read_counters(struct session_counters *counters)
{
    struct ppp_session *session = &PP_ppp_str;
    if (!counters) {
        return -1;
    }
    counters->sent_bytes = session->octets_sent;
    counters->sent_packets = session->packets_sent;
    counters->ipcp_sent = session->control_sent[PPP_IPCP];
    counters->send_status = session->send_status;
    counters->receive_status = session->receive_status;
    counters->received_bytes = session->octets_received;
    counters->received_packets = session->packets_received;
    counters->ipcp_received = session->received.protocol.ipcp;
    counters->unknown = session->unknown_protocol;
    counters->discarded = session->discarded;
    counters->overflow = session->overflow;
    counters->bad_frames = session->receive_errors;
    counters->allocation_errors = session->allocation_errors;
    return 0;
}
void AvepppSetDns(long myip, long hisip, long mydns1, long mydns2, long hisdns1,
                  long hisdns2)
{
    PP_OS_CriticalLock();
    PP_ParamMyIp = myip;
    PP_ParamHisIp = hisip;
    PP_ParamMyDns1 = mydns1;
    PP_ParamMyDns2 = mydns2;
    PP_ParamHisDns1 = hisdns1;
    PP_ParamHisDns2 = hisdns2;
    PP_OS_CriticalUnLock();
}
void AvepppGetDns(long *myip, long *hisip, long *mydns1, long *mydns2,
                  long *hisdns1, long *hisdns2)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    struct ppp_ipcp *ipcp = fsm->context;
    PP_OS_CriticalLock();
#ifdef NONMATCHING
    /* this side gives 0 for each address while IPCP has no context */
    if (!ipcp) {
        *myip = *hisip = *mydns1 = *mydns2 = *hisdns1 = *hisdns2 = 0;
        PP_OS_CriticalUnLock();
        return;
    }
#endif
    *myip = ipcp->local.want.address;
    *hisip = ipcp->local.want.peer_address;
    *mydns1 = ipcp->local.want.dns1;
    *mydns2 = ipcp->local.want.dns2;
    *hisdns1 = ipcp->remote.want.dns1;
    *hisdns2 = ipcp->remote.want.dns2;
    PP_OS_CriticalUnLock();
}

#include <access/at_api.h>
#include <access/aveppp.h>
#include <access/dns.h>
#include <access/in.h>
#include <access/tcp_private.h>
#include <access/vsyncpro.h>
#include <nt_utl.h>
#include <sg_xpt.h>
#include <string.h>
#include "chaostgkinder/network.h"
#include "chaostgkinder/network_layout.h"
#include "main.h"
#include "system/areas.h"
#include "system/frame.h"

/* a numeric ticket's slot, for an address its name spelled out */
struct address_ticket {
    Bool active;
    Uint32 address;
};
#define TICKET_SLOTS 4
/* a numeric ticket is this bit and the index of its slot in tickets. any
 * other ticket is the resolver's */
#define NUMERIC_TICKET 0x40000000
/* in the network information's flag: dial by pulses, not tones */
#define ISP_PULSE_DIAL 0x100
/* the room for the network information at WORK_AREA, ahead of the flash
 * manager's workspace */
#define NETWORK_INFO_ROOM 0x4000

Sint32 kinder_isp_result;
struct isp_account kinder_isp_accounts[ISP_ACCOUNTS];
Sint32 kinder_selected_isp;
static Bool pulse_dial;
static Sint32 ppp_handle;
static Uint32 primary_dns;
static Uint32 secondary_dns;
static Bool ppp_open;
static Bool resolver_open;
static struct address_ticket tickets[TICKET_SLOTS];

#ifdef NONMATCHING
#else
/* call each function of table, which is size bytes long. an entry of NULL
 * or -1 holds none */
static void run_initializers(network_initializer *table, int size)
{
    network_initializer *entry = table;
    network_initializer *end = entry + (size / (int)sizeof(*entry));
    network_initializer fn;
    while (entry != end) {
        fn = *entry++;
        if (fn != NULL && fn != (network_initializer)-1) {
            fn();
        }
    }
}
#endif
static void network_frame_wait(void)
{
    vsWaitVSync(1);
}
static void read_isp_accounts(void)
{
    void *buffer = WORK_AREA;
    void *work = WORK_AREA + NETWORK_INFO_ROOM;
    NetworkInfo3 *info;
    int i;
    int j;
    NetworkAccessInfo *source;
    struct isp_account *account;
    if ((kinder_isp_result = ntInfInit(buffer, work)) == NTD_OK) {
        /* @hack redundant: the empty else of this test, which nothing needs */
        if ((kinder_isp_result = ntInfGetNetworkInfo3(&info)) == NTD_OK) {
            kinder_selected_isp = (info->flag & ISP_USE2) ? 1 : 0;
            pulse_dial = (info->flag & ISP_PULSE_DIAL) ? TRUE : FALSE;
            memset(kinder_isp_accounts, 0, sizeof(kinder_isp_accounts));
            for (i = 0; i < ISP_ACCOUNTS; i++) {
                if (i == 0) {
                    source = &info->isp_info1;
                } else {
                    source = &info->isp_info2;
                }
                account = &kinder_isp_accounts[i];
                memcpy(account->login, source->loginId,
                       sizeof(source->loginId));
                memcpy(account->password, source->loginPasswd,
                       sizeof(source->loginPasswd));
                memcpy(account->mail, source->mailAddress,
                       sizeof(source->mailAddress));
                for (j = 0; (unsigned)j <
                            sizeof(account->phone) / sizeof(account->phone[0]);
                     j++) {
                    memcpy(account->phone[j], source->accessPointNumber[j],
                           sizeof(source->accessPointNumber[j]));
                }
                account->primary_dns = source->dnsServerAddress1.s_addr;
                account->secondary_dns = source->dnsServerAddress2.s_addr;
            }
        } else {
        }
        ntInfExit();
    }
}
void kinder_network_start(void)
{
#ifdef NONMATCHING
    /* this side clears and constructs the sections by
     * kinder_network_init_sections, as network_layout.h says */
    kinder_network_init_sections();
#else
    memset(kinder_bss, 0, kinder_bss_bytes);
    memset(kinder_sg_bss, 0, kinder_sg_bss_bytes);
    run_initializers(kinder_sg_constructors, kinder_sg_ctor_bytes);
    run_initializers(kinder_constructors, kinder_ctor_bytes);
#endif
    read_isp_accounts();
    game_set_frame_wait(network_frame_wait);
    vsInitVSyncProc(frame_wait);
    AvepppInitialize();
    AT_init(NULL);
    ppp_open = FALSE;
    resolver_open = FALSE;
}
void kinder_network_stop(void)
{
    if (resolver_open) {
        ADNS_Finalize();
    }
    AT_disp(NULL);
    AvepppTerminate();
    vsFinishVSyncProc();
#ifdef NONMATCHING
    /* this side runs the destructors by kinder_network_fini_sections, as
     * network_layout.h says */
    kinder_network_fini_sections();
#else
    run_initializers(kinder_destructors, kinder_dtor_bytes);
    run_initializers(kinder_sg_destructors, kinder_sg_dtor_bytes);
#endif
    game_set_frame_wait(NULL);
}

void kinder_connection_dial(int number)
{
    AvepppSerialParam serial;
    AvepppTelephoneParam phone;
    AvepppConnectionParam connection;
    AvepppOpenParam params;
    struct isp_account *account = &kinder_isp_accounts[kinder_selected_isp];

    params.serial = &serial;
    params.telephone = &phone;
    params.pppconection = &connection;
    params.login_script1 =
        "send\"AT\\r\"\nwait\"OK\",3\nsend\"AT&F\\r\"\nwait\"OK\",6\n"
        "send\"ATX3\\r\"\nwait\"OK\",2\n";
    params.login_script2 = "dial\"%t\"\n";
    params.telephone_number = account->phone[number];
    params.login = account->login;
    params.password = account->password;
    params.signup = NULL;
    params.reserved_1 = 0;
    params.reserved_2 = 0;
    serial.speed = 9;
    serial.stop = 0;
    serial.parity = 2;
    serial.flow = 1;
    serial.databit = 0;
    serial.reserved_1 = 0;
    serial.reserved_2 = 0;
    phone.dialtype = pulse_dial;
    phone.outside_line = 0;
    phone.timeout = 70;
    phone.dial_retry = 0;
    phone.dial_interval = 30;
    phone.outside_number = "0,";
    phone.reserved_1 = 0;
    phone.reserved_2 = 0;
#ifdef NONMATCHING
    /* this side starts both addresses at 0, which the matching side passes as
     * the stack leaves them */
    connection.his_ip = 0;
    connection.my_ip = 0;
#endif
    connection.recognize = 4;
    connection.mru = 1500;
    connection.magic_number = 0x12345678;
    connection.acfcomp = 0;
    connection.protocomp = 0;
    connection.vjcomp = 0;
    connection.reserved_1 = 0;
    connection.reserved_2 = 0;
    AvepppSetDns(0, 0, 0, 0, 0, 0);
    ppp_handle = AvepppOpen(&params);
    ppp_open = TRUE;
}
int kinder_connection_hang_up(void)
{
    int result = 0;
    if (ppp_open) {
        result = AvepppClose(ppp_handle);
    }
    return result;
}
enum kinder_connection_state kinder_connection_state(void)
{
    AvepppStatus status;
    enum kinder_connection_state state = CONNECTION_UNKNOWN;
    AvepppGetStatus(&status);
    switch (status.phase) {
    case AvepppConnectUnused:
        state = CONNECTION_IDLE;
        break;
    case AvepppConnectOpenStandby:
    case AvepppConnectDialing:
        state = CONNECTION_DIALING;
        break;
    case AvepppConnectAuthen:
        state = CONNECTION_NEGOTIATING;
        break;
    case AvepppConnectEstablished:
        state = CONNECTION_ONLINE;
        break;
    case AvepppConnectDisconnected:
    case AvepppConnectFail:
        state = CONNECTION_DISCONNECTED;
        break;
    case AvepppConnectCloseStandby:
    case AvepppConnectOnhooking:
        state = CONNECTION_CLOSING;
        break;
    }
    return state;
}
const char *kinder_connection_number(int number)
{
    return kinder_isp_accounts[kinder_selected_isp].phone[number];
}
static Bool read_ipv4(const char *text, Uint8 *address)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. an address takes
     * four parts of one to three digits, each at most 255 */
    unsigned part = 0;
    unsigned count = 0;
    unsigned digits = 0;
    for (;;) {
        int ch = *text++;
        if (ch >= '0' && ch <= '9') {
            part = (part * 10) + (ch - '0');
            if (++digits > 3 || part > 255) {
                return FALSE;
            }
        } else {
            if (!digits || (ch != '.' && ch != '\0')) {
                return FALSE;
            }
            address[count++] = (Uint8)part;
            if (!ch) {
                return count == 4;
            }
            if (count == 4) {
                return FALSE;
            }
            digits = 0;
            part = 0;
        }
    }
#else
    /* @bug logic: takes any name of digits and dots for an address, whatever
     * its count of parts and of their digits. read_ipv4 returns TRUE at
     * 0x0c9d408a once it meets the name's end or its fourth part's end */
    int ch;
    int count = 0;
    Uint8 octet = 0;
    do {
        ch = *text++;
        if (ch == 0 || ch == '.') {
            /* @bug arithmetic: keeps each part mod 256. the store at
             * 0x0c9d401c writes the part's low byte alone */
            address[count++] = octet;
            octet = 0;
        } else if (ch >= '0' && ch <= '9') {
            octet = (octet * 10) + ch - '0';
        } else {
            return FALSE;
        }
    } while (ch != 0 && count < 4);
    return TRUE;
#endif
}
void kinder_resolver_start(void)
{
    long local_ip;
    long peer_ip;
    long peer_primary;
    long peer_secondary;
    long local_primary;
    long local_secondary;
    int i;
    local_ip = 0;
    peer_ip = 0;
    local_primary = 0;
    local_secondary = 0;
    peer_primary = 0;
    peer_secondary = 0;
    AvepppGetDns(&local_ip, &peer_ip, &local_primary, &local_secondary,
                 &peer_primary, &peer_secondary);
    primary_dns = kinder_isp_accounts[kinder_selected_isp].primary_dns;
    if (local_primary) {
        primary_dns = htonl(local_primary);
    }
    secondary_dns = kinder_isp_accounts[kinder_selected_isp].secondary_dns;
    if (local_secondary) {
        secondary_dns = htonl(local_secondary);
    }
    ADNS_Initialize((const Uint8 *)"", primary_dns, secondary_dns);
    resolver_open = TRUE;
    for (i = 0; (unsigned)i < sizeof(tickets) / sizeof(tickets[0]); i++) {
        tickets[i].active = FALSE;
    }
}
int kinder_lookup_start(const char *name)
{
    Uint32 address;
    int i;
    struct address_ticket *ticket;
    if (read_ipv4(name, (Uint8 *)&address)) {
        for (i = 0; (unsigned)i < sizeof(tickets) / sizeof(tickets[0]); i++) {
            ticket = &tickets[i];
            if (!ticket->active) {
                ticket->active = TRUE;
                /* @bug uninitialized: copies the bytes of address that
                 * read_ipv4 left unset for a name of fewer than four parts,
                 * at 0x0c9d4176 */
                ticket->address = address;
                return i | NUMERIC_TICKET;
            }
        }
    }
    return ADNS_GetTicket((const Uint8 *)name);
}
void kinder_lookup_release(Sint32 ticket)
{
    /* @hack spelling: the rest under an else after the return, where it
     * could follow the return unnested */
    if (ticket < 0) {
        return;
    } else {
        if (ticket & NUMERIC_TICKET) {
            ticket &= ~NUMERIC_TICKET;
            if ((Uint32)ticket < TICKET_SLOTS) {
                tickets[ticket].active = FALSE;
            }
        } else {
            ADNS_ReleaseTicket(ticket);
        }
    }
}
enum kinder_lookup_status kinder_lookup_poll(Sint32 ticket, Uint32 *address)
{
    enum kinder_lookup_status status = KINDER_LOOKUP_FAILED;
    /* @hack spelling: the empty branch for a negative ticket, where testing
     * ticket >= 0 would do */
    if (ticket < 0) {
    } else {
        if (ticket & NUMERIC_TICKET) {
            ticket &= ~NUMERIC_TICKET;
            if ((Uint32)ticket < TICKET_SLOTS) {
                *address = tickets[ticket].address;
                status = KINDER_LOOKUP_DONE;
            }
        } else {
            switch (ADNS_LookUp(ticket, (Uint8 *)address)) {
            case eOK:
                status = KINDER_LOOKUP_DONE;
                break;
            case eWouldBlock:
                status = KINDER_LOOKUP_WAITING;
                break;
            /* @hack redundant: the cases eGeneric and eTimedOut, which the
             * switch's end would take */
            case eGeneric:
            case eTimedOut:
                break;
            }
        }
    }
    return status;
}
static void socket_event(Sint16 handle, Sint16 event)
{
}
Sint16 kinder_tcp_open(Uint32 address, Uint16 port)
{
    AT_openparam params;
    params.youraddr.u32 = address;
    params.yourport.u16 = port;
    params.myport.u16 = 0;
    params.reserve1 = 9;
    params.ttl = 255;
    params.svctype = 0;
    params.df_flag = 0;
    params.reserve2 = 0;
    params.reserve3 = NULL;
    params.asraddr = socket_event;
    return AT_apiCall(TCP_OPEN, &params, NULL);
}
void kinder_tcp_close(Sint16 handle)
{
    AT_nhparam params;
    params.nh = handle;
    AT_apiCall(TCP_CLOSE, &params, NULL);
}
void kinder_tcp_status(Sint16 handle, struct kinder_tcp_status *status)
{
    AT_nhparam params;
    AT_stat_r result;
    params.nh = handle;
    if (AT_apiCall(TCP_STAT, &params, &result) < 0) {
        status->state = TCP_FAILED;
        /* @bug uninitialized: copies result's window sizes after TCP_STAT
         * fails, which writes none for a handle it rejects, at 0x0c9d43a6 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the failure reports both
         * sizes 0 */
        status->send_size = 0;
        status->receive_size = 0;
        return;
#endif
    } else {
        switch (result.stat) {
        case TCPS_CLOSED:
            status->state = TCP_CLOSED;
            break;
        case TCPS_SYN_SENT:
        case TCPS_SYN_RECEIVED:
            status->state = TCP_CONNECTING;
            break;
        case TCPS_LISTEN:
        case TCPS_ESTABLISHED:
            status->state = TCP_CONNECTED;
            break;
        case TCPS_CLOSE_WAIT:
        case TCPS_FIN_WAIT_1:
        case TCPS_CLOSING:
        case TCPS_LAST_ACK:
        case TCPS_FIN_WAIT_2:
        case TCPS_TIME_WAIT:
            status->state = TCP_CLOSING;
            break;
        default:
            status->state = TCP_FAILED;
            break;
        }
    }
    status->send_size = result.sendwin;
    status->receive_size = result.recvwin;
}
int kinder_tcp_receive(Sint16 handle, Uint8 *buffer, int count)
{
    AT_rcvparam params;
    params.nh = handle;
    params.len = count;
    params.buf = buffer;
    params.notifyaddr = NULL;
    return AT_apiCall(TCP_RECEIVE, &params, NULL);
}
int kinder_tcp_send(Sint16 handle, Uint8 *buffer, int count)
{
    AT_sendparam params;
    params.nh = handle;
    params.notifyaddr = NULL;
    params.reserve = 0;
    params.bufnum = 1;
    params.buffers[0].len = count;
    params.buffers[0].buf = buffer;
    return AT_apiCall(TCP_SEND, &params, NULL);
}

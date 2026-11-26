#include <access/aveppp.h>
#include <access/datalink.h>
#include <sg_xpt.h>
#include <string.h>
#include "packet_driver.h"
#include "ppp_driver.h"
#include "ppp_framing.h"
#include "ppp_fsm.h"
#include "ppp_link.h"
#include "ppp_modem.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_serial.h"
#include "ppp_system.h"

/* PP_GetPPPStatus's result while IPCP isn't open */
enum { PPP_NOT_OPEN = -16 };
struct pd_handle {
    Uint8 used, if_class;
    Uint16 if_type;
    Uint8 if_number;
    char type[PD_TYPE_BYTES];
    Uint16 type_length;
    pd_receiver receive;
};
static short driver_version = 1;
static char interface_class[PD_INTERFACES] = {PD_CLASS_ETHERNET,
                                              PD_CLASS_ETHERNET};
static short driver_type = PD_DRIVER_TYPE;
static char interface_number[PD_INTERFACES] = {0, 1};
static char driver_functionality = 5;
#ifdef DREAMCAST
static char driver_reserved = 0;
#endif
static char driver_name[] = "Packet Driver";
/* the Ethernet addresses the driver makes up for this end and for the peer */
/* clang-format off */
static Uint8 local_address[ETHERNET_ADDRESS_BYTES] = {1, 0x80, 0x92, 0x95, 9, 0x13};
static Uint8 remote_address[ETHERNET_ADDRESS_BYTES] = {1, 0x80, 0x92, 0x95, 9, 0x14};
/* clang-format on */
static struct pd_handle handles[PD_HANDLES];
static struct pd_statistics statistics[PD_HANDLES];
/* @hack type: an array of one, where a char would do */
static char receive_mode[1];
static struct ppp_buffer send_packet;
#ifdef NONMATCHING
/* this side keeps the headroom and the payload in one record, so the headers
 * PP_pushdown puts ahead of the payload stay inside one object. GCC promises no
 * array's place beside another */
static struct {
    Uint8 headroom[PPP_HEADROOM], payload[PPP_FRAME_DATA_BYTES];
} outgoing;
#define SEND_PAYLOAD outgoing.payload
#else
/* the room ahead of send_payload where PP_pushdown puts PPP's headers. it
 * measures the room from send_packet */
static Uint8 header_space[PPP_HEADROOM];
static Uint8 send_payload[PPP_FRAME_DATA_BYTES];
#define SEND_PAYLOAD send_payload
#endif
static short open_serial(short port, short parity, short stop, short speed,
                         short bits);
#ifdef NONMATCHING
/* this side takes a handle as the Sint32 its callers pass, where a short would
 * truncate it past check_handle's tests */
typedef Sint32 pd_handle_number;
#else
typedef short pd_handle_number;
#endif
static short check_handle(pd_handle_number handle);
static short send_frame(const char *frame, short length);

short PP_pkd_ppp_up(short flags, short port, short parity, short stop,
                    short speed, short data_bits)
{
    int i;
    short result;
    for (i = 0; i < PD_HANDLES; i++) {
        handles[i].used = 0;
    }
    result = open_serial(port, parity, stop, speed, data_bits);
    ppp_iostatus(PPP_SERIAL_CONNECT);
    return result;
}
short PP_pkd_lcp_open(struct ppp_connection *configuration)
{
    struct ppp_session *session;
    struct ppp_fsm *fsm;
    struct ppp_lcp_side *peer;
    PP_Com_reflush(configuration);
    session = &PP_ppp_str;
    fsm = &session->fsm[PPP_LCP];
    if (PP_dolcp_open(fsm, configuration) != 0) {
        return PPP_LINK_FAILED;
    }
    if (PP_lcp_monitor(fsm) != 0) {
        return PPP_LINK_FAILED;
    }
    if (configuration->authentication == PPP_AUTH_AUTO) {
        /* @hack spelling: the remote side stepped from the local, where
         * &((struct ppp_lcp *)fsm->context)->remote would do */
        peer = (struct ppp_lcp_side *)fsm->context + 1;
        if (peer->work.authentication == PPP_PAP_PROTOCOL) {
            configuration->authentication = PPP_AUTH_PEER | PPP_AUTH_PAP;
        }
        if (peer->work.authentication == PPP_CHAP_PROTOCOL) {
            configuration->authentication = PPP_AUTH_PEER | PPP_AUTH_CHAP;
        }
    }
    return 0;
}
short PP_pkd_pap_open(char *login, char *password)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_PAP];
    short result;
#ifdef NONMATCHING
    /* this side fails where PAP's user can't be set or its request can't be
     * sent, which the matching side ignores */
    if (PP_dopap_user(login, password, fsm)) {
        return PPP_AUTH_REJECTED;
    }
    if (PP_fsm_sendreq(fsm)) {
        return PPP_LINK_FAILED;
    }
#else
    PP_dopap_user(login, password, fsm);
    PP_fsm_sendreq(fsm);
#endif
    result = PP_pap_monitor(fsm);
    if (result == -1) {
        return PPP_AUTH_REJECTED;
    }
    if (result != 0) {
        return PPP_LINK_FAILED;
    }
    PP_doppp_active(&session->fsm[PPP_IPCP]);
    return 0;
}
short PP_pkd_chap_open(char *login, char *password)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_CHAP];
    short result;
#ifdef NONMATCHING
    /* this side fails where CHAP's user can't be set, which the matching side
     * ignores */
    if (PP_dochap_user(login, password, fsm)) {
        return PPP_LINK_FAILED;
    }
#else
    PP_dochap_user(login, password, fsm);
#endif
    result = PP_chap_monitor(fsm);
    if (result == -1) {
        return PPP_AUTH_REJECTED;
    }
    if (result != 0) {
        return PPP_LINK_FAILED;
    }
    PP_doppp_active(&session->fsm[PPP_IPCP]);
    return 0;
}
short PP_pkd_ipcp_standby(short compression, long local_ip, long peer_ip,
                          long local_dns1, long local_dns2, long peer_dns1,
                          long peer_dns2)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    struct ppp_ipcp *ipcp = fsm->context;
    PP_doppp_active(fsm);
    if (PP_doipcp_compress(compression, &ipcp->local) ||
        PP_doipcp_address(&ipcp->local, local_ip) ||
        PP_doipcp_dns1_address(&ipcp->local, local_dns1) ||
        PP_doipcp_dns2_address(&ipcp->local, local_dns2) ||
        PP_doipcp_remote_address(&ipcp->remote, peer_ip) ||
        PP_doipcp_remote_dns1_address(&ipcp->remote, peer_dns1) ||
        PP_doipcp_remote_dns2_address(&ipcp->remote, peer_dns2)) {
        return PPP_LINK_FAILED;
    }
    return 0;
}
short PP_pkd_ipcp_open(void)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    if (PP_doipcp_open(fsm) != 0) {
        return PPP_LINK_FAILED;
    }
    if (PP_ipcp_monitor(fsm) != 0) {
        return PPP_LINK_FAILED;
    }
    return 0;
}
short PP_pkd_ppp_close(void)
{
    struct ppp_session *session = &PP_ppp_str;
    PP_fsm_down(&session->fsm[PPP_IPCP]);
    PP_fsm_down(&session->fsm[PPP_PAP]);
    PP_fsm_down(&session->fsm[PPP_CHAP]);
    PP_fsm_down(&session->fsm[PPP_LCP]);
    session->phase = PPP_DEAD;
    ppp_restart();
    return 0;
}
Sint32 DPD_driver_info(Sint32 interface, struct pd_info *info)
{
#ifdef NONMATCHING
    /* this side refuses a missing info and an interface past PD_INTERFACES,
     * which indexes the tables below */
    if (!info || interface < 0 || interface >= PD_INTERFACES) {
        return PD_NO_NUMBER;
    }
#endif
    info->if_class = interface_class[interface];
    info->if_type = driver_type;
    info->if_number = interface_number[interface];
    info->name = driver_name;
    info->functionality = driver_functionality;
    info->version = driver_version;
    return 0;
}
Sint32 DPD_access_type(Sint32 if_class, Sint32 if_type, Sint32 if_number,
                       const char *type, Uint32 type_length,
                       pd_receiver receive)
{
    short free_handle = -1;
    short i;
    short common_length;
    unsigned long occupied_length;
    /* @hack type: 20 bytes, where the PD_TYPE_BYTES a type may have would do */
    char saved_type[20];
    struct pd_handle *entry;
    struct pd_handle *registered;
    struct pd_handle *table;
#ifdef NONMATCHING
    /* this side refuses an interface past PD_INTERFACES, which indexes
     * interface_class below, a missing receiver, and a length with no type */
    if (if_number < 0 || if_number >= PD_INTERFACES) {
        return PD_NO_NUMBER;
    }
    if (!receive || (!type && type_length)) {
        return PD_BAD_HANDLE;
    }
#endif
    if (if_class != interface_class[if_number]) {
        return PD_NO_CLASS;
    }
    if (if_type != driver_type && if_type != PD_ANY_TYPE) {
        return PD_NO_TYPE;
    }
    if (if_number != 0 && if_number != 1) {
        return PD_NO_NUMBER;
    }
    if (type_length > PD_TYPE_BYTES) {
        return PD_TYPE_IN_USE;
    }
#ifdef NONMATCHING
    /* this side copies nothing for a type of no bytes, which may be null */
    if (type_length)
#endif
    {
        memcpy(saved_type, type, type_length);
    }
    /* @hack spelling: the table read through registered, table and index,
     * where handles[i] would do */
    registered = handles;
    table = registered;
    for (i = 0; i < (short)PD_HANDLES; i++) {
        if (registered[i].used) {
            short index = i;
            occupied_length = table[index].type_length;
            common_length =
                type_length < occupied_length ? type_length : occupied_length;
            if (memcmp(saved_type, table[index].type, common_length) == 0) {
                return PD_TYPE_IN_USE;
            }
        } else if (free_handle == -1) {
            free_handle = i;
        }
    }
    if (free_handle == -1) {
        return PD_BAD_HANDLE;
    }
    entry = &registered[free_handle];
    entry->used++;
    entry->if_number = if_number;
    entry->if_class = if_class;
    memcpy(entry->type, saved_type, type_length);
    entry->type_length = type_length;
    entry->receive = receive;
    return free_handle;
}
Sint32 DPD_release_type(Sint32 handle)
{
    if (check_handle(handle) == 0) {
        if (handles[handle].used) {
            handles[handle].used = 0;
#ifdef NONMATCHING
            /* this side forgets the released handle's receiver */
            handles[handle].receive = NULL;
#endif
        }
    }
    return 0;
}
Sint32 DPD_send_pkt(const void *frame, Uint32 length)
{
    if (length > PD_MAX_FRAME) {
        return PD_NO_SPACE;
    }
#ifdef NONMATCHING
    /* this side refuses a missing frame, and one shorter than its Ethernet
     * header */
    if (!frame || length < ETHERNET_HEADER_BYTES) {
        return PD_NO_SPACE;
    }
#endif
    statistics[0].packets_out++;
    statistics[0].bytes_out += length;
#ifdef NONMATCHING
    /* a short frame's padding would only follow the datagram over PPP */
#else
    if (length < PD_MIN_SEND) {
        length = PD_MIN_SEND;
    }
#endif
    return send_frame(frame, length);
}
Sint32 DPD_terminate(Sint32 mode)
{
    return 0;
}
/* DLD_get_address writes *length, and the data link calls both through one
 * pointer type */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
Sint32 DPD_get_address(Sint32 handle, Uint8 *address, Sint32 *length)
{
    if (check_handle(handle) != 0) {
        return PD_BAD_HANDLE;
    }
    if (*length < ETHERNET_ADDRESS_BYTES) {
        return PD_NO_SPACE;
    }
    memcpy(address, local_address, ETHERNET_ADDRESS_BYTES);
    return 0;
}
Sint32 DPD_reset_interface(Sint32 handle)
{
    short i;
    short active;
    if (check_handle(handle) != 0) {
        return PD_BAD_HANDLE;
    }
    active = 0;
    for (i = 0; i < (short)PD_HANDLES; i++) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: each step tests entry i */
        if (handles[i].used && handles[i].if_number == 0) {
#else
        /* @bug logic: a reset goes ahead while another handle on interface 0
         * is in use. each step tests handle's own entry, where entry i is
         * meant (0x0c9e0b40) */
        if (handles[handle].used && handles[handle].if_number == 0) {
#endif
            active++;
        }
    }
    if (active) {
        return PD_CANNOT_RESET;
    }
    return 0;
}
Sint32 Pd_get_parameter(struct pd_parameters *parameters)
{
    parameters->version_major = 1;
    parameters->version_minor = 9;
    parameters->record_length = sizeof(*parameters);
    parameters->address_length = ETHERNET_ADDRESS_BYTES;
    parameters->mtu = 1512;
    parameters->multicast_bytes = 64;
    parameters->receive_buffers = 9;
    parameters->send_buffers = 3;
    parameters->interrupt = 0;
    return 0;
}
Sint32 DPD_as_send_pkt(void)
{
    return PD_BAD_COMMAND;
}
static short check_handle(pd_handle_number handle)
{
#ifdef NONMATCHING
    /* this side refuses a negative handle, which the test below lets through */
    if (handle < 0) {
        return PD_BAD_HANDLE;
    }
#endif
    if (handle >= PD_HANDLES) {
        return PD_BAD_HANDLE;
    }
    if (!handles[handle].used) {
        return PD_BAD_HANDLE;
    }
    return 0;
}
Sint32 DPD_set_rcv_mode(Sint32 handle, Sint32 mode)
{
    if (check_handle(handle) != 0) {
        return PD_BAD_HANDLE;
    }
    if (mode < PD_RECEIVE_OFF || mode > PD_RECEIVE_ALL) {
        return PD_BAD_MODE;
    }
    receive_mode[0] = mode;
    return 0;
}
Sint32 DPD_get_rcv_mode(Sint32 handle, Sint32 *mode)
{
    if (check_handle(handle) != 0) {
        return PD_BAD_HANDLE;
    }
    *mode = receive_mode[0];
    return 0;
}
Sint32 DPD_get_statistics(struct pd_statistics *output)
{
    *output = statistics[0];
    return 0;
}
static short send_frame(const char *frame, short length)
{
    struct ppp_buffer *packet = &send_packet;
    PP_OS_CriticalLock();
    if (PPP_ConnectionStatus == AvepppConnectEstablished) {
        packet->data = SEND_PAYLOAD;
        memcpy(packet->data, frame + ETHERNET_HEADER_BYTES,
               length - ETHERNET_HEADER_BYTES);
        packet->length = length - ETHERNET_HEADER_BYTES;
        ppp_send(packet);
    }
    PP_OS_CriticalUnLock();
    return 0;
}
static short open_serial(short port, short parity, short stop, short speed,
                         short bits)
{
    struct ppp_serial_config config;
    config.port = port;
    config.parity = parity;
    config.stop = stop;
    config.enabled = 1;
    config.speed = speed;
    config.data_bits = bits;
    config.flags = 0;
    if (PP_ComOpen(&config) != 0) {
        return -1;
    }
    PPP_ComUsing = 1;
    return 0;
}
short PP_direct_pk_send(char *bytes, short length)
{
    if (PP_ComWrite(length, bytes) != 0) {
        return -1;
    }
    return 0;
}
short PP_direct_pk_recv(char *bytes, short length)
{
    char byte;
    Uint16 received;
#ifdef NONMATCHING
    /* this side refuses a negative length, on which the loop below would run
     * past bytes */
    if (length < 0) {
        return -1;
    }
#endif
    while (length) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: a failed read, or one with no
         * byte, ends the receive */
        if (PP_ComRead(1, &byte, &received) != 0 || received != 1) {
#else
        /* @bug logic: stores byte unread where PP_ComRead succeeds with no
         * byte. the test needs the call to fail and no byte to come, where
         * either is meant (0x0c9e0d1e) */
        if (PP_ComRead(1, &byte, &received) != 0 && received != 1) {
#endif
            return -1;
        }
        *bytes++ = byte;
        length--;
    }
    return 0;
}
short PP_direct_pk_stat(Uint16 *count)
{
    if (PP_ComReadedLen(count) != 0) {
        return -1;
    }
    return 0;
}
short PP_GetPPPStatus(struct ppp_driver_statistics *status)
{
    struct ppp_session *session = &PP_ppp_str;
    struct ppp_fsm *fsm = &session->fsm[PPP_IPCP];
    struct ppp_ipcp *ipcp = fsm->context;
    /* @hack spelling: address and peer_address read through local, where
     * ipcp->local would do */
    struct ipcp_side *local;
    long local_ip;
    long peer_ip;
    long local_dns1;
    long local_dns2;
    long peer_dns1;
    long peer_dns2;
    short i;
    if (fsm->state != PPP_OPENED) {
        return PPP_NOT_OPEN;
    }
    local = &ipcp->local;
    local_ip = local->want.address;
    peer_ip = local->want.peer_address;
    local_dns1 = ipcp->local.want.dns1;
    local_dns2 = ipcp->local.want.dns2;
    peer_dns1 = ipcp->remote.want.dns1;
    peer_dns2 = ipcp->remote.want.dns2;
    for (i = 3; i >= 0; i--) {
        status->local_ip[i] = local_ip;
        local_ip >>= 8;
    }
    for (i = 3; i >= 0; i--) {
        status->peer_ip[i] = peer_ip;
        peer_ip >>= 8;
    }
    for (i = 3; i >= 0; i--) {
        status->local_dns1[i] = local_dns1;
        local_dns1 >>= 8;
        status->local_dns2[i] = local_dns2;
        local_dns2 >>= 8;
        status->peer_dns1[i] = peer_dns1;
        peer_dns1 >>= 8;
        status->peer_dns2[i] = peer_dns2;
        peer_dns2 >>= 8;
    }
    status->octets_sent = session->octets_sent;
    status->packets_sent = session->packets_sent;
    for (i = 0; i < (short)PPP_PROTOCOLS; i++) {
        status->control_sent[i] = session->control_sent[i];
    }
    status->send_status = session->send_status;
    status->receive_status = session->receive_status;
    status->octets_received = session->octets_received;
    status->packets_received = session->packets_received;
    for (i = 0; i < (short)PPP_PROTOCOLS; i++) {
        status->control_received[i] = session->received.count[i];
    }
    status->unknown = session->unknown_protocol;
    status->discarded = session->discarded;
    status->overflow = session->overflow;
    status->receive_errors = session->receive_errors;
    status->receive_allocation_errors = session->allocation_errors;
    return 0;
}
short PP_ip_route(struct ppp_buffer *packet, int port)
{
    short length;
    pd_receiver receive;
    Uint8 *frame;
#ifdef NONMATCHING
    /* this side refuses a missing packet, one whose frame would pass
     * PD_MAX_FRAME, and a route while no handle is open on interface 0 */
    if (!handles[0].used || !packet ||
        packet->length > PD_MAX_FRAME - ETHERNET_HEADER_BYTES) {
        return -1;
    }
#endif
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((length = packet->length + ETHERNET_HEADER_BYTES) < PD_MIN_RECEIVE) {
        length = PD_MIN_RECEIVE;
    }
    if ((receive = handles[0].receive) == NULL) {
        return -1;
    }
    if ((frame = receive(NULL, PD_GET_BUFFER, length, NULL)) == NULL) {
        return -1;
    }
    memcpy(frame, local_address, ETHERNET_ADDRESS_BYTES);
    memcpy(frame + ETHERNET_ADDRESS_BYTES, remote_address,
           ETHERNET_ADDRESS_BYTES);
    frame[ETHERNET_TYPE_OFFSET] = ETHERTYPE_IP >> 8;
    frame[ETHERNET_TYPE_OFFSET + 1] = ETHERTYPE_IP & 0xff;
    memcpy(frame + ETHERNET_HEADER_BYTES, packet->data, packet->length);
#ifdef NONMATCHING
    /* this side zeroes the frame's bytes past the packet, up to length, which
     * the matching side hands the receiver as the buffer held them */
    memset(frame + ETHERNET_HEADER_BYTES + packet->length, 0,
           length - ETHERNET_HEADER_BYTES - packet->length);
#endif
    if (receive(NULL, PD_COPIED, length, frame) != NULL) {
        return -1;
    }
    return 0;
}

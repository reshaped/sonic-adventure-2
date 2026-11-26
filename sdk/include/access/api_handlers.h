/* the handlers of ACCESS's requests, which AT_apiCall runs by request code.
 * each returns a Sint32, which AT_apiCall returns as a Sint16 */
#ifndef ACCESS_API_HANDLERS_H
#define ACCESS_API_HANDLERS_H
#include <access/at_api.h>
#include <access/at_types.h>
#include <sg_xpt.h>

extern const char *const ntgTcpBuild;

struct at_option {
    Sint16 nh, option, length;
    void *value;
};
struct at_ip_open {
    Uint32 address;
    Sint16 protocol;
    AT_asr asr;
    AT_udp_receiver receiver;
};
/* a raw IP send of bufnum buffers, AT_MAX_BUFFERS - 2 at most: IP_Send and
 * DL_Send each append one */
struct at_ip_send {
    Sint16 nh;
    Sint8 bufnum;
    struct {
        Sint16 len;
        Uint8 *buf;
    } buffers[AT_REQUEST_BUFFERS];
};
struct at_arp_entry {
    Uint32 address;
    Uint8 hardware[6];
    Sint16 flags;
};
struct at_arp_list {
    Sint16 length;
    void *entries;
};

Sint32 API_SetASR(AT_setasr *request);
Sint32 API_IfConfigUp(AT_ifconfigupparam *request);
Sint32 API_IfConfigDown(AT_ifconfigdownparam *request);
Sint32 API_TCP_Open(AT_openparam *request);
Sint32 API_TCP_POpen(AT_popenparam *request);
Sint32 API_TCP_Accept(AT_acceptparam *request, Uint32 *address,
                      Uint16 *local_port, Uint16 *peer_port);
Sint32 API_TCP_Close(AT_nhparam *request);
Sint32 API_TCP_Shutdown(AT_nhparam *request);
Sint32 API_TCP_Abort(AT_nhparam *request);
Sint32 API_TCP_GetAddr(AT_nhparam *request, Uint32 *address, Uint16 *local_port,
                       Uint16 *peer_port);
Sint32 API_TCP_Stat(AT_nhparam *request, Uint16 *state, Uint16 *pending,
                    Uint16 *send_window, Uint16 *receive_window);
Sint32 API_TCP_Send(AT_sendparam *request);
Sint32 API_TCP_Sendf(AT_sendfparam *request);
Sint32 API_TCP_Receive(AT_rcvparam *request, Sint16 *length);
Sint32 API_TCP_Receivef(void *request, Sint16 *length);
Sint32 API_TCP_Cancel(AT_cancelparam *request);
Sint32 API_SetSockopt(struct at_option *request);
Sint32 API_GetSockopt(struct at_option *request);
Sint32 API_UDP_Open(AT_uopenparam *request);
Sint32 API_UDP_Send(AT_usendparam *request);
Sint32 API_IP_Open(struct at_ip_open *request);
Sint32 API_IP_Send(struct at_ip_send *request);
Sint32 API_RouteAdd(AT_addrtparam *request);
Sint32 API_RouteDel(AT_delrtparam *request);
Sint32 API_RouteList(AT_lstrtparam *request);
Sint32 API_ArpAdd(struct at_arp_entry *request);
Sint32 API_ArpDel(struct at_arp_entry *request);
Sint32 API_ArpList(struct at_arp_list *request);
Sint32 API_UdpRecvCallback(void *destination, AT_MaxBuffs *packet,
                           Sint32 length);
Sint32 API_UdpRecv(AT_MaxBuffs *packet, Uint32 source, Sint16 port,
                   AT_udp_receiver receiver, Sint16 socket);
Sint32 API_IpRecv(AT_MaxBuffs *packet, Uint32 source, AT_udp_receiver receiver,
                  int socket);
/* the bridges that pass a socket's event on to its callback. apisub.c defines
 * them with Sint16 words. api.c declares and calls them with Sint32 words under
 * AT_PROMOTED_CALLBACKS, a mismatch it tags as a bug */
#ifdef AT_PROMOTED_CALLBACKS
#define AT_CALLBACK_WORD Sint32
#else
#define AT_CALLBACK_WORD Sint16
#endif
Sint32 tcp_accept_notify(AT_CALLBACK_WORD socket, AT_accept_notify callback,
                         AT_CALLBACK_WORD child, Uint32 address,
                         Uint16 local_port, Uint16 peer_port);
Sint32 tcp_send_notify(AT_CALLBACK_WORD socket, AT_send_notify callback,
                       AT_CALLBACK_WORD result);
Sint32 tcp_sendf_notify(AT_CALLBACK_WORD socket, AT_sendf_notify callback,
                        AT_CALLBACK_WORD result, Uint8 *buffer);
Sint32 tcp_receive_notify(Sint16 socket, AT_receive_notify callback,
                          Sint16 result, AT_CALLBACK_WORD length);
#undef AT_CALLBACK_WORD
void asr_call(Sint16 socket, AT_asr callback, Sint16 event);
void udp_rcv_call(AT_udp_receiver receiver, Sint16 socket, AT_MaxBuffs *packet,
                  Sint16 length, Uint32 source, Sint16 port);
int initialize_all(int reserved);
void terminate_all(void);
#endif

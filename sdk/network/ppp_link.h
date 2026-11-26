/* the PPP link's records and calls, from the modem's line through LCP and
 * authentication to IPCP */
#ifndef NETWORK_PPP_LINK_H
#define NETWORK_PPP_LINK_H
#include <access/aveppp.h>
#include <sg_xpt.h>
#include "ppp_fsm.h"

enum {
    PPP_AUTH_PAP = 0,
    PPP_AUTH_NONE = 1,
    PPP_AUTH_CHAP = 2,
    PPP_AUTH_AUTO = 4,
    PPP_AUTH_METHOD = 0x7f,
    PPP_AUTH_PEER = 0x80,
    PPP_LINK_FAILED = -17,
    PPP_AUTH_REJECTED = -18
};
struct ppp_connection {
    Uint32 character_map;
    Uint16 address_compression, authentication;
    Sint32 magic_number;
    Uint16 mru, protocol_compression;
    char *login, *password;
};
struct ipcp_options {
    Sint16 negotiate;
    Sint32 address, peer_address;
    Sint16 compression, slots;
    Uint8 slot_compression;
    Sint32 dns1, dns2, peer_dns1, peer_dns2;
};
struct ipcp_side {
    Sint16 will_negotiate;
    struct ipcp_options want, work;
};
struct ppp_compression;
struct ppp_ipcp {
    struct ipcp_side local, remote;
    struct ppp_compression *compression;
};
extern struct ppp_ipcp cont_ipcp_s;
/* the link's statistics as PP_GetPPPStatus reports them. get_ppp_info
 * copies them into _ppp_info, whose struct ppp_statistics narrows
 * control_sent and send_status to 16 bits */
struct ppp_driver_statistics {
    Uint8 local_ip[4], peer_ip[4], local_dns1[4], local_dns2[4], peer_dns1[4],
        peer_dns2[4];
    Sint32 octets_sent, packets_sent, control_sent[PPP_PROTOCOLS];
    Sint32 send_status;
    Sint16 receive_status;
    Sint32 octets_received, packets_received;
    Uint16 control_received[PPP_PROTOCOLS];
    Uint16 unknown, discarded, overflow, receive_errors,
        receive_allocation_errors;
};
struct ppp_statistics {
    Uint8 local_ip[4], peer_ip[4], local_dns1[4], local_dns2[4], peer_dns1[4],
        peer_dns2[4];
    Sint32 octets_sent, packets_sent;
    Uint16 control_sent[PPP_PROTOCOLS];
    Sint16 send_status, receive_status;
    Sint32 octets_received, packets_received;
    Uint16 control_received[PPP_PROTOCOLS];
    Uint16 unknown, discarded, overflow, receive_errors,
        receive_allocation_errors;
};
void ppp_ready(struct ppp_session *session);
short ppp_iostatus(short status);
short ppp_send(struct ppp_buffer *packet);
short PP_GetPPPStatus(struct ppp_driver_statistics *status);
extern struct ppp_session PP_ppp_str;
short PP_pkd_ppp_up(short flags, short port, short parity, short stop,
                    short speed, short data_bits);
short PP_pkd_lcp_open(struct ppp_connection *configuration);
short PP_pkd_pap_open(char *login, char *password);
short PP_pkd_chap_open(char *login, char *password);
short PP_pkd_ipcp_standby(short vj_compression, long local_ip, long peer_ip,
                          long local_dns1, long local_dns2, long peer_dns1,
                          long peer_dns2);
short PP_pkd_ipcp_open(void);
short PP_pkd_ppp_close(void);
void PP_ComClose(void);
Uint16 PP_ComLineSence(void);
int ppp_is_abort(void);
#endif

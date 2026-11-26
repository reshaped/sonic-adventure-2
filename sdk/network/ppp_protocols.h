/* the PPP protocols' calls. LCP negotiates the link, PAP and CHAP authenticate
 * it, and IPCP configures IP over it */
#ifndef NETWORK_PPP_PROTOCOLS_H
#define NETWORK_PPP_PROTOCOLS_H

struct ipcp_side;
struct ppp_buffer;
struct ppp_connection;
struct ppp_fsm;
struct ppp_session;

void PP_lcp_init(struct ppp_session *session);
short PP_dolcp_open(struct ppp_fsm *fsm, struct ppp_connection *configuration);
short PP_lcp_monitor(struct ppp_fsm *fsm);
void lcp_opening(struct ppp_fsm *fsm);

void PP_ipcp_init(struct ppp_session *session);
short PP_doipcp_open(struct ppp_fsm *fsm);
short PP_ipcp_monitor(struct ppp_fsm *fsm);
short PP_doipcp_address(struct ipcp_side *side, long address);
short PP_doipcp_remote_address(struct ipcp_side *side, long address);
short PP_doipcp_dns1_address(struct ipcp_side *side, long address);
short PP_doipcp_dns2_address(struct ipcp_side *side, long address);
short PP_doipcp_remote_dns1_address(struct ipcp_side *side, long address);
short PP_doipcp_remote_dns2_address(struct ipcp_side *side, long address);
short PP_doipcp_compress(short disable, struct ipcp_side *side);

void PP_pap_init(struct ppp_session *session);
void PP_pap_local(struct ppp_session *session);
short PP_pap_remote(struct ppp_session *session);
void PP_pap_down(struct ppp_fsm *fsm);
void PP_pap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet);
struct ppp_buffer *PP_pap_makereq(struct ppp_fsm *fsm);
short PP_dopap_user(char *login, char *password, struct ppp_fsm *fsm);
short PP_pap_monitor(struct ppp_fsm *fsm);

void PP_chap_init(struct ppp_session *session);
short PP_chap_local(struct ppp_session *session);
short PP_chap_remote(struct ppp_session *session);
void PP_chap_proc(struct ppp_fsm *fsm, struct ppp_buffer *packet);
/* argument is CHAP's struct ppp_fsm */
short PP_dochap_user(char *login, char *secret, void *argument);
short PP_chap_monitor(struct ppp_fsm *fsm);

#endif

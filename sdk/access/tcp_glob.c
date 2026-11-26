/* ACCESS's settings and tables. ConfigBlock points at ip_addr, so that ip_addr,
 * netmask and broadcast form its AT_config. that holds only while the three lie
 * in a row, as they do in the image. under NONMATCHING, ConfigBlock points at a
 * record of its own */
#include <access/at_api.h>
#include <access/stack_globals.h>
#include <sg_xpt.h>

Uint8 rt_size = 10;
Uint8 arp_size = 10;
Uint16 tcp_win_size = 4096;
#ifdef NONMATCHING
/* this side gives ConfigBlock a record of its own, since GCC needn't lay out
 * ip_addr, netmask and broadcast in a row */
static AT_config configuration;
AT_config *ConfigBlock = &configuration;
#else
AT_config *ConfigBlock = (AT_config *)&ip_addr;
Uint32 ip_addr, netmask, broadcast;
#endif
Uint8 max_tcb_entry, max_ucb_entry, max_icb_entry;
char my_username[16], my_hostname[32];
struct tcb *tcb_table;
struct ucb *ucb_table;
struct net_icb *icb_table;
void *dphvec;
Uint8 *_streambuf;
struct memory_pool *mcb_table;
Uint8 max_mcb_entry;
Uint16 udp_port_num, tcp_port_num;

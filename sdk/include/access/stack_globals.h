/* ACCESS's settings, control tables and buffers, which tcp_glob.c defines.
 * ACCESS_LOCAL_ADDRESS, ACCESS_NETMASK and ACCESS_BROADCAST read the addresses
 * ConfigBlock points at */
#ifndef ACCESS_STACK_GLOBALS_H
#define ACCESS_STACK_GLOBALS_H
#include <access/at_api.h>
#include <sg_xpt.h>

struct tcb;
struct ucb;
struct net_icb;
struct memory_pool;

extern Uint8 rt_size, arp_size;
extern Uint16 tcp_win_size;
extern AT_config *ConfigBlock;

#ifdef NONMATCHING
/* this side reads the three through ConfigBlock, which points at a record of
 * its own under NONMATCHING (tcp_glob.c) */
#define ACCESS_LOCAL_ADDRESS (ConfigBlock->ip_addr.u32)
#define ACCESS_NETMASK (ConfigBlock->netmask.u32)
#define ACCESS_BROADCAST (ConfigBlock->broadcast.u32)
#else
extern Uint32 ip_addr, netmask, broadcast;
#define ACCESS_LOCAL_ADDRESS ip_addr
#define ACCESS_NETMASK netmask
#define ACCESS_BROADCAST broadcast
#endif
extern Uint8 max_tcb_entry, max_ucb_entry, max_icb_entry;
extern char my_username[16], my_hostname[32];
extern struct tcb *tcb_table;
extern struct ucb *ucb_table;
extern struct net_icb *icb_table;
extern void *dphvec;
extern Uint8 *_streambuf;
extern struct memory_pool *mcb_table;
extern Uint8 max_mcb_entry;
extern Uint16 udp_port_num, tcp_port_num;
#endif

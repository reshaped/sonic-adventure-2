/* the MIB-II groups ACCESS keeps. mib_init resets the IP and TCP groups,
 * ip_addr_tab and mib_addrs to their defaults. it then takes the address,
 * mask and broadcast bit of interface 0, and tcpMaxConn from max_tcb_entry */
#include <access/mib.h>
#include <access/tcp_private.h>
#include <access/datalink.h>
#include <access/ip_types.h>
#include <access/packet_queue.h>
#include <sg_xpt.h>
#include <string.h>

/* RFC 1213's ipForwarding for a host, and its tcpRtoAlgorithm for Van
 * Jacobson's */
#define MIB_NOT_FORWARDING 2
#define MIB_RTO_VANJ 4
/* the least significant bit of an address, which a Uint32 holds in network
 * order */
#define ADDRESS_LOW_BIT 0x01000000

Uint32 icmp_mib[MIB_ICMP_COUNTERS];
Uint32 udp_mib[MIB_UDP_COUNTERS];
struct ip_mib ip_mib;
struct tcp_mib tcp_mib;
struct ip_addr_entry ip_addr_tab;
union mib_address_table mib_addrs;

/* each TCPS_ state's number as RFC 1213's tcpConnState */
Uint8 mib_tcpstatemap[TCPS_TIME_WAIT + 1] = {1, 2, 3, 4, 5, 8, 6, 10, 9, 7, 11};

/* clang-format off */
static const struct ip_mib const_ip_mib = {
    MIB_NOT_FORWARDING, IP_DEFAULT_TTL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
/* clang-format on */

static const struct tcp_mib const_tcp_mib = {
    MIB_RTO_VANJ,
    TCPTV_MIN *TCP_TICK_MS,
    TCPTV_REXMTMAX *TCP_TICK_MS,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static const struct ip_addr_entry const_ip_addr_tab = {
    0, 0, 1, 0, 1, ETHERNET_MTU,
};

static void *const const_mib_addrs[10] = {
    0, &ip_mib, &ip_addr_tab, 0, 0, icmp_mib, &tcp_mib, 0, udp_mib, 0,
};

void mib_init(void)
{
    struct net_iface *entry;

    memcpy(&ip_mib, &const_ip_mib, sizeof(ip_mib));
    memcpy(&tcp_mib, &const_tcp_mib, sizeof(tcp_mib));
    memcpy(&ip_addr_tab, &const_ip_addr_tab, sizeof(ip_addr_tab));
    memcpy(mib_addrs.entries, const_mib_addrs, sizeof(mib_addrs));
    entry = DL_GetIfEntry(0);
    if (entry) {
        ip_addr_tab.ipAdEntAddr = entry->addr;
        ip_addr_tab.ipAdEntNetMask = entry->mask;
        ip_addr_tab.ipAdEntBcastAddr =
            (entry->broadcast & ADDRESS_LOW_BIT) ? 1 : 0;
    }
    tcp_mib.tcpMaxConn = max_tcb_entry;
}

/* ACCESS's counters, in MIB-II's groups. each field is the RFC 1213 object of
 * its name */
#ifndef ACCESS_MIB_H
#define ACCESS_MIB_H
#include <sg_xpt.h>

struct ip_mib {
    Uint8 ipForwarding;
    Uint8 ipDefaultTTL;
    Uint32 ipInReceives;
    Uint32 ipInHdrErrors;
    Uint32 ipInAddrErrors;
    Uint32 ipForwDatagrams;
    Uint32 ipInUnknownProtos;
    Uint32 ipInDiscards;
    Uint32 ipInDelivers;
    Uint32 ipOutRequests;
    Uint32 ipOutDiscards;
    Uint32 ipOutNoRoutes;
    Uint32 ipReasmTimeout;
    Uint32 ipReasmReqds;
    Uint32 ipReasmOKs;
    Uint32 ipReasmFails;
    Uint32 ipFragOKs;
    Uint32 ipFragFails;
    Uint32 ipFragCreates;
    Uint32 ipRoutingDiscards;
};

struct tcp_mib {
    Uint16 tcpRtoAlgorithm;
    Uint16 tcpRtoMin;
    Uint16 tcpRtoMax;
    Uint16 tcpMaxConn;
    Uint32 tcpActiveOpens;
    Uint32 tcpPassiveOpens;
    Uint32 tcpAttemptFails;
    Uint32 tcpEstabResets;
    Uint32 tcpCurrEstab;
    Uint32 tcpInSegs;
    Uint32 tcpOutSegs;
    Uint32 tcpRetransSegs;
    Uint32 tcpInErrs;
    Uint32 tcpOutRsts;
};

struct ip_addr_entry {
    Uint32 reserved;
    Uint32 ipAdEntAddr;
    Uint32 ipAdEntIfIndex;
    Uint32 ipAdEntNetMask;
    Uint32 ipAdEntBcastAddr;
    Uint32 ipAdEntReasmMaxSize;
};

/* the counters RFC 1213 gives its icmp group and its udp group */
#define MIB_ICMP_COUNTERS 26
#define MIB_UDP_COUNTERS 4
extern Uint32 icmp_mib[MIB_ICMP_COUNTERS];
extern Uint32 udp_mib[MIB_UDP_COUNTERS];
extern struct ip_mib ip_mib;
extern struct tcp_mib tcp_mib;
extern struct ip_addr_entry ip_addr_tab;
union mib_address_table {
    void *first;
    void *entries[10];
};
extern union mib_address_table mib_addrs;

extern Uint8 mib_tcpstatemap[11];
void mib_init(void);
#endif

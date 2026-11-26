/* the route table and the search for an address's route. IP_InitIfconfig
 * adds the route to the interface's network, and the application and ICMP
 * redirects add the rest */
#include <access/at_api.h>
#include <access/at_types.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/datalink.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#include <access/stack_globals.h>

/* the bytes IP_RouteList writes of an AT_RouteEntry: all but its padding */
#define ROUTE_ENTRY_BYTES 14
#define HOST_ROUTE_MASK 0xffffffff
/* the metric of a route a redirect adds */
#define REDIRECT_METRIC 1
/* the bit after class A's. a class C, D or E address has it set, and a class B
 * one clear */
#define IN_CLASSC_BIT 0x40000000

static int find_gateway(Uint32 dst);

int IP_RouteAdd(Uint32 destination, Uint32 mask, Uint32 gateway, Uint16 metric,
                Uint8 kind, Uint16 flags)
{
    int index;
    int found;
    RtEntry *rt;
    RtEntry *entry;
    int slot;

    found = 0;
    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        rt = &route_table[index];
        if (rt->dst == destination && (rt->flags & RTF_UP)) {
            if (destination == 0 && mask == 0 && gateway == 0 &&
                kind == RT_KIND_INTERFACE) {
                continue;
            }
            /* @hack spelling: the jumps to one return of -1, where a return
             * in place of each would do */
            goto error;
        }
        if (!found && rt->flags == 0) {
            slot = index;
            entry = &route_table[slot];
            found = 1;
        }
    }
    if (found != 1) {
        goto error;
    }
    entry->mask = mask;
    if (kind == RT_KIND_REDIRECT) {
        entry->flags = flags | RTF_UP;
        if (flags & RTF_HOST) {
            entry->mask = HOST_ROUTE_MASK;
        }
    } else if (kind == RT_KIND_INTERFACE) {
        entry->flags = RTF_UP | RTF_LOCAL;
    } else if (kind != RT_KIND_NETWORK && (destination & ~mask) != 0) {
        entry->flags = RTF_UP | RTF_GATEWAY | RTF_HOST | RTF_LOCAL;
        entry->mask = HOST_ROUTE_MASK;
    } else {
        entry->flags = RTF_UP | RTF_GATEWAY | RTF_LOCAL;
    }
    entry->ifno = 0;
    entry->refcnt = 0;
    entry->redirect = 0;
    entry->kind = kind;
    entry->dst = destination;
    entry->metric = metric;
    entry->gateway = gateway;
    return 0;
error:
    return -1;
}

int IP_RouteDel(Uint32 destination, Uint32 mask, Uint32 gateway, Uint8 kind)
{
    int index;
    RtEntry *rt;
    RtEntry *entry;
    int slot;

    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        rt = &route_table[index];
        if (rt->dst == destination && rt->gateway == gateway &&
            ((kind != RT_KIND_INTERFACE && rt->kind != RT_KIND_INTERFACE) ||
             (kind == RT_KIND_INTERFACE && rt->kind == RT_KIND_INTERFACE)) &&
            (rt->flags & RTF_UP)) {
            slot = index;
            entry = &route_table[slot];
            break;
        }
    }
    if (index >= IP_ROUTE_ENTRIES) {
        return -1;
    }
    entry->flags = 0;
    return 0;
}

void IP_FreeRoute(RtCache *route)
{
    RtEntry *entry;

    entry = route->rt;
    if (entry) {
        entry->refcnt--;
        if (entry->refcnt == 0 && !(entry->flags & RTF_UP)) {
            entry->flags = 0;
        }
        route->rt = 0;
    }
}

Uint16 IP_RouteList(Uint16 size, AT_Vptr *destination, Uint8 remove)
{
    AT_RouteEntry route;
    AT_delrtparam deletion;
    AT_Vptr pointer;
    void *record;
    Uint16 record_size;
    int index;
    int count;
    Uint16 rest;

    rest = size;
    if (!remove) {
        record_size = ROUTE_ENTRY_BYTES;
    } else {
        record_size = sizeof(AT_delrtparam);
    }
    count = 0;
    index = 0;
    while (rest != 0 && rest >= record_size) {
        while (index < IP_ROUTE_ENTRIES &&
               !(route_table[index].flags & RTF_UP)) {
            index++;
        }
        if (index >= IP_ROUTE_ENTRIES) {
            break;
        }
        if (!remove) {
            route.dstaddr.u32 = route_table[index].dst;
            route.gtwayaddr.u32 = route_table[index].gateway;
            route.reserve1 = route_table[index].metric;
            route.reserve2 = route_table[index].flags;
            route.reserve3 = 1;
            record = &route;
        } else {
            deletion.dstaddr.u32 = route_table[index].dst;
            deletion.gtwayaddr.u32 = route_table[index].gateway;
            record = &deletion;
        }
        OS_SetVptrLocal(&pointer, record);
        OS_Vmemcpy(destination, &pointer, record_size);
        OS_IncVptr(destination, record_size);
        count++;
        index++;
        rest -= record_size;
    }
    return count;
}

int IP_ComeIcmpRedirect(Uint32 destination, Uint32 gateway, Uint16 flags,
                        Uint32 source)
{
    int index;
    RtEntry *entry;
    Uint32 mask;

    mask =
        IP_SetNetmaskRoute(destination, ACCESS_NETMASK, ACCESS_LOCAL_ADDRESS);
    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if (!(entry->flags & RTF_UP) || entry->gateway != source) {
            continue;
        }
        entry->redirect = 1;
        if (entry->dst == 0) {
            flags = (flags & RTF_HOST) | RTF_GATEWAY;
            break;
        } else if ((entry->dst & entry->mask) != (destination & entry->mask)) {
            /* @hack spelling: the test negated twice, where an || of
             * (flags & RTF_HOST) and a != of the networks would do */
            if (!(!(flags & RTF_HOST) &&
                  (entry->dst & mask) == (entry->mask & destination))) {
                continue;
            }
        }
        if ((entry->flags & RTF_HOST) ||
            (!(entry->flags & RTF_HOST) && !(flags & RTF_HOST))) {
            entry->flags = flags | RTF_UP | RTF_GATEWAY | RTF_MODIFIED;
            entry->gateway = gateway;
            return 0;
        }
    }
    flags |= RTF_DYNAMIC;
    if (IP_RouteAdd(destination, mask, gateway, REDIRECT_METRIC,
                    RT_KIND_REDIRECT, flags)) {
        return -1;
    }
    return 0;
}

RtEntry *IP_RouteSearch(Uint32 destination)
{
    int index;

    if ((index = find_gateway(destination)) == -1) {
        return 0;
    }
    return &route_table[index];
}

RtEntry *IP_GetRouteEntry(Uint32 destination)
{
    int index;
    RtEntry *entry;

    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if ((entry->flags & RTF_UP) && (entry->flags & RTF_LOCAL) &&
            entry->dst == destination) {
            return entry;
        }
    }
    return 0;
}

Uint32 IP_GetMyaddrRoute(RtEntry *route)
{
    struct net_iface *ifp;

    if ((ifp = DL_GetIfEntry(route->ifno)) == 0) {
        return 0;
    }
    return ifp->addr;
}

int IP_GetRouteAddr(Uint32 destination, RtCache *route, struct route_info *info)
{
    int index;
    struct net_iface *ifp;
    Uint32 gateway;

    if (route && route->rt && !route->rt->redirect) {
        if (route->rt->flags & RTF_UP) {
            if (info) {
                if ((ifp = DL_GetIfEntry(route->rt->ifno)) == 0) {
                    return -1;
                }
                info->gateway = route->gateway;
                info->addr = ifp->addr;
                info->mtu = ifp->mtu;
                info->ifno = route->rt->ifno;
            }
            return 0;
        }
        IP_FreeRoute(route);
    }
    if ((index = find_gateway(destination)) == -1) {
        return -1;
    }
    if ((ifp = DL_GetIfEntry(route_table[index].ifno)) == 0) {
        return -1;
    }
    if (ifp->addr == route_table[index].gateway) {
        gateway = destination;
    } else {
        gateway = route_table[index].gateway;
    }
    if (route) {
        route->rt = &route_table[index];
        route_table[index].refcnt++;
        route->gateway = gateway;
    }
    if (info) {
        info->gateway = gateway;
        info->addr = ifp->addr;
        info->mtu = ifp->mtu;
        info->ifno = route_table[index].ifno;
    }
    return 0;
}

Uint32 IP_SetNetmask(Uint32 mask, Uint32 address)
{
    Uint32 widened;
    Uint32 host;

    widened = OS_Htonl(mask);
    host = OS_Htonl(address);
    if (host == 0) {
        return 0;
    } else if (IN_CLASSA(host)) {
        widened |= IN_CLASSA_NET;
    } else if (IN_CLASSB(host)) {
        widened |= IN_CLASSB_NET;
    } else if (IN_CLASSC(host)) {
        widened |= IN_CLASSC_NET;
    }
    return OS_Htonl(widened);
}

Uint32 IP_SetNetmaskRoute(Uint32 destination, Uint32 mask,
                          Uint32 interface_address)
{
    Uint32 host;
    Uint32 network_mask;

    host = OS_Htonl(destination);
    if (IN_CLASSA(host)) {
        network_mask = IN_CLASSA_NET;
    } else if (!(host & IN_CLASSC_BIT)) {
        network_mask = IN_CLASSB_NET;
    } else {
        network_mask = IN_CLASSC_NET;
    }
    if ((host & network_mask) == (OS_Htonl(interface_address) & network_mask)) {
        network_mask = OS_Htonl(mask);
    }
    return OS_Htonl(network_mask);
}

/* the index of dst's route, or -1. the route to the interface's network
 * comes first, and takes a broadcast, 0 and the loopback address too. then
 * come a host's route, a network's through a gateway, and the default route */
static int find_gateway(Uint32 dst)
{
    int index;
    RtEntry *entry;

    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if ((entry->flags & RTF_UP) && (entry->flags & RTF_LOCAL) &&
            (entry->dst == (entry->mask & dst) || dst == INADDR_BROADCAST ||
             dst == 0 || dst == OS_Htonl(INADDR_LOOPBACK)) &&
            entry->kind == RT_KIND_INTERFACE) {
            return index;
        }
    }
    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if ((entry->flags & RTF_UP) && (entry->flags & RTF_HOST) &&
            entry->dst == (entry->mask & dst)) {
            return index;
        }
    }
    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if ((entry->flags & RTF_UP) && (entry->flags & RTF_GATEWAY) &&
            entry->dst == (entry->mask & dst)) {
            return index;
        }
    }
    for (index = 0; index < IP_ROUTE_ENTRIES; index++) {
        entry = &route_table[index];
        if ((entry->flags & RTF_UP) && (entry->flags & RTF_GATEWAY) &&
            entry->dst == 0) {
            return index;
        }
    }
    return -1;
}

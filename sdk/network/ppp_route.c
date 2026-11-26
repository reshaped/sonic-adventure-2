#include <access/at_api.h>
#include <sg_xpt.h>
#include "ppp_route.h"

/* the interface PPP's packet driver serves, and the metric of its default
 * route */
enum { PPP_INTERFACE = 0, DEFAULT_ROUTE_METRIC = 10 };

Sint16 PP_getconfig(void)
{
    /* @hack redundant: an interface record no code reads, which sizes the
     * frame */
    AT_ifconfigupparam unused;
    AT_configptr config;
    (void)unused;
    return AT_apiCall(GETCONFIG, &config, &config);
}
Sint16 PP_ifconfig(Uint32 address, Uint32 netmask)
{
    AT_ifconfigupparam up;
    AT_ifconfigdownparam down;
    Sint16 result;
    up.if_num = PPP_INTERFACE;
    up.ip_addr.u32 = address;
    up.netmask.u32 = netmask;
    up.broadcast.u32 = 0;
    result = AT_apiCall(IFCONFIG, &up, &up);
    if (result != 0) {
        down.if_num = PPP_INTERFACE;
        AT_apiCall(IFDOWN, &down, &down);
        result = AT_apiCall(IFCONFIG, &up, &up);
    }
    return result;
}
Sint16 PP_ifconfigdown(Uint32 address, Uint32 netmask)
{
    AT_ifconfigdownparam down;
    (void)address;
    (void)netmask;
    down.if_num = PPP_INTERFACE;
    return AT_apiCall(IFDOWN, &down, &down);
}
Sint16 PP_addroute(Uint32 gateway)
{
    AT_addrtparam route;
    route.dstaddr.u32 = 0;
    route.gtwayaddr.u32 = gateway;
    route.reserve = DEFAULT_ROUTE_METRIC;
    return AT_apiCall(ROUTE_ADD, &route, &route);
}
Sint16 PP_delroute(Uint32 gateway)
{
    AT_delrtparam route;
    route.dstaddr.u32 = 0;
    route.gtwayaddr.u32 = gateway;
    return AT_apiCall(ROUTE_DEL, &route, &route);
}

/* ACCESS's interface 0, which runs over PPP, and its default route. each call
 * returns AT_apiCall's result */
#ifndef NETWORK_PPP_ROUTE_H
#define NETWORK_PPP_ROUTE_H
#include <sg_xpt.h>
/* ask ACCESS for its configuration, whose pointer the call drops */
Sint16 PP_getconfig(void);
/* bring the interface up with address and netmask. where that fails, it takes
 * the interface down and tries once more */
Sint16 PP_ifconfig(Uint32 address, Uint32 netmask);
/* take the interface down. address and netmask are unread */
Sint16 PP_ifconfigdown(Uint32 address, Uint32 netmask);
/* add and delete the default route through gateway */
Sint16 PP_addroute(Uint32 gateway);
Sint16 PP_delroute(Uint32 gateway);
#endif

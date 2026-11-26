/* the start of every layer, and the steps that bring an interface up and
 * down */
#include <access/datalink.h>
#include <access/ip_private.h>
#include <access/ip_types.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
int INIT_All(void)
{
    if (OS_Init() != 0) {
        return -1;
    }
    if (DL_Init() != 0) {
        return -1;
    }
    if (IP_Init() != 0) {
        return -1;
    }
    if (ARP_Init() != 0) {
        return -1;
    }
    if (UDP_Init() != 0) {
        return -1;
    }
    if (TCP_Init() != 0) {
        return -1;
    }
    if (MP_Init() != 0) {
        return -1;
    }
    return 0;
}

int INIT_IfconfigUp(Uint32 address, Uint32 mask, Uint32 broadcast_address,
                    int interface_number)
{
    if (DL_IfUp(address, mask, broadcast_address, interface_number) != 0) {
        return -1;
    }
    if (IP_InitIfconfig(interface_number) != 0) {
        DL_IfDown(interface_number);
        return -1;
    }
    return 0;
}

int INIT_IfconfigDown(int interface_number)
{
    IP_IfDown(interface_number);
    DL_IfDown(interface_number);
    return 0;
}

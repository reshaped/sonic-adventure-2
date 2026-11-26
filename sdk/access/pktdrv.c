/* the packet driver, the data link's one Ethernet interface. it sends frames
 * through pd_ftp_send and queues those that come in */
#include <access/at_types.h>
#include <access/datalink.h>
#include <access/ip_types.h>
#include <access/packet_queue.h>
#include <access/pktdrv.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>

static Uint8 pktdrv_send_buffer[ETHERNET_FRAME_BYTES];

Sint32 PKTDRV_Init(void)
{
    return 0;
}

Sint32 PKTDRV_CanExit(void)
{
    return 0;
}

Sint32 PKTDRV_Exit(void)
{
    return 0;
}

Sint32 PKTDRV_GetIfnum(void)
{
    return 1;
}

Sint32 PKTDRV_IfInit(Sint32 interface_number)
{
    struct net_iface *interface;

    if (!(interface = DL_GetIfEntryFirst(interface_number))) {
        return -1;
    }
    if (pd_ftp_init(interface_number) != 0) {
        return -1;
    }
    interface->kind = DL_KIND_PKTDRV;
    interface->mtu = ETHERNET_MTU;
    interface->addr = 0;
    interface->broadcast = INADDR_BROADCAST;
    interface->network = 0;
    interface->mask = 0;
    interface->htype = DL_TYPE_ETHER;
    interface->hlen = ETHERNET_ADDRESS_BYTES;
    interface->bcast[0] = -1;
    interface->bcast[1] = -1;
    interface->bcast[2] = -1;
    interface->bcast[3] = -1;
    interface->bcast[4] = -1;
    interface->bcast[5] = -1;
    interface->state = DL_IF_DOWN;
    return 0;
}

Sint32 PKTDRV_IfUp(Sint32 interface_number)
{
    struct net_iface *interface;

    if (!(interface = DL_GetIfEntryFirst(interface_number))) {
        return -1;
    }
    if (interface->state == DL_IF_UP) {
        return -1;
    }
    if (pd_ftp_open() != 0) {
        return -1;
    }
    if (pd_ftp_getaddr(interface->haddr) != 0) {
        return -1;
    }
    interface->state = DL_IF_UP;
    return 0;
}

Sint32 PKTDRV_IfDown(Sint32 interface_number)
{
    struct net_iface *interface;

    if (!(interface = DL_GetIfEntry(interface_number))) {
        return -1;
    }
    if (pd_ftp_close() != 0) {
        return -1;
    }
    interface->state = DL_IF_DOWN;
    return 0;
}

Sint32 PKTDRV_Send(AT_MaxBuffs *buffs, Sint32 interface_number)
{
    struct net_iface *interface;
    AT_Vptr destination;
    Uint8 *cursor;
    Sint32 total;
    Sint32 index;

    total = 0;
    if (!(interface = DL_GetIfEntry(interface_number))) {
        return -1;
    }
    cursor = pktdrv_send_buffer;
#ifdef NONMATCHING
    /* this side refuses a bufnum outside 0 to AT_MAX_BUFFERS, which the copy
     * below walks */
    if (buffs->bufnum < 0 || buffs->bufnum > AT_MAX_BUFFERS) {
        return -1;
    }
#endif
    for (index = buffs->bufnum; index > 0; index--) {
#ifdef NONMATCHING
        /* this side refuses a buffer of negative length, or one that would run
         * past pktdrv_send_buffer's ETHERNET_FRAME_BYTES */
        if (buffs->buffers[index - 1].len < 0 ||
            buffs->buffers[index - 1].len > ETHERNET_FRAME_BYTES - total) {
            return -1;
        }
#endif
        OS_SetVptrLocal(&destination, cursor);
        OS_Vmemcpy(&destination, &buffs->buffers[index - 1].vptr,
                   buffs->buffers[index - 1].len);
        cursor += buffs->buffers[index - 1].len;
        total += buffs->buffers[index - 1].len;
    }

    switch (interface->kind) {
    case DL_KIND_PKTDRV:
        if (pd_ftp_send(pktdrv_send_buffer, total) == 0) {
            return 0;
        }
        break;
    }
    return -1;
}

struct received_frame {
    Sint32 length;
    Sint32 interface_number;
};

Sint32 PKTDRV_Recv(Sint32 length, Sint32 interface_number)
{
    struct received_frame received;

    received.length = length;
    received.interface_number = interface_number;

    /* @hack spelling: received's fields read through its address, where
     * received.length and received.interface_number would do */
    tcp_que_regist(&tcp_que_tbl, (&received)->length,
                   (&received)->interface_number);
    return 0;
}

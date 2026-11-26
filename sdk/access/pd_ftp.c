/* the data link's packet driver: the broadband adapter's or PPP's, as
 * AT_CardType says. each frame that comes in takes a slot of tcp_que_tbl */
#include <access/cardprobe.h>
#include <access/datalink.h>
#include <access/packet_queue.h>
#include <access/pktdrv.h>
#include <sg_xpt.h>
#include <string.h>
#include "../network/packet_driver.h"
#include "../network/ppp_driver.h"
#include "../nexgen/dld_driver.h"

struct pd_driver {
    Sint32 (*init)(Sint32 mode);
    Sint32 (*driver_info)(Sint32 ifnum, struct pd_info *info);
    Sint32 (*access_type)(Sint32 if_class, Sint32 if_type, Sint32 if_number,
                          const char *type, Uint32 type_len,
                          pd_receiver receiver);
    Sint32 (*release_type)(Sint32 handle);
    Sint32 (*send_pkt)(const void *frame, Uint32 len);
    Sint32 (*terminate)(Sint32 mode);
    Sint32 (*get_address)(Sint32 handle, Uint8 *address, Sint32 *len);
    Sint32 (*reset_interface)(Sint32 handle);
    Sint32 (*as_send_pkt)(void);
    Sint32 (*set_rcv_mode)(Sint32 handle, Sint32 mode);
    Sint32 (*get_rcv_mode)(Sint32 handle, Sint32 *mode);
    Sint32 (*get_statistics)(struct pd_statistics *output);
    Sint32 (*get_parameters)(struct pd_parameters *output);
};

static struct pd_info pf_inf;
static Sint32 pf_ifnum;
static Sint32 pf_mac_len;
static Sint32 pf_hndl_arp;
static Sint32 pf_hndl_ip;
static struct pd_driver pd_disp;

/* the Ethernet types of ARP and IPv4, high byte first */
static Uint8 pf_type_arp[2] = {ETHERTYPE_ARP >> 8, ETHERTYPE_ARP & 0xff};
static Uint8 pf_type_ip[2] = {ETHERTYPE_IP >> 8, ETHERTYPE_IP & 0xff};

static void *pd_receiver1(void *driver, Sint32 status, Sint32 length,
                          void *buffer);
static void *pd_receiver2(void *driver, Sint32 status, Sint32 length,
                          void *buffer);
static void *pd_receiver_1st(void *driver, Sint32 length);
static int pd_receiver_2nd(void *driver, Sint32 length);

Sint32 pd_ftp_init(Sint32 interface_number)
{
    struct pd_driver *driver = &pd_disp;
    Sint32 (*init)(Sint32 mode);

    /* @hack redundant: unused, which only sizes the frame */
    int unused[3];
    (void)unused;

    switch (AT_CardType) {
    case AT_CARD_TYPE_LAN:
        driver->init = DLD_init;
        driver->driver_info = DLD_driver_info;
        driver->access_type = DLD_access_type;
        driver->release_type = DLD_release_type;
        driver->send_pkt = DLD_send_pkt;
        driver->terminate = DLD_terminate;
        driver->get_address = DLD_get_address;
        break;
    case AT_CARD_TYPE_PPP:
        driver->init = NULL;
        driver->driver_info = DPD_driver_info;
        driver->access_type = DPD_access_type;
        driver->release_type = DPD_release_type;
        driver->send_pkt = DPD_send_pkt;
        driver->terminate = DPD_terminate;
        driver->get_address = DPD_get_address;
        driver->reset_interface = DPD_reset_interface;
        driver->as_send_pkt = DPD_as_send_pkt;
        driver->set_rcv_mode = DPD_set_rcv_mode;
        driver->get_rcv_mode = DPD_get_rcv_mode;
        break;
    default:
        return -1;
    }
    init = driver->init;
    if (init != NULL && init(0) != 0) {
        return -1;
    }
    pf_ifnum = interface_number;
    driver->driver_info(interface_number, &pf_inf);
    return 0;
}

Sint32 pd_ftp_send(Uint8 *frame, Sint32 length)
{
    int result;

    result = pd_disp.send_pkt(frame, length);
    if (result < 0) {
        return -1;
    }
    return 0;
}

Sint32 pd_ftp_open(void)
{
    struct pd_driver *driver = &pd_disp;
    Sint32 handle;

    handle = driver->access_type(pf_inf.if_class, pf_inf.if_type,
                                 pf_inf.if_number, (char *)pf_type_arp,
                                 sizeof(pf_type_arp), pd_receiver1);
    pf_hndl_arp = handle;
    if (handle < 0) {
        return -1;
    }
    handle = driver->access_type(pf_inf.if_class, pf_inf.if_type,
                                 pf_inf.if_number, (char *)pf_type_ip,
                                 sizeof(pf_type_ip), pd_receiver2);
    pf_hndl_ip = handle;
    if (handle < 0) {
        driver->release_type(pf_hndl_arp);
        return -1;
    }
    return 0;
}

Sint32 pd_ftp_close(void)
{
    pd_disp.release_type(pf_hndl_ip);
    pd_disp.release_type(pf_hndl_arp);
    return 0;
}

Sint32 pd_ftp_getaddr(Uint8 *address)
{
    int result;

    pf_mac_len = ETHERNET_ADDRESS_BYTES;
    result = pd_disp.get_address(pf_hndl_arp, address, &pf_mac_len);
    if (result < 0) {
        return -1;
    }
    return 0;
}

static void *pd_receiver1(void *driver, Sint32 status, Sint32 length,
                          void *buffer)
{
    void *result = 0;

    if (status == PD_GET_BUFFER) {
        result = pd_receiver_1st(driver, length);
    } else {
        pd_receiver_2nd(driver, length);
    }
    return result;
}

static void *pd_receiver2(void *driver, Sint32 status, Sint32 length,
                          void *buffer)
{
    void *result = 0;

    if (status == PD_GET_BUFFER) {
        result = pd_receiver_1st(driver, length);
    } else {
        pd_receiver_2nd(driver, length);
    }
    return result;
}

static void *pd_receiver_1st(void *driver, Sint32 length)
{
    if (tcp_que_available(&tcp_que_tbl) == 0 || length > ETHERNET_FRAME_BYTES) {
        return NULL;
    }
    return tcp_que_wptr(&tcp_que_tbl)->frame;
}

static int pd_receiver_2nd(void *driver, Sint32 length)
{
    /* @hack redundant: the test of PKTDRV_Recv's result, whose two ways both
     * return 0 */
    if (PKTDRV_Recv(length, pf_ifnum) != 0) {
        return 0;
    }
    return 0;
}

Sint32 pd_ftp_terminate(void)
{
    pd_disp.terminate(0);
    return 0;
}

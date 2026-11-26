/* PPP's packet driver, which ACCESS's data link takes in place of the
 * broadband adapter's. it sends the bytes after each frame's Ethernet header
 * over PPP, whatever the frame's type. each datagram PPP receives goes to
 * handle 0's receiver, whatever type that handle registered. it arrives
 * behind an Ethernet header the driver makes up. the DPD_ calls are the packet
 * driver specification's, and a negative result is one of the PD_ errors */
#ifndef NETWORK_PPP_DRIVER_H
#define NETWORK_PPP_DRIVER_H
#include <sg_xpt.h>
#include "../network/packet_driver.h"
struct ppp_buffer;
enum {
    PD_BAD_HANDLE = -1,
    PD_NO_CLASS = -2,
    PD_NO_TYPE = -3,
    PD_NO_NUMBER = -4,
    PD_BAD_MODE = -8,
    PD_NO_SPACE = -9,
    PD_TYPE_IN_USE = -10,
    PD_BAD_COMMAND = -11,
    PD_CANNOT_RESET = -15
};
enum {
    PD_TYPE_BYTES = 8, /* the longest type DPD_access_type takes */
    PD_HANDLES = 8,
    PD_MAX_FRAME = 1514, /* the longest frame DPD_send_pkt takes */
    /* the shortest frame the driver sends, and the shortest it hands a
     * receiver. they are Ethernet's minimum frame, without its FCS and with
     * it */
    PD_MIN_SEND = 60,
    PD_MIN_RECEIVE = 64,
    /* the class and the driver type DPD_access_type takes. PD_ANY_TYPE
     * stands for any type, and PD_INTERFACES counts the interface numbers */
    PD_CLASS_ETHERNET = 1,
    PD_DRIVER_TYPE = 51,
    PD_ANY_TYPE = -1,
    PD_INTERFACES = 2,
    /* the receive modes run from the receiver off to every frame taken */
    PD_RECEIVE_OFF = 1,
    PD_RECEIVE_ALL = 6
};
/* Pd_get_parameter's record. receive_buffers is one less than the frames
 * the driver can receive back to back. send_buffers is one less than those it
 * can send. interrupt is 0 for none */
struct pd_parameters {
    Uint8 version_major, version_minor, record_length, address_length;
    Uint16 mtu, multicast_bytes, receive_buffers, send_buffers, interrupt;
};
struct pd_statistics {
    Sint32 packets_in, packets_out, bytes_in, bytes_out;
    Sint32 errors_in, errors_out, dropped;
};
Sint32 DPD_driver_info(Sint32 interface, struct pd_info *info);
/* register receive for frames of type, type_length bytes long. if_class must
 * be PD_CLASS_ETHERNET, and if_type PD_DRIVER_TYPE or PD_ANY_TYPE. if_number
 * must be below PD_INTERFACES: the SHC build reads interface_class[if_number]
 * before it tests if_number. the result is the handle, or an error.
 * PD_NO_CLASS, PD_NO_TYPE and PD_NO_NUMBER refuse those arguments.
 * PD_TYPE_IN_USE refuses a type over PD_TYPE_BYTES, or one that agrees with a
 * registered type over the shorter length. PD_BAD_HANDLE means no handle is
 * free */
Sint32 DPD_access_type(Sint32 if_class, Sint32 if_type, Sint32 if_number,
                       const char *type, Uint32 type_length,
                       pd_receiver receive);
Sint32 DPD_release_type(Sint32 handle);
/* the datagram after frame's Ethernet header goes over PPP while the
 * connection is established. otherwise the driver drops the frame and still
 * returns 0. the result is PD_NO_SPACE for a length past PD_MAX_FRAME. the
 * SHC build pads a frame under PD_MIN_SEND bytes with the bytes after it.
 * frame must then address PD_MIN_SEND bytes */
Sint32 DPD_send_pkt(const void *frame, Uint32 length);
Sint32 DPD_terminate(Sint32 mode);
/* *length is the room at address. the call fails where that room can't take
 * an Ethernet address, and it leaves *length as it is */
Sint32 DPD_get_address(Sint32 handle, Uint8 *address, Sint32 *length);
Sint32 DPD_reset_interface(Sint32 handle);
Sint32 Pd_get_parameter(struct pd_parameters *parameters);
Sint32 DPD_as_send_pkt(void);
/* the driver keeps one mode for every handle, from PD_RECEIVE_OFF to
 * PD_RECEIVE_ALL, and filters nothing by it */
Sint32 DPD_set_rcv_mode(Sint32 handle, Sint32 mode);
Sint32 DPD_get_rcv_mode(Sint32 handle, Sint32 *mode);
/* of the counts, only packets_out and bytes_out ever change */
Sint32 DPD_get_statistics(struct pd_statistics *output);
/* the serial port's bytes as they are, without PPP's framing, which the dial
 * script sends and reads. each returns 0, or -1 */
short PP_direct_pk_send(char *bytes, short length);
short PP_direct_pk_recv(char *bytes, short length);
/* the number of bytes waiting goes to *count */
short PP_direct_pk_stat(Uint16 *count);
/* hand packet, an IP datagram PPP received, to handle 0's receiver. port is
 * unread. the result is 0, or -1 where the receiver is unset, gives no buffer
 * or refuses the frame */
short PP_ip_route(struct ppp_buffer *packet, int port);
#endif

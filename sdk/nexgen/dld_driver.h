/* the packet driver of the broadband adapter, an RTL8139 behind the GAPS
 * bridge on the G2 bus */
#ifndef NEXGEN_DLD_DRIVER_H
#define NEXGEN_DLD_DRIVER_H
#include <sg_xpt.h>
#include "../network/packet_driver.h"

/* 0 if GAPS answers with its ID, else -1 */
Sint32 DLD_probe(void);
/* set up GAPS, the card and its interrupt, then run the self-test in
 * RTL_DIAG_MEMORY. it zeroes the card's packet memory first. its settings,
 * as DLD_GetCtlPara reports them, are RTL_MEDIA_10_HALF, RTL_LOOPBACK_OFF and
 * RTL_RECEIVE_BROADCAST. it returns 0, -2 if GAPS doesn't come up, or -1 if
 * the self-test fails. a failed self-test leaves the card as the test left
 * it. mode goes unread */
Sint32 DLD_init(Sint32 mode);
/* -2 while a receiver is registered. otherwise it removes the interrupt
 * handler, disables GAPS and returns 0. mode goes unread */
Sint32 DLD_terminate(Sint32 mode);
/* fill info: version 1, if_class 1, if_type 52 and if_number 0. its name is
 * dl_driver_build, and its functionality 2. it returns 0. handle goes
 * unread */
Sint32 DLD_driver_info(Sint32 handle, struct pd_info *info);
/* register receiver for the frames of the EtherType at type, high byte
 * first: ARP's or IPv4's. it returns the EtherType as the handle. it returns
 * -1 if the type has a receiver already, and -2 for any other type. the
 * driver drops a frame whose type has no receiver. if_class, if_type,
 * if_number and type_length go unread.
 *
 * the receiver runs in the adapter's interrupt, with itself as driver and
 * NULL as buffer at both calls. length counts the frame from its header to
 * its payload's end, without its CRC. a frame longer than ETHERNET_FRAME_BYTES
 * is dropped, and the receiver doesn't see it. the buffer the receiver
 * returns must start on a 4-byte boundary. the driver copies length rounded up
 * to a multiple of 4: up to 3 bytes too many */
Sint32 DLD_access_type(Sint32 if_class, Sint32 if_type, Sint32 if_number,
                       const char *type, Uint32 type_length,
                       pd_receiver receiver);
/* remove the receiver of the EtherType handle, ARP's or IPv4's, and return 0.
 * it returns -1 for any other handle */
Sint32 DLD_release_type(Sint32 handle);
/* queue the length bytes at buffer, a frame without the CRC the card adds.
 * length must be at most ETHERNET_FRAME_BYTES. it returns 0 once the frame is
 * out of buffer. it returns -2 while the link is down. it returns -1 while
 * all RTL_TX_SLOTS transmit buffers wait to go out. a frame under 60 bytes
 * goes out as 60, padded with what follows it in its transmit buffer */
Sint32 DLD_send_pkt(const void *buffer, Uint32 length);
/* copy the card's MAC address from its IDR registers into the
 * ETHERNET_ADDRESS_BYTES at address. *length gets ETHERNET_ADDRESS_BYTES. it
 * returns 0. handle goes unread */
Sint32 DLD_get_address(Sint32 handle, Uint8 *address, Sint32 *length);
/* write the ETHERNET_ADDRESS_BYTES at address into the EEPROM by
 * DLD_SetEtdAddr. it returns 0, or -1 if that fails. it returns -2, and writes
 * nothing, unless length is ETHERNET_ADDRESS_BYTES */
Sint32 DLD_set_address(Uint8 *address, Sint32 length);
#endif

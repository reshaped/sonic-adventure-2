/* a packet driver calls a receiver twice for each frame. with status
 * PD_GET_BUFFER, the receiver returns a buffer for length bytes, or NULL to
 * drop the frame. with PD_COPIED, the frame is in that buffer */
#ifndef NETWORK_PACKET_DRIVER_H
#define NETWORK_PACKET_DRIVER_H
#include <sg_xpt.h>

#define PD_GET_BUFFER 0
#define PD_COPIED 1
typedef void *(*pd_receiver)(void *driver, Sint32 status, Sint32 length,
                             void *buffer);
struct pd_info {
    Sint32 version;
    Uint8 if_class, if_type, if_number;
    char *name;
    Uint8 functionality;
};
#endif

/* the head position in a frame's subcode, which gdDecSubcode decodes for the
 * file system */
#ifndef SUBCODE_POSITION_H
#define SUBCODE_POSITION_H
#include <sg_xpt.h>

/* a head position, from a frame's subcode Q channel. minute, second and frame
 * are the time within the track */
struct subcode_position {
    Uint32 track;
    Uint32 minute;
    Uint32 second;
    Uint32 frame;
    /* the Q channel's control byte in bits 24 to 31, above the absolute time
     * in frames */
    Uint32 disc_frame;
    Uint32 index;
};

/* decode the head position from subcode into position, and return 0.
 * subcode is a frame's whole subcode, as the drive's subcode request returns
 * it. the call returns -1 where the subcode's first byte isn't 0. it returns
 * -2 where the size the subcode gives isn't 100 bytes. it returns -5 where the
 * Q channel holds no position, and -3 where the channel fails its CRC. each
 * failure leaves position as it was */
Sint32 gdDecSubcode(Sint8 *subcode, struct subcode_position *position);

#endif

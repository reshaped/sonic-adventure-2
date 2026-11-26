/* PPP's packets. a struct ppp_buffer heads each packet's frame. data points at
 * the bytes not yet read, and length counts them */
#ifndef NETWORK_PPP_PACKET_H
#define NETWORK_PPP_PACKET_H
#include <sg_xpt.h>

#define PPP_FRAME_DATA_BYTES 2048
#define PPP_SHORT_FRAME_BYTES 128
/* the room before each frame's payload for the headers PP_pushdown puts ahead
 * of it */
#define PPP_HEADROOM 144

struct ppp_buffer {
    Uint8 *data;
    Uint16 length;
    Uint16 reserved;
};
/* the work frame at index, 0 or 1, emptied. NULL for any other index */
struct ppp_buffer *PP_alloc_mbuf_work(Uint16 index);
short PP_len_p(const struct ppp_buffer *packet);
void PP_trim_mbuf(struct ppp_buffer **cursor, short length);
/* take up to length bytes off the front of *cursor into destination, or drop
 * them where destination is NULL. it returns how many it took, which is no
 * more than *cursor holds: a packet is one frame. cursor and *cursor may be
 * NULL, which takes none */
short PP_pullup(struct ppp_buffer **cursor, Uint8 *destination, Uint16 length);
struct ppp_buffer *PP_pushdown(struct ppp_buffer *packet, short length);
/* a copy of length bytes from source, in the one short frame each call
 * reuses. length must be PPP_SHORT_FRAME_BYTES or less, which only the GCC
 * build checks */
struct ppp_buffer *PP_qdata(const Uint8 *source, Uint16 length);
Uint16 PP_dqdata(const struct ppp_buffer *packet, Uint8 *destination,
                 Uint16 length);
Uint8 *PP_put16(Uint8 *destination, Uint16 value);
Uint8 *PP_put32(Uint8 *destination, Uint32 value);
Uint16 PP_get16(const Uint8 *source);
Uint32 PP_get32(const Uint8 *source);
/* the next byte, 16-bit word or 32-bit word of *packet, pulled up into the
 * includer's pull_byte or pull_word. PPP_PULL_BYTE and PPP_PULL16 are -1, and
 * PPP_PULL32 0, where too few bytes are left. each PPP member defines that
 * scratch, whether or not its code pulls */
#define PPP_PULL_BYTE(packet)                                                  \
    (PP_pullup((packet), &pull_byte, 1) != 1 ? -1 : pull_byte)
#define PPP_PULL16(packet)                                                     \
    (PP_pullup((packet), pull_word, 2) != 2 ? -1 : PP_get16(pull_word))
#define PPP_PULL32(packet)                                                     \
    (PP_pullup((packet), pull_word, 4) != 4 ? 0 : PP_get32(pull_word))
#ifdef NONMATCHING
#else
/* PPP_PULL_BYTE's byte, taken straight off the cursor while more than one
 * byte remains */
#define pull_octet(packet)                                                     \
    ((packet) && *(packet) && (*(packet))->length > 1                          \
         ? ((*(packet))->length--, (int)*(*(packet))->data++)                  \
         : (PP_pullup((packet), &pull_byte, 1) != 1 ? -1 : (int)pull_byte))
#endif
#endif

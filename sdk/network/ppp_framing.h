/* PPP's frames, the protocols they carry, and Van Jacobson's compression of
 * TCP/IP headers, which IPCP negotiates */
#ifndef NETWORK_PPP_FRAMING_H
#define NETWORK_PPP_FRAMING_H
#include <sg_xpt.h>
struct ppp_buffer;
struct ppp_compression;
/* PP_slhc_compress's results. VJ_PLAIN_IP leaves the datagram as it was.
 * VJ_FULL_HEADER keeps its whole header, with its slot number in IP's
 * protocol field. VJ_DELTA_HEADER compresses its header */
enum { VJ_PLAIN_IP = 0x40, VJ_FULL_HEADER = 0x70, VJ_DELTA_HEADER = 0x80 };
/* a frame's protocol field, by the protocol it carries */
enum {
    PPP_IPV4 = 0x21,
    PPP_VJ_COMPRESSED = 0x2d,
    PPP_VJ_UNCOMPRESSED = 0x2f,
    PPP_IPCP_PROTOCOL = 0x8021,
    PPP_LCP_PROTOCOL = 0xc021,
    PPP_PAP_PROTOCOL = 0xc023,
    PPP_CHAP_PROTOCOL = 0xc223
};
void DumpPacket(const char *name, Uint16 length, const Uint8 *data);
extern const Uint16 PP_fcstab[256];
/* the most connection slots each way */
enum { VJ_SLOTS = 16 };
/* the one compression state, set up for receive_count and send_count slots.
 * each count runs from 0, for no compression that way, to VJ_SLOTS */
struct ppp_compression *PP_slhc_init(short receive_count, short send_count);
void PP_slhc_free(struct ppp_compression *state);
/* a nonzero compress_slot leaves out the slot number where it repeats the
 * last one sent. the result is a VJ_ result, or -1 */
short PP_slhc_compress(struct ppp_compression *state,
                       struct ppp_buffer **packet, short compress_slot);
/* PP_slhc_uncompress rebuilds the datagram of a compressed frame, and
 * PP_slhc_remember takes in an uncompressed frame's. each returns the
 * datagram's length, or 0 where it refuses the frame */
short PP_slhc_uncompress(struct ppp_compression *state,
                         struct ppp_buffer **packet);
short PP_slhc_remember(struct ppp_compression *state,
                       struct ppp_buffer **packet);
/* refuse each compressed frame that doesn't name its slot, until one does or
 * an uncompressed frame comes. the result is 0 */
short PP_slhc_toss(struct ppp_compression *state);
/* hand a received frame, less its FCS, to the protocol its protocol field
 * names. a frame of a protocol not above goes back to the peer in an LCP
 * Protocol-Reject */
void ppp_proc(struct ppp_buffer *packet);
#endif

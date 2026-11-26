/* IP and TCP headers in host order, which Van Jacobson compression takes off
 * a datagram and puts back */
#ifndef NETWORK_PPP_IP_H
#define NETWORK_PPP_IP_H
#include <sg_xpt.h>

#define PPP_IP_HEADER_BYTES 20
#define PPP_TCP_HEADER_BYTES 20
/* the most option bytes an IP or a TCP header holds */
#define PPP_OPTION_BYTES 40

struct ppp_buffer;
/* fragment_offset counts bytes */
struct ppp_ip {
    Uint32 source, destination;
    Uint16 length, identification, fragment_offset, checksum;
    Uint8 congested, dont_fragment, more_fragments;
    Uint8 version, service, ttl, protocol, option_length;
    Uint8 options[PPP_OPTION_BYTES];
};
struct ppp_pseudo_header {
    Uint32 source, destination;
    Uint8 protocol;
    Uint16 length;
};
struct ppp_tcp {
    Uint16 source, destination;
    Uint32 sequence, acknowledgement;
    Uint16 window, checksum, urgent_pointer, maximum_segment;
    struct {
        Uint8 congested, urgent, ack, push, reset, syn, fin;
    } flags;
    Uint8 option_length;
    Uint8 options[PPP_OPTION_BYTES];
};
Uint16 Htons(Uint16 value);
Uint32 Htonl(Uint32 value);
/* put ip's header ahead of buffer's data, which must have room for it, and
 * return buffer. checksum_valid zero computes the checksum, and any other
 * value writes ip->checksum */
struct ppp_buffer *PP_htonip(const struct ppp_ip *ip, struct ppp_buffer *buffer,
                             Sint16 checksum_valid);
/* take an IP header and its options off *buffer, and cut the rest to the
 * datagram's length. the result is the header's length, or 0xffff where the
 * header is cut short or too short */
Uint16 PP_ntohip(struct ppp_ip *ip, struct ppp_buffer **buffer);
/* the Internet checksum of buffer's first length bytes, or of all it holds
 * where that is less. where pseudo isn't NULL, the sum starts with its 12
 * bytes as they lie in memory. its fields must be in network order, and the
 * pad byte after protocol 0 */
Uint16 PP_cksum(const struct ppp_pseudo_header *pseudo,
                const struct ppp_buffer *buffer, Uint16 length);
/* put tcp's header ahead of buffer's data. a NULL pseudo writes
 * tcp->checksum, and any other computes the checksum over pseudo and the
 * segment's pseudo->length bytes. the result is the buffer, or NULL without
 * room */
struct ppp_buffer *PP_htontcp(const struct ppp_tcp *tcp,
                              struct ppp_buffer *buffer,
                              const struct ppp_pseudo_header *pseudo);
/* take a TCP header and its options off *buffer. the result is the header's
 * length, or 0xffff where it is cut short or too short */
Uint16 PP_ntohtcp(struct ppp_tcp *tcp, struct ppp_buffer **buffer);
#endif

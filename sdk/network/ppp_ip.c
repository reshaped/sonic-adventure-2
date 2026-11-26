#include <sg_xpt.h>
#include <string.h>
#include "ppp_ip.h"
#include "ppp_packet.h"

#define IP_MORE_FRAGMENTS 0x2000
#define IP_DONT_FRAGMENT 0x4000
#define IP_OFFSET_MASK 0x1fff
#define IP_STAGING_OPTIONS 44
#define IP_STAGING_BYTES (PPP_IP_HEADER_BYTES + IP_STAGING_OPTIONS)
#define IP_ERROR ((Uint16) - 1)
#define PSEUDO_HEADER_WORDS 6
/* version_length holds the version in its high four bits. its low four hold
 * the header's length in 32-bit words. the fragment field counts 8-byte
 * units */
#define IP_VERSION_SHIFT 4
#define IP_NIBBLE 15
#define IP_WORDS(bytes) ((bytes) >> 2)
#define IP_BYTES(words) ((words) << 2)
#define FRAGMENT_UNITS(bytes) ((bytes) >> 3)
#define FRAGMENT_BYTES(units) ((units) << 3)

/* an IP header in network order, without its options */
struct ip_wire {
    Uint8 version_length, service;
    Uint16 length, identification, fragment;
    Uint8 ttl, protocol;
    Uint16 checksum;
    Uint32 source, destination;
};
/* PP_htonip's header, with room for 4 more option bytes than an IP header
 * holds */
struct ip_packet {
    struct ip_wire header;
    Uint8 options[IP_STAGING_OPTIONS];
};

Uint16 Htons(Uint16 value)
{
    return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
}
Uint32 Htonl(Uint32 value)
{
    return ((value & 0xff) << 24) | ((value & 0xff00) << 8) |
           ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
}
struct ppp_buffer *PP_htonip(const struct ppp_ip *ip, struct ppp_buffer *buffer,
                             Sint16 checksum_valid)
{
    struct ip_packet packet;
    /* @hack spelling: the header written through wire, where packet itself
     * would do */
    struct ip_packet *wire = &packet;
    Uint16 length;
    Uint16 fragment;
    Uint8 *data;
#ifdef NONMATCHING
    /* this side refuses a missing header, and options past the header's room or
     * off a word */
    if (!ip || ip->option_length > sizeof(ip->options) ||
        (ip->option_length & 3)) {
        return NULL;
    }
#endif
    length = PPP_IP_HEADER_BYTES + ip->option_length;
    if (length > IP_STAGING_BYTES) {
        length = IP_STAGING_BYTES;
    }
    buffer = PP_pushdown(buffer, length);
#ifdef NONMATCHING
    /* this side returns the NULL PP_pushdown gives where the buffer has no
     * room */
    if (!buffer) {
        return NULL;
    }
#endif
    data = buffer->data;
    wire->header.version_length =
        (ip->version << IP_VERSION_SHIFT) | IP_WORDS(length);
    wire->header.service = ip->service;
    wire->header.length = Htons(ip->length);
    wire->header.identification = Htons(ip->identification);
    fragment = FRAGMENT_UNITS(ip->fragment_offset);
    if (ip->dont_fragment) {
        fragment |= IP_DONT_FRAGMENT;
    }
    if (ip->more_fragments) {
        fragment |= IP_MORE_FRAGMENTS;
    }
    wire->header.fragment = Htons(fragment);
    wire->header.ttl = ip->ttl;
    wire->header.protocol = ip->protocol;
    if (checksum_valid) {
        wire->header.checksum = Htons(ip->checksum);
    } else {
        wire->header.checksum = 0;
    }
    wire->header.source = Htonl(ip->source);
    wire->header.destination = Htonl(ip->destination);
    if (ip->option_length) {
        memcpy(wire->options, ip->options,
               ip->option_length < IP_STAGING_OPTIONS ? ip->option_length
                                                      : IP_STAGING_OPTIONS);
    }
    memcpy(data, wire, length);
    if (!checksum_valid) {
        wire->header.checksum = Htons(PP_cksum(NULL, buffer, length));
    }
    memcpy(data, wire, length);
    return buffer;
}
Uint16 PP_ntohip(struct ppp_ip *ip, struct ppp_buffer **buffer)
{
    struct ip_wire header;
#ifdef NONMATCHING
    /* this side takes a missing buffer as NULL, which the test below refuses */
    struct ppp_buffer *original = buffer ? *buffer : NULL;
#else
    struct ppp_buffer *original = *buffer;
#endif
    /* @hack spelling: the header read through wire, where header itself would
     * do */
    struct ip_wire *wire = &header;
    Uint16 fragment;
    Uint16 length;
#ifdef NONMATCHING
    /* this side refuses a missing header or buffer */
    if (!ip || !original) {
        return IP_ERROR;
    }
#endif
    if (PP_pullup(buffer, (Uint8 *)&header, PPP_IP_HEADER_BYTES) !=
        PPP_IP_HEADER_BYTES) {
        return IP_ERROR;
    }
    ip->version = (wire->version_length >> IP_VERSION_SHIFT) & IP_NIBBLE;
    ip->service = wire->service;
    ip->length = Htons(wire->length);
    ip->identification = Htons(wire->identification);
    /* @hack spelling: the field read into fragment before its swap, where
     * Htons(wire->fragment) would do */
    fragment = wire->fragment;
    fragment = Htons(fragment);
    ip->fragment_offset = FRAGMENT_BYTES(fragment & IP_OFFSET_MASK);
    if (fragment & IP_MORE_FRAGMENTS) {
        ip->more_fragments = 1;
    } else {
        ip->more_fragments = 0;
    }
    if (fragment & IP_DONT_FRAGMENT) {
        ip->dont_fragment = 1;
    } else {
        ip->dont_fragment = 0;
    }
    ip->ttl = wire->ttl;
    ip->protocol = wire->protocol;
    ip->checksum = Htons(wire->checksum);
    ip->source = Htonl(wire->source);
    ip->destination = Htonl(wire->destination);
#ifdef NONMATCHING
    /* this side refuses a length shorter than the IP header, from which the
     * subtraction below would wrap */
    if (ip->length < PPP_IP_HEADER_BYTES) {
        return IP_ERROR;
    }
#endif
    if (ip->length - PPP_IP_HEADER_BYTES < original->length) {
        original->length = ip->length - PPP_IP_HEADER_BYTES;
    }
    length = IP_BYTES(wire->version_length & IP_NIBBLE);
    if (length < PPP_IP_HEADER_BYTES) {
        ip->option_length = 0;
        return IP_ERROR;
    }
    ip->option_length = length - PPP_IP_HEADER_BYTES;
    if (ip->option_length) {
        if (PP_pullup(buffer, ip->options, ip->option_length) <
            ip->option_length) {
            return IP_ERROR;
        }
    }
    return length;
}
static Uint32 sum_buffer(Uint32 sum, const struct ppp_buffer *buffer,
                         Uint16 length)
{
    const Uint8 *data = buffer->data;
#ifdef NONMATCHING
    /* this side counts in an int, where a Sint16 count goes negative for a
     * length past 32767 */
    int count = buffer->length < length ? buffer->length : length;
#else
    Sint16 count = buffer->length < length ? buffer->length : length;
#endif
    Uint16 last;
    int odd;

    if (count & 1) {
        last = data[count - 1];
        odd = 1;
        count--;
    } else {
        odd = 0;
        last = 0;
    }
    count >>= 1;
    while (count > 0) {
        sum += *data++ << 8;
        sum += *data++;
        count--;
    }
    if (odd) {
        sum += last << 8;
    }
    return sum;
}
Uint16 PP_cksum(const struct ppp_pseudo_header *pseudo,
                const struct ppp_buffer *buffer, Uint16 length)
{
    Uint32 sum = 0;
    const Uint8 *data;
    int i;
    if (pseudo) {
        data = (const Uint8 *)pseudo;
        for (i = 0; i < PSEUDO_HEADER_WORDS; i++) {
            sum += *data++ << 8;
            sum += *data++;
        }
    }
    sum = sum_buffer(sum, buffer, length);
    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;
    return (Uint16)~sum;
}

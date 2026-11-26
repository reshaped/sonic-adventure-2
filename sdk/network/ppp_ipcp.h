/* IPCP's option types, its protocol, and the options each side starts from */
#ifndef NETWORK_PPP_IPCP_H
#define NETWORK_PPP_IPCP_H
#include "ppp_link.h"

/* IPCP's option types, then the bounds IPCP_OPTION_INDEX compares them with.
 * the bounds share the types' enum, since GCC warns of a comparison between
 * constants of two enums */
enum {
    IPCP_ADDRESSES = 1,
    IPCP_COMPRESS = 2,
    IPCP_ADDRESS = 3,
    IPCP_DNS1 = 0x81,
    IPCP_DNS2 = 0x83,
    IPCP_OWN_TYPES = 8,
    IPCP_HIGH_TYPES = 0x80,
    IPCP_HIGH_END = 0x90
};

enum { IPCP_OPTION_BITS = 16 };
/* an option type's bit in a negotiate mask, which is also its index in
 * ipcp_option_lengths. a type below IPCP_OWN_TYPES takes its own. one from
 * IPCP_HIGH_TYPES up to IPCP_HIGH_END takes the type less their difference.
 * of those, only 0x80 to 0x87 fall among a mask's IPCP_OPTION_BITS. any
 * other type takes 0 */
#define IPCP_OPTION_INDEX(type)                                                \
    ((type) < IPCP_OWN_TYPES ? (type)                                          \
     : (type) >= IPCP_HIGH_TYPES && (type) < IPCP_HIGH_END                     \
         ? (type) - (IPCP_HIGH_TYPES - IPCP_OWN_TYPES)                         \
         : 0)
#define IPCP_MASK(type) (1 << IPCP_OPTION_INDEX(type))
enum {
    IPCP_ADDRESS_BIT = IPCP_MASK(IPCP_ADDRESS),
    IPCP_COMPRESS_BIT = IPCP_MASK(IPCP_COMPRESS),
    IPCP_DNS1_BIT = IPCP_MASK(IPCP_DNS1),
    IPCP_DNS2_BIT = IPCP_MASK(IPCP_DNS2)
};

extern struct ppp_protocol ipcp_protocol;
extern struct ipcp_options ipcp_default_options;
extern Sint16 ipcp_default_negotiation;
/* each option's length, by the option's bit */
extern Uint8 ipcp_option_lengths[IPCP_OPTION_BITS];
#ifdef NONMATCHING
/* an IPCP_ADDRESSES option's length, and every other option's */
enum { IPCP_ADDRESSES_LENGTH = 10, IPCP_OPTION_LENGTH = 6 };
extern ppp_event ipcp_release, ipcp_reset, ipcp_starting, ipcp_opening,
    ipcp_closing, ipcp_stopping;
extern ppp_check ipcp_request, ipcp_ack, ipcp_nak, ipcp_reject;
/* append the option type, with its values from settings, to *packet. where
 * *packet is NULL, it takes work frame 0. length must be the type's
 * IPCP_ADDRESSES_LENGTH or IPCP_OPTION_LENGTH. IPCP_COMPRESS with a protocol
 * other than VJ's takes its last two bytes from parameters, which may be NULL.
 * a byte parameters doesn't hold is 0xff. a failure makes *packet NULL: a type
 * it doesn't know, another length or a full frame */
void ipcp_emit_option(struct ppp_buffer **packet,
                      const struct ipcp_options *settings, Uint8 type,
                      Uint8 length, struct ppp_buffer **parameters);
struct ppp_buffer *ipcp_make_request(struct ppp_fsm *fsm);
/* append each option mask holds, in the order of its bits, as
 * ipcp_emit_option does */
void ipcp_write_options(struct ppp_buffer **packet,
                        const struct ipcp_options *settings, Sint16 mask);
#endif
#endif

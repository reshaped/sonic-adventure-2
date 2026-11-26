/* LCP's options: their types, their lengths and their bits in a negotiate
 * mask. the GCC side declares the calls that set each side's options */
#ifndef NETWORK_PPP_LCP_H
#define NETWORK_PPP_LCP_H
#include <sg_xpt.h>
enum {
    LCP_MRU = 1,
    LCP_ACCM,
    LCP_AUTHENTICATION,
    LCP_ENCRYPTION,
    LCP_MAGIC,
    LCP_QUALITY,
    LCP_PROTOCOL_COMPRESSION,
    LCP_ADDRESS_COMPRESSION,
    LCP_OPTION_TYPES
};
/* the authentication option's length for PAP, and for CHAP, which adds its
 * algorithm */
enum { LCP_PAP_OPTION_BYTES = 4, LCP_CHAP_OPTION_BYTES = 5 };
/* the least and the most MRU this end takes, and CHAP's algorithm number for
 * MD5 */
enum { LCP_MIN_MRU = 128, LCP_MAX_MRU = 4096, CHAP_MD5 = 5 };
/* an option type's bit in a negotiate mask */
#define LCP_MASK(option) (1U << (option))
/* the map lcp_set_accm takes for no ACCM */
#define LCP_NO_ACCM (-1)

struct ppp_buffer;
struct ppp_fsm;
struct ppp_lcp_options;
struct ppp_lcp_side;
/* each option's length, by type */
extern Uint8 lcp_option_lengths[LCP_OPTION_TYPES];
#ifdef NONMATCHING
/* append the option type, with its value from settings, to *packet. where
 * *packet is NULL, it takes work frame 1. length must be the type's in
 * lcp_option_lengths. CHAP's authentication option takes one byte more, for
 * its algorithm, and parameters is unread. a failure makes *packet NULL: a
 * type past LCP_ADDRESS_COMPRESSION, another length, an authentication
 * protocol but PAP or CHAP, or a full frame */
void lcp_emit_option(struct ppp_buffer **packet,
                     const struct ppp_lcp_options *settings, Uint8 type,
                     Uint8 length, struct ppp_buffer **parameters);
/* append each option mask holds, in type order, as lcp_emit_option does */
void lcp_write_options(struct ppp_buffer **packet,
                       const struct ppp_lcp_options *settings, Uint16 mask);
struct ppp_buffer *lcp_make_request(struct ppp_fsm *fsm);

/* set what side asks for, or for the compressions, what it takes. map
 * LCP_NO_ACCM asks for no ACCM, and magic_number 0 for no magic number. a
 * nonzero disable refuses that compression. lcp_set_authentication takes
 * PPP_AUTH_NONE, PPP_AUTH_PAP, PPP_AUTH_CHAP or PPP_AUTH_AUTO, and returns 1
 * for any other. lcp_set_mru returns -1 for an mru outside LCP_MIN_MRU to
 * LCP_MAX_MRU. each returns 0 otherwise */
short lcp_set_accm(struct ppp_lcp_side *side, long map);
void lcp_set_address_compression(struct ppp_lcp_side *side, short disable);
short lcp_set_authentication(struct ppp_lcp_side *side, short method);
short lcp_set_magic(struct ppp_lcp_side *side, long magic_number);
short lcp_set_mru(struct ppp_lcp_side *side, short mru);
short lcp_set_protocol_compression(struct ppp_lcp_side *side, short disable);
void lcp_stopping(struct ppp_fsm *fsm);
void lcp_closing(struct ppp_fsm *fsm);
#endif
#endif

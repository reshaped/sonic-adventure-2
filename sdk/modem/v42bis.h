/* V.42bis compression for LAPM. the coder takes the host's bytes into TM, the
 * transmit ring. the decoder takes the bytes of RR, the receive ring, to the
 * host */
#ifndef MODEM_V42BIS_H
#define MODEM_V42BIS_H
#include <sg_xpt.h>

/* run the coder for encode set, or the decoder for encode 0, where p0 lets it
 * run.
 *
 * the coder codes nothing unless TM has more than TM_ROOM bytes free. it codes
 * nothing either while tx_max_packet_size or more coded bytes wait there for
 * LAPM's frames. it stops once that many wait, or once the host has no more
 * bytes. it takes no new buffer from the host while a break from the host
 * waits.
 *
 * the decoder starts only where the host has a whole buffer free. it stops
 * where RR is empty, or where the host has no buffer free after one it sends.
 * it also stops once it has read more than tx_max_packet_size bytes of RR as
 * it sends a buffer. a break from the line that waits at RR's read point stops
 * it too. a call that finds that break there sends it to the host instead */
void v42bis(char encode);
/* send the peer the command that resets its decoder, and reset the coder. a
 * coder in compressed mode first flushes the string in progress, if any, and
 * returns to transparent mode. the reset empties the dictionary, and leaves
 * the coder in transparent mode with an escape character of 0 */
void reset_v42bis_coder(void);
/* reset both dictionaries for dictionary_size codewords, N2, and strings of
 * up to string_limit bytes, N7. dictionary_size must lie from 512, V.42bis's
 * least, to V42BIS_MAX_NODES, and string_limit must be at most
 * COM_BUFFER_SIZE - 2. directions, V.42bis's P0, goes unread. v42bis takes the
 * directions from p0. with encryption set, the cipher starts over too */
void raz(Uint16 directions, Uint16 dictionary_size, Uint16 string_limit);

#endif

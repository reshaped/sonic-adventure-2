/* SA2DTP's cipher, which XORs each byte with the next of a key stream. the
 * stream is the CIPHER_WORDS words packet_cipher_seed sets, in turn and over
 * again, each word low byte first */
#ifndef CHAOSTGKINDER_PACKET_CIPHER_H
#define CHAOSTGKINDER_PACKET_CIPHER_H
#include <sg_xpt.h>

#define CIPHER_WORDS 55
/* index is the next word's in state. word holds the bytes_left bytes of the
 * last word yet to use, low byte first */
struct packet_cipher {
    int index, bytes_left;
    Uint32 word;
    Uint32 state[CIPHER_WORDS];
};

void packet_cipher_seed(struct packet_cipher *cipher, Uint32 seed);
void packet_cipher_apply(struct packet_cipher *cipher, Uint8 *buffer,
                         int count);
#endif

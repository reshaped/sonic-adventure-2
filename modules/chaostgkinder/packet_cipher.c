#include <sg_xpt.h>
#include "chaostgkinder/packet_cipher.h"

/* the state's words are a ring. the seed sets them in turn CIPHER_STRIDE
 * apart round it. a mix takes from each word in index order the word
 * CIPHER_LAG before it round the ring. for each of the first CIPHER_LAG
 * words, that word is CIPHER_LEAD after it */
#define CIPHER_STRIDE 21
#define CIPHER_LAG 24
/* @hack spelling: CIPHER_LEAD written as 31, where CIPHER_WORDS - CIPHER_LAG
 * would do */
#define CIPHER_LEAD 31
static void cipher_mix(struct packet_cipher *cipher)
{
    Uint32 *next;
    Uint32 *earlier;
    int i;
    for (i = 0; i < CIPHER_LAG; i++) {
        cipher->state[i] -= cipher->state[i + CIPHER_LEAD];
    }
    earlier = cipher->state;
    for (next = cipher->state + CIPHER_LAG; next < cipher->state + CIPHER_WORDS;
         next++) {
        *next -= *earlier++;
    }
}

void packet_cipher_seed(struct packet_cipher *cipher, Uint32 seed)
{
    Uint32 value = 1;
    Uint32 previous;
    int position = 0;
    int i;
    for (i = 0; i < CIPHER_WORDS; i++) {
        position += CIPHER_STRIDE;
        if (position >= CIPHER_WORDS) {
            position -= CIPHER_WORDS;
        }
        cipher->state[position] = value;
        previous = value;
        value = seed - value;
        seed = previous;
    }
    cipher_mix(cipher);
    cipher_mix(cipher);
    cipher_mix(cipher);
    cipher_mix(cipher);
    cipher->index = 0;
    cipher->bytes_left = 0;
}

static Uint32 cipher_next_word(struct packet_cipher *cipher)
{
    Uint32 word = cipher->state[cipher->index];
    if (++cipher->index == CIPHER_WORDS) {
        cipher->index = 0;
    }
    return word;
}

static Uint8 cipher_next_byte(struct packet_cipher *cipher)
{
    Uint8 byte;
    if (--cipher->bytes_left < 0) {
        cipher->bytes_left = (int)sizeof(cipher->word) - 1;
        cipher->word = cipher_next_word(cipher);
    }
    byte = (Uint8)cipher->word;
    cipher->word >>= 8;
    return byte;
}

void packet_cipher_apply(struct packet_cipher *cipher, Uint8 *buffer, int count)
{
    while (--count >= 0) {
        *buffer++ ^= cipher_next_byte(cipher);
    }
}

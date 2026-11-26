#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/network.h"

static const char alphabet[] =
    "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890+-";

/* the three bytes at id[from] as four characters of alphabet at out[to].
 * the characters take the bytes' little-endian 24 bits six at a time, low
 * bits first */
#define ENCODE_TRIPLE(out, id, to, from)                                       \
    do {                                                                       \
        (out)[to] = alphabet[(id)[from] & 0x3f];                               \
        (out)[(to) + 1] =                                                      \
            alphabet[((id)[from] >> 6) | (((id)[(from) + 1] << 2) & 0x3c)];    \
        (out)[(to) + 2] = alphabet[((id)[(from) + 1] >> 4) |                   \
                                   (((id)[(from) + 2] << 4) & 0x30)];          \
        (out)[(to) + 3] = alphabet[(id)[(from) + 2] >> 2];                     \
    } while (0)

/* the id's first and second triples, each as four characters, and the
 * characters' end */
#define FIRST_TRIPLE 0
#define SECOND_TRIPLE 3
#define TRIPLE_CHARACTERS 4
#define ID_CHARACTERS 8

void kinder_console_id(char *text)
{
    Uint8 id[SYD_CFG_IID_SIZE];
    memset(id, 0, sizeof(id));
    syCfgGetIndividualID((Sint8 *)id);
    ENCODE_TRIPLE(text, id, 0, FIRST_TRIPLE);
    ENCODE_TRIPLE(text, id, TRIPLE_CHARACTERS, SECOND_TRIPLE);
    text[ID_CHARACTERS] = 0;
}

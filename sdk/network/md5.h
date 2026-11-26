/* MD5, as RFC 1321 defines it, with which CHAP hashes its challenges and
 * responses */
#ifndef NETWORK_MD5_H
#define NETWORK_MD5_H
#include <sg_xpt.h>

#define MD5_DIGEST_BYTES 16
#define MD5_BLOCK_BYTES 64

typedef struct {
    Uint32 state[4];
    Uint32 count[2]; /* the bits taken so far, low word first */
    Uint8 buffer[MD5_BLOCK_BYTES];
} MD5_CTX;

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, const Uint8 *input, unsigned int length);
/* write the digest of the bytes context took, then clear context */
void MD5Final(Uint8 digest[MD5_DIGEST_BYTES], MD5_CTX *context);
/* the digest of text, in a buffer the next call overwrites */
Uint8 *MDString(const char *text);
#endif

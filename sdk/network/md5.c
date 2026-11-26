#include <sg_xpt.h>
#include <string.h>
#include "md5.h"

/* where in its last block MD5Final puts the message's length */
#define MD5_LENGTH_OFFSET 56
static Uint8 padding_block[MD5_BLOCK_BYTES] = {0x80};
/* MDString's digest, in the first MD5_DIGEST_BYTES */
static Uint8 string_digest[MD5_BLOCK_BYTES];

static void transform(Uint32 state[4], const Uint8 block[MD5_BLOCK_BYTES]);
static void encode(Uint8 *bytes, const Uint32 *words, unsigned int length);
static void decode(Uint32 *words, const Uint8 *bytes, unsigned int length);
static void copy_bytes(Uint8 *destination, const Uint8 *source,
                       unsigned int length);
static void fill_bytes(Uint8 *destination, int value, unsigned int length);

void MD5Init(MD5_CTX *context)
{
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}
void MD5Update(MD5_CTX *context, const Uint8 *input, unsigned int length)
{
    unsigned int consumed;
    unsigned int used;
    unsigned int remaining;

    used = (context->count[0] >> 3) & (MD5_BLOCK_BYTES - 1);
    if ((context->count[0] += (Uint32)length << 3) < ((Uint32)length << 3)) {
        context->count[1]++;
    }
    context->count[1] += (Uint32)length >> 29;
    remaining = MD5_BLOCK_BYTES - used;
    if (length >= remaining) {
        copy_bytes(&context->buffer[used], input, remaining);
        transform(context->state, context->buffer);
#ifdef NONMATCHING
        /* this side tests the bytes left, where the matching side's sum can
         * wrap for a length within MD5_BLOCK_BYTES of the type's top */
        for (consumed = remaining; length - consumed >= MD5_BLOCK_BYTES;
             consumed += MD5_BLOCK_BYTES) {
#else
        for (consumed = remaining; consumed + (MD5_BLOCK_BYTES - 1) < length;
             consumed += MD5_BLOCK_BYTES) {
#endif
            transform(context->state, &input[consumed]);
        }
        used = 0;
    } else {
        consumed = 0;
    }
    copy_bytes(&context->buffer[used], &input[consumed], length - consumed);
}
void MD5Final(Uint8 digest[MD5_DIGEST_BYTES], MD5_CTX *context)
{
    Uint8 encoded_count[8];
    unsigned int used;
    unsigned int padding;

    encode(encoded_count, context->count, sizeof(encoded_count));
    used = (context->count[0] >> 3) & (MD5_BLOCK_BYTES - 1);
    padding = used < MD5_LENGTH_OFFSET
                  ? MD5_LENGTH_OFFSET - used
                  : MD5_LENGTH_OFFSET + MD5_BLOCK_BYTES - used;
    MD5Update(context, padding_block, padding);
    MD5Update(context, encoded_count, sizeof(encoded_count));
    encode(digest, context->state, MD5_DIGEST_BYTES);
    fill_bytes((Uint8 *)context, 0, sizeof(*context));
}

/* RFC 1321's F, G, H and I, the functions of the four rounds */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define STEP(a, b, mix, index, rotation, constant)                             \
    {                                                                          \
        (a) += (mix) + words[index] + (Uint32)(constant);                      \
        (a) = ((a) << (rotation)) | ((a) >> (32 - (rotation)));                \
        (a) += (b);                                                            \
    }

static void transform(Uint32 state[4], const Uint8 block[MD5_BLOCK_BYTES])
{
    Uint32 a = state[0];
    Uint32 b = state[1];
    Uint32 c = state[2];
    Uint32 d = state[3];
    Uint32 words[16];

    decode(words, block, MD5_BLOCK_BYTES);
    STEP(a, b, F(b, c, d), 0, 7, 0xd76aa478);
    STEP(d, a, F(a, b, c), 1, 12, 0xe8c7b756);
    STEP(c, d, F(d, a, b), 2, 17, 0x242070db);
    STEP(b, c, F(c, d, a), 3, 22, 0xc1bdceee);
    STEP(a, b, F(b, c, d), 4, 7, 0xf57c0faf);
    STEP(d, a, F(a, b, c), 5, 12, 0x4787c62a);
    STEP(c, d, F(d, a, b), 6, 17, 0xa8304613);
    STEP(b, c, F(c, d, a), 7, 22, 0xfd469501);
    STEP(a, b, F(b, c, d), 8, 7, 0x698098d8);
    STEP(d, a, F(a, b, c), 9, 12, 0x8b44f7af);
    STEP(c, d, F(d, a, b), 10, 17, 0xffff5bb1);
    STEP(b, c, F(c, d, a), 11, 22, 0x895cd7be);
    STEP(a, b, F(b, c, d), 12, 7, 0x6b901122);
    STEP(d, a, F(a, b, c), 13, 12, 0xfd987193);
    STEP(c, d, F(d, a, b), 14, 17, 0xa679438e);
    STEP(b, c, F(c, d, a), 15, 22, 0x49b40821);

    STEP(a, b, G(b, c, d), 1, 5, 0xf61e2562);
    STEP(d, a, G(a, b, c), 6, 9, 0xc040b340);
    STEP(c, d, G(d, a, b), 11, 14, 0x265e5a51);
    STEP(b, c, G(c, d, a), 0, 20, 0xe9b6c7aa);
    STEP(a, b, G(b, c, d), 5, 5, 0xd62f105d);
    STEP(d, a, G(a, b, c), 10, 9, 0x02441453);
    STEP(c, d, G(d, a, b), 15, 14, 0xd8a1e681);
    STEP(b, c, G(c, d, a), 4, 20, 0xe7d3fbc8);
    STEP(a, b, G(b, c, d), 9, 5, 0x21e1cde6);
    STEP(d, a, G(a, b, c), 14, 9, 0xc33707d6);
    STEP(c, d, G(d, a, b), 3, 14, 0xf4d50d87);
    STEP(b, c, G(c, d, a), 8, 20, 0x455a14ed);
    STEP(a, b, G(b, c, d), 13, 5, 0xa9e3e905);
    STEP(d, a, G(a, b, c), 2, 9, 0xfcefa3f8);
    STEP(c, d, G(d, a, b), 7, 14, 0x676f02d9);
    STEP(b, c, G(c, d, a), 12, 20, 0x8d2a4c8a);

    STEP(a, b, H(b, c, d), 5, 4, 0xfffa3942);
    STEP(d, a, H(a, b, c), 8, 11, 0x8771f681);
    STEP(c, d, H(d, a, b), 11, 16, 0x6d9d6122);
    STEP(b, c, H(c, d, a), 14, 23, 0xfde5380c);
    STEP(a, b, H(b, c, d), 1, 4, 0xa4beea44);
    STEP(d, a, H(a, b, c), 4, 11, 0x4bdecfa9);
    STEP(c, d, H(d, a, b), 7, 16, 0xf6bb4b60);
    STEP(b, c, H(c, d, a), 10, 23, 0xbebfbc70);
    STEP(a, b, H(b, c, d), 13, 4, 0x289b7ec6);
    STEP(d, a, H(a, b, c), 0, 11, 0xeaa127fa);
    STEP(c, d, H(d, a, b), 3, 16, 0xd4ef3085);
    STEP(b, c, H(c, d, a), 6, 23, 0x04881d05);
    STEP(a, b, H(b, c, d), 9, 4, 0xd9d4d039);
    STEP(d, a, H(a, b, c), 12, 11, 0xe6db99e5);
    STEP(c, d, H(d, a, b), 15, 16, 0x1fa27cf8);
    STEP(b, c, H(c, d, a), 2, 23, 0xc4ac5665);

    STEP(a, b, I(b, c, d), 0, 6, 0xf4292244);
    STEP(d, a, I(a, b, c), 7, 10, 0x432aff97);
    STEP(c, d, I(d, a, b), 14, 15, 0xab9423a7);
    STEP(b, c, I(c, d, a), 5, 21, 0xfc93a039);
    STEP(a, b, I(b, c, d), 12, 6, 0x655b59c3);
    STEP(d, a, I(a, b, c), 3, 10, 0x8f0ccc92);
    STEP(c, d, I(d, a, b), 10, 15, 0xffeff47d);
    STEP(b, c, I(c, d, a), 1, 21, 0x85845dd1);
    STEP(a, b, I(b, c, d), 8, 6, 0x6fa87e4f);
    STEP(d, a, I(a, b, c), 15, 10, 0xfe2ce6e0);
    STEP(c, d, I(d, a, b), 6, 15, 0xa3014314);
    STEP(b, c, I(c, d, a), 13, 21, 0x4e0811a1);
    STEP(a, b, I(b, c, d), 4, 6, 0xf7537e82);
    STEP(d, a, I(a, b, c), 11, 10, 0xbd3af235);
    STEP(c, d, I(d, a, b), 2, 15, 0x2ad7d2bb);
    STEP(b, c, I(c, d, a), 9, 21, 0xeb86d391);
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    fill_bytes((Uint8 *)words, 0, sizeof(words));
}
static void encode(Uint8 *bytes, const Uint32 *words, unsigned int length)
{
    unsigned int word;
    unsigned int byte;
    for (word = 0, byte = 0; byte < length; word++, byte += 4) {
        bytes[byte] = (Uint8)words[word];
        bytes[byte + 1] = (Uint8)(words[word] >> 8);
        bytes[byte + 2] = (Uint8)(words[word] >> 16);
        bytes[byte + 3] = (Uint8)(words[word] >> 24);
    }
}
static void decode(Uint32 *words, const Uint8 *bytes, unsigned int length)
{
    unsigned int word;
    unsigned int byte;
    for (word = 0, byte = 0; byte < length; word++, byte += 4) {
        words[word] = (Uint32)bytes[byte] | ((Uint32)bytes[byte + 1] << 8) |
                      ((Uint32)bytes[byte + 2] << 16) |
                      ((Uint32)bytes[byte + 3] << 24);
    }
}
static void copy_bytes(Uint8 *destination, const Uint8 *source,
                       unsigned int length)
{
    unsigned int i;
    for (i = 0; i < length; i++) {
        destination[i] = source[i];
    }
}
static void fill_bytes(Uint8 *destination, int value, unsigned int length)
{
    unsigned int i;
    for (i = 0; i < length; i++) {
        destination[i] = (Uint8)value;
    }
}
Uint8 *MDString(const char *text)
{
    MD5_CTX context;
    unsigned int length = strlen(text);
    MD5Init(&context);
    MD5Update(&context, (const Uint8 *)text, length);
    MD5Final(string_digest, &context);
    return string_digest;
}

#include <assert.h>
#include <string.h>

#include "system/prs.h"

static void check_stream(const Uint8 *stream, Uint32 size, const char *text)
{
    Uint8 output[20];
    Uint8 before[sizeof(output)];
    Uint32 consumed, written, i;
    Uint32 length = (Uint32)strlen(text);

    memset(output, '!', sizeof(output));
    memcpy(before, output, sizeof(output));
    for (i = 0; i < size; i++) {
        consumed = written = 1;
        assert(!prs_decompress_checked(stream, i, output + 1, length, &consumed,
                                       &written));
        assert(consumed == 0 && written == 0);
        assert(!memcmp(output, before, sizeof(output)));
    }
    if (length != 0) {
        assert(!prs_decompress_checked(stream, size, output + 1, length - 1,
                                       &consumed, &written));
        assert(!memcmp(output, before, sizeof(output)));
    }
    assert(prs_decompress_checked(stream, size, NULL, length, &consumed,
                                  &written));
    assert(consumed == size && written == length);
    assert(prs_decompress_checked(stream, size, output + 1, length, &consumed,
                                  &written));
    assert(consumed == size && written == length);
    assert(output[0] == '!' && output[1 + length] == '!');
    assert(!memcmp(output + 1, text, length));
}

static void check_in_place(void)
{
    /* "abcdef", a copy of 256 bytes from 1 back, then 'g'. after the copy the
     * stream has read 10 bytes and decoded 262. its output is never further
     * ahead of its input, so decoding in place needs an offset of 252. 'g'
     * then reads a second control byte, which a decode at offset 251 has
     * already overwritten. measured after that read or only at the end
     * marker, the output is just 251 ahead, which would allow that decode */
    static const Uint8 stream[] = {0xbf, 'a',  'b',  'c',  'd', 'e', 'f',
                                   0xf8, 0xff, 0xff, 0x05, 'g', 0,   0};
    Uint8 buffer[252 + sizeof(stream)], before[sizeof(buffer)];
    Uint32 consumed, written, i;

    memset(buffer, '!', sizeof(buffer));
    memcpy(buffer + 251, stream, sizeof(stream));
    memcpy(before, buffer, sizeof(buffer));
    assert(prs_decompress_in_place(buffer, sizeof(buffer), 251, sizeof(stream),
                                   &consumed, &written) == PRS_OVERRUN);
    assert(consumed == sizeof(stream) && written == 263);
    assert(!memcmp(before, buffer, sizeof(buffer)));
    consumed = written = 1;
    assert(prs_decompress_in_place(buffer, sizeof(buffer), 251,
                                   sizeof(stream) - 1, &consumed,
                                   &written) == PRS_INVALID);
    assert(consumed == 0 && written == 0);
    consumed = written = 1;
    assert(prs_decompress_in_place(buffer, sizeof(buffer), (Uint32)-1, 1,
                                   &consumed, &written) == PRS_INVALID);
    assert(consumed == 0 && written == 0);
    consumed = written = 1;
    assert(prs_decompress_in_place(buffer, sizeof(buffer), 251, (Uint32)-1,
                                   &consumed, &written) == PRS_INVALID);
    assert(consumed == 0 && written == 0);
    assert(!memcmp(before, buffer, sizeof(buffer)));

    memcpy(buffer + 252, stream, sizeof(stream));
    assert(prs_decompress_in_place(buffer, sizeof(buffer), 252, sizeof(stream),
                                   &consumed, &written) == PRS_DECODED);
    assert(consumed == sizeof(stream) && written == 263);
    assert(!memcmp(buffer, "abcdef", 6) && buffer[262] == 'g');
    for (i = 6; i < 262; i++) {
        assert(buffer[i] == 'f');
    }
}

int main(void)
{
    static const Uint8 empty[] = {2, 0, 0};
    static const Uint8 short_copy[] = {0xb3, 'a', 'b', 0xfe, 0, 0};
    static const Uint8 long_copies[] = {0x55, 'x', 0xf9, 0xff, 0xf8,
                                        0xff, 4,   0,    0};
    static const Uint8 control_reload[] = {0xff, 'a', 'b', 'c', 'd', 'e',
                                           'f',  'g', 'h', 2,   0,   0};
    static const Uint8 no_history[] = {0, 0xff, 0};
    Uint8 adjacent[10] = {5, 'z', 0, 0, '!', '!', '!', '!', '!', '!'};
    Uint8 before[sizeof(adjacent)];
    Uint32 consumed, written;

    check_in_place();
    check_stream(empty, sizeof(empty), "");
    check_stream(short_copy, sizeof(short_copy), "abababa");
    check_stream(long_copies, sizeof(long_copies), "xxxxxxxxx");
    check_stream(control_reload, sizeof(control_reload), "abcdefgh");
    memcpy(before, adjacent, sizeof(before));
    assert(!prs_decompress_checked(no_history, sizeof(no_history), adjacent,
                                   sizeof(adjacent), NULL, NULL));
    assert(!prs_decompress_checked(adjacent, sizeof(adjacent), adjacent + 2, 1,
                                   NULL, NULL));
    assert(!memcmp(before, adjacent, sizeof(adjacent)));
    assert(prs_decompress_checked(adjacent, sizeof(adjacent), adjacent + 4, 1,
                                  &consumed, &written));
    assert(consumed == 4 && written == 1 && adjacent[4] == 'z');
    return 0;
}

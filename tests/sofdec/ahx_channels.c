/* ahx_read_header on headers of two and three channels, in a buffer of
 * MPEG_STREAM_HEADER_MAX bytes as mpeg_read_stream_header gives it. two
 * channels fill both of info's records. three are refused, and nothing past
 * info->channel changes */
#include <assert.h>
#include <string.h>

#include "ahx_header.c"

#define BUFFER_BYTES 512
#define FILL 0x5a

/* a header of channels channels without loops, each channel's out_vol
 * present and set to its index plus 1 */
static void header(Uint8 *buffer, Sint32 channels)
{
    Uint8 *p = buffer;
    Sint32 length;
    Sint32 i;

    memset(buffer, 0, BUFFER_BYTES);
    length = COPYRIGHT_BYTES + FORMAT_BYTES + LOOP_COUNT_BYTES +
             channels * CHANNEL_BYTES;
    *p++ = ADX_MARK >> 8;
    *p++ = ADX_MARK & 0xff;
    *p++ = length >> 8;
    *p++ = length & 0xff;
    p[3] = channels;
    p += FORMAT_BYTES + LOOP_COUNT_BYTES;
    for (i = 0; i < channels; i++) {
        p[i * CHANNEL_BYTES + 1] = 1;
        p[i * CHANNEL_BYTES + 3] = i + 1;
    }
}

int main(void)
{
    Uint8 buffer[BUFFER_BYTES];
    Uint8 unread[sizeof(((struct mpeg_stream_info *)0)->unk_44)];
    struct mpeg_stream_info info;
    Sint32 length;

    header(buffer, 2);
    memset(&info, FILL, sizeof(info));
    assert(ahx_read_header(buffer, BUFFER_BYTES, &length, &info) == 0);
    assert(info.channels == 2);
    assert(info.channel[0].out_vol == 1);
    assert(info.channel[1].out_vol == 2);
    header(buffer, 3);
    memset(&info, FILL, sizeof(info));
    memset(unread, FILL, sizeof(unread));
    assert(ahx_read_header(buffer, BUFFER_BYTES, &length, &info) == -2);
    assert(memcmp(info.unk_44, unread, sizeof(unread)) == 0);
    return 0;
}

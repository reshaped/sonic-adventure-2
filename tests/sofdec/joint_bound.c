/* mpeg_set_frame's joint stereo bound for each layer but II, whose table
 * this harness lacks. layer I keeps the image's row, the next layer's. layer
 * III and the reserved layer take sblimit, and read nothing past jsb_table */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mpeg_audio.c"

/* the layer II table, which no frame here takes */
Sint32 mpeg_read_bit_alloc(Sint32 table, MPEG_ALLOC_TABLE **alloc)
{
    (void)table;
    (void)alloc;
    abort();
}

/* the bound of a joint stereo frame of layer and mode_extension */
static Sint32 bound(Sint32 layer, Sint32 mode_extension)
{
    struct mpeg_header hdr;
    struct mpeg_frame fr;

    memset(&hdr, 0, sizeof(hdr));
    memset(&fr, 0, sizeof(fr));
    hdr.version = 1;
    hdr.layer = layer;
    hdr.mode = MODE_JOINT;
    hdr.mode_extension = mode_extension;
    mpeg_set_frame(&hdr, &fr);
    assert(fr.channels == NCH_STEREO);
    return fr.jsbound;
}

int main(void)
{
    Sint32 extension;

    for (extension = 0; extension < 4; extension++) {
        assert(bound(1, extension) == jsb_table[1][extension]);
        assert(bound(3, extension) == SBLIMIT);
        assert(bound(LAYER_CODES, extension) == SBLIMIT);
    }
    return 0;
}

/* the ADX header preceding an AHX stream */
#ifndef AHX_HEADER_H
#define AHX_HEADER_H
#include <sg_xpt.h>

#define AHX_HEADER_LEAD 4
#define OPTION_DRCT_LVL 0
#define OPTION_FXCH 1
#define OPTION_FXLEV 2
#define OPTION_PAN 3

struct mpeg_option {
    Sint8 present;
    Sint8 value;
};
/* a channel's settings. ahx_read_header writes a value only where its
 * present field, or out_vol_present, is positive. a pan is 8 times the
 * mixer's setting */
struct mpeg_channel {
    Sint16 out_vol_present;
    Sint16 out_vol;
    struct mpeg_option option[4];
    Sint32 unk_0c;
};

struct ahx_loop {
    Sint16 number;
    Sint32 start_sample;
    Sint32 start_offset;
    Sint32 end_sample;
    Sint32 end_offset;
};

struct mpeg_stream_info {
    Sint8 encoding;
    Sint8 block_bytes;
    Sint8 sample_bits;
    Sint8 channels;
    Sint32 sample_rate;
    Sint32 total_samples;
    Sint16 cutoff;
    Sint16 loop_count;
    struct ahx_loop loop;
    struct mpeg_channel channel[2];
    Sint8 unk_44[28];
};

/* read the ADX header at buffer, size bytes long, that precedes an AHX stream.
 * where length isn't NULL, *length gets the header's bytes after its first
 * AHX_HEADER_LEAD. it gets 0 where the call returns -1 or -4. with info NULL,
 * only those first bytes are read. otherwise it fills *info. the header must
 * then have 18 bytes readable after its loop count, loop or no loop. it must
 * have 2 channels or fewer.
 *
 * it returns 0 where it read the header. it returns -1 where size is under
 * AHX_HEADER_LEAD, and -4 where the header doesn't start with 0x8000. it
 * returns -2 where the fields run past size or the header's length. it returns
 * -3 where the loops run past its length */
Sint32 ahx_read_header(const Uint8 *buffer, Sint32 size, Sint32 *length,
                       struct mpeg_stream_info *info);

#endif

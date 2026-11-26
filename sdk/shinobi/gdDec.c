/* the head position in a frame's subcode Q channel */
#include <subcode_position.h>
#include <sg_xpt.h>

/* the drive's subcode is a SUBCODE_HEADER-byte header, then a byte for each
 * of a frame's SUBCODE_SYMBOLS symbols. the header's two bytes from
 * SUBCODE_SIZE_AT hold the subcode's size, high byte first. a channel takes
 * one bit of every symbol, CHANNEL_SIZE bytes in all */
#define SUBCODE_HEADER 4
#define SUBCODE_SIZE_AT 2
#define SUBCODE_SYMBOLS 96
#define SUBCODE_SIZE (SUBCODE_HEADER + SUBCODE_SYMBOLS)
#define SYMBOL_Q_BIT 6
#define SYMBOL_Q (1 << SYMBOL_Q_BIT)
#define CHANNEL_SIZE 12

/* the Q channel's bytes. control and mode come first, then track and index.
 * the time within the track follows, then a zero and the absolute time. a
 * CRC ends the channel */
#define Q_CONTROL 0
#define Q_TRACK 1
#define Q_INDEX 2
#define Q_MINUTE 3
#define Q_SECOND 4
#define Q_FRAME 5
#define Q_DISC_TIME 7
#define Q_CRC 10
/* the mode, in the control byte's low four bits. mode 1 carries a
 * position */
#define Q_MODE_MASK 0x0f
#define Q_MODE_POSITION 1

#define SECONDS_PER_MINUTE 60
#define FRAMES_PER_SECOND 75

/* gdDecSubcode's failures */
#define BAD_HEADER (-1)
#define BAD_SIZE (-2)
#define BAD_CRC (-3)
#define NO_POSITION (-5)

/* crc_update's table: entry n is n times x^16, modulo the polynomial 0x1021 */
static const Uint16 crc_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108,
    0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
    0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b,
    0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee,
    0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
    0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d,
    0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5,
    0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
    0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4,
    0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
    0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13,
    0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
    0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e,
    0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1,
    0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
    0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0,
    0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657,
    0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
    0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882,
    0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e,
    0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
    0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d,
    0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

/* the CRC-16 of size bytes at data, from seed */
static Uint16 crc_update(Uint32 seed, Sint8 *data, Sint32 size)
{
    Uint32 crc = seed;
    /* @hack spelling: byte, a copy of data to walk, where data itself would
     * do */
    Sint8 *byte = data;

    while (size-- > 0) {
        crc = (((Uint16)crc << 8) & 0xff00) ^
              (Sint16)crc_table[(Uint8)((Uint16)crc >> 8) ^ (Uint8)*byte++];
    }
    return crc;
}

/* check crc against the Q channel at data, which stores the CRC-16 of its
 * first Q_CRC bytes inverted. it returns 0 when crc matches, and -1
 * otherwise */
static Sint32 crc_check(Sint8 *data, Uint16 crc)
{
    /* @hack spelling: actual, the inverted CRC through a local of its own,
     * where one conditional expression would do */
    Uint16 actual = ~crc_update(0, data, Q_CRC);

    if (crc != actual) {
        return -1;
    }
    return 0;
}

static Sint32 decode_bcd(Sint8 value)
{
    return (((value & 0xf0) >> 4) * 10) + (value & 0x0f);
}

/* a BCD minute, second and frame, as a count of frames */
static Sint32 time_to_frames(Sint8 *time)
{
    Sint32 minute;
    Sint32 second;
    Sint32 frame;

    minute = decode_bcd(*time++);
    second = decode_bcd(*time++);
    frame = decode_bcd(*time);
    return (((minute * SECONDS_PER_MINUTE) + second) * FRAMES_PER_SECOND) +
           frame;
}

Sint32 gdDecSubcode(Sint8 *subcode, struct subcode_position *position)
{
    struct {
        Sint8 p[CHANNEL_SIZE];
        Sint8 q[CHANNEL_SIZE];
    } channels;
    Sint8 *symbol;
    int i;
    int bit;

    if (subcode[0] != 0) {
        return BAD_HEADER;
    }
    if ((subcode[SUBCODE_SIZE_AT] * 256) + subcode[SUBCODE_SIZE_AT + 1] !=
        SUBCODE_SIZE) {
        return BAD_SIZE;
    }
    symbol = subcode + SUBCODE_HEADER;
    for (i = 0; i < CHANNEL_SIZE; i++) {
        /* the P channel is only cleared: the loop gathers the Q bit alone */
        channels.p[i] = 0;
        channels.q[i] = 0;
        for (bit = 7; bit >= 0; bit--) {
            if (bit == 7) {
                channels.q[i] |= (*symbol++ & SYMBOL_Q) << (7 - SYMBOL_Q_BIT);
            } else {
                channels.q[i] |= (*symbol++ & SYMBOL_Q) >> (SYMBOL_Q_BIT - bit);
            }
        }
        if ((channels.q[Q_CONTROL] & Q_MODE_MASK) != Q_MODE_POSITION) {
            return NO_POSITION;
        }
    }
    if (crc_check(channels.q, ((Uint8)channels.q[Q_CRC] << 8) |
                                  (Uint8)channels.q[Q_CRC + 1])) {
        return BAD_CRC;
    }
    position->track = decode_bcd(channels.q[Q_TRACK]);
    position->minute = decode_bcd(channels.q[Q_MINUTE]);
    position->second = decode_bcd(channels.q[Q_SECOND]);
    position->frame = decode_bcd(channels.q[Q_FRAME]);
    position->disc_frame = ((Uint32)channels.q[Q_CONTROL] << 24) |
                           time_to_frames(channels.q + Q_DISC_TIME);
    position->index = decode_bcd(channels.q[Q_INDEX]);
    return 0;
}

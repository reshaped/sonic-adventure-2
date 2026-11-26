/* Exercise xid's length checks on the non-matching side: it takes no
 * parameter of a group that runs past its frame, and none past an HDLC
 * options parameter longer than hdlc. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "v42.c"

/* the frame's address and control octets, ahead of the XID */
#define FRAME_HEADER_BYTES 2
/* a group length past what each frame below holds */
#define LONG_GROUP 255
/* an HDLC options parameter's octets of value, one more than hdlc holds */
#define LONG_HDLC_BYTES 5

/* room past each frame for what xid reads without its checks */
static Uint8 frame[FRAME_HEADER_BYTES + 2 * LONG_GROUP];

void raz(Uint16 directions, Uint16 dictionary_size, Uint16 string_limit)
{
    (void)directions;
    (void)dictionary_size;
    (void)string_limit;
    abort(); /* xid returns before it takes a V.42bis group. */
}

void setup_timer(Uint32 *deadline, Uint32 ticks)
{
    (void)deadline;
    (void)ticks;
    abort(); /* xid returns before it answers. */
}

/* hand xid a frame whose XID is the first length octets of xid_octets */
static void take(const Uint8 *xid_octets, int length)
{
    memset(frame, 0, sizeof(frame));
    memcpy(frame + FRAME_HEADER_BYTES, xid_octets, length);
    dce_in_pntr = frame;
    dce_in_cntr = FRAME_HEADER_BYTES + length;
    rm_read_temp = frame + FRAME_HEADER_BYTES;
    rm_frame_length = dce_in_cntr - 1;
    xid();
}

/* an HDLC options parameter longer than hdlc leaves it as it was */
static void check_long_hdlc_options(void)
{
    static const Uint8 offered[] = {XID_FORMAT,
                                    XID_NEGOTIATION,
                                    0,
                                    XID_PARAMETER_BYTES(LONG_HDLC_BYTES),
                                    XID_HDLC_OPTIONS,
                                    LONG_HDLC_BYTES,
                                    1,
                                    2,
                                    3,
                                    4,
                                    5};
    static const Uint8 kept[sizeof(hdlc)] = {9, 9, 9, 9};
    memcpy(hdlc, kept, sizeof(hdlc));
    take(offered, sizeof(offered));
    assert(!memcmp(hdlc, kept, sizeof(hdlc)));
}

/* a negotiation group longer than its frame sets no window */
static void check_long_negotiation_group(void)
{
    static const Uint8 offered[] = {
        XID_FORMAT,          XID_NEGOTIATION,  0, LONG_GROUP,
        XID_TRANSMIT_WINDOW, XID_WINDOW_BYTES, 5};
    win_k_rx = 99;
    take(offered, sizeof(offered));
    assert(win_k_rx == 99);
}

/* a V.42bis group longer than its frame sets no dictionary size. the frame
 * runs past 35 octets, as xid requires of one with that group */
static void check_long_v42bis_group(void)
{
    Uint8 offered[40] = {XID_FORMAT,
                         XID_NEGOTIATION,
                         0,
                         0,
                         XID_V42BIS_GROUP,
                         0,
                         LONG_GROUP,
                         XID_DICTIONARY_SIZE,
                         XID_DICTIONARY_BYTES,
                         0x08,
                         0x00};
    p1_init = 0xffff;
    p1 = 7;
    take(offered, sizeof(offered));
    assert(p1 == 7);
}

int main(void)
{
    check_long_hdlc_options();
    check_long_negotiation_group();
    check_long_v42bis_group();
    return 0;
}

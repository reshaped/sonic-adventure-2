/* SA2DTP's packets, whose fields are little-endian. a packet's header is
 * two halfwords: its size in bytes, header included, then its kind. every
 * service shares the kinds of enum packet_kind. kinds from 16 are each
 * service's own */
#ifndef CHAOSTGKINDER_PACKET_H
#define CHAOSTGKINDER_PACKET_H
#include <sg_xpt.h>

#define PACKET_HEADER_SIZE 4
/* frame is a packet's undecoded bytes. PACKET_FRAME_HALFWORD is the
 * halfword at frame[at]. PACKET_FRAME_SIZE and PACKET_FRAME_KIND are the
 * header's size and kind */
#define PACKET_FRAME_HALFWORD(frame, at)                                       \
    ((frame)[(at)] | ((frame)[(at) + 1] << 8))
#define PACKET_FRAME_SIZE(frame) PACKET_FRAME_HALFWORD(frame, 0)
#define PACKET_FRAME_KIND(frame) PACKET_FRAME_HALFWORD(frame, 2)
enum packet_kind {
    PACKET_EMPTY,
    PACKET_DISCONNECT,
    PACKET_NOTICE,
    PACKET_PING,
    PACKET_PONG,
    PACKET_GREETING
};

struct kinder_packet;
/* a kind's reader and writer. each takes the packet's bytes at cursor,
 * header first, and returns the cursor past them */
struct packet_codec {
    Uint8 *(*read)(struct kinder_packet *, Uint8 *);
    Uint8 *(*write)(struct kinder_packet *, Uint8 *);
};

struct kinder_packet {
    Uint16 size;
    int kind;
    const struct packet_codec *codec;
};

/* the bytes of a packet's text field */
#define PACKET_TEXT_SIZE 128

/* a ping, or the pong that answers it with its cookie */
struct ping_packet {
    struct kinder_packet header;
    Uint32 cookie;
};

/* the server's greeting: the seeds of the receive cipher and the send
 * cipher, and its text */
struct greeting_packet {
    struct kinder_packet header;
    Uint32 receive_seed, send_seed;
    char text[PACKET_TEXT_SIZE];
};

/* each reads or writes one field at *cursor, and moves *cursor past it. a
 * text field takes size bytes, which must be at least 1. a read keeps at most
 * size - 1 of them, and ends text with a NUL. a write pads text with NULs to
 * size */
Uint8 packet_read_u8(Uint8 **cursor);
Uint16 packet_read_u16(Uint8 **cursor);
Uint32 packet_read_u32(Uint8 **cursor);
void packet_read_bytes(Uint8 **cursor, void *data, Uint32 size);
void packet_read_text(Uint8 **cursor, char *text, Uint32 size);
void packet_write_u8(Uint8 **cursor, Uint8 value);
void packet_write_u16(Uint8 **cursor, Uint16 value);
void packet_write_u32(Uint8 **cursor, Uint32 value);
void packet_write_bytes(Uint8 **cursor, const void *data, Uint32 size);
void packet_write_text(Uint8 **cursor, const char *text, Uint32 size);
Uint8 *packet_read_header(struct kinder_packet *packet, Uint8 *cursor);
Uint8 *packet_write_header(struct kinder_packet *packet, Uint8 *cursor);
/* set packet's kind to -1 and its size to the header's. its codec reads and
 * writes the header alone. a kind's init calls it first. the init then sets
 * its kind and codec, and adds its fields' bytes to size */
void packet_init(struct kinder_packet *packet);
void pong_packet_init(struct ping_packet *pong);
/* decode frame's packet, of a kind enum packet_kind names. frame must hold
 * the whole packet. returns the packet, which the caller frees with
 * packet_free. returns NULL for another kind, or where the packet's record
 * can't be allocated. under NONMATCHING, a size other than the kind's is
 * NULL too */
struct kinder_packet *packet_decode(Uint8 *frame);
void packet_free(struct kinder_packet *packet);
#endif

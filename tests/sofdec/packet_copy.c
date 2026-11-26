/* sfmps_CopyPketData on a packet of each kind. each kind but MPS_KIND_OTHER
 * goes to its copy in sfmps_CopyPketFn. a packet of MPS_KIND_OTHER is taken
 * and dropped, as padding is, and nothing reads past the table */
#include <assert.h>
#include <string.h>

#include <mps.h>
#include <sfd.h>
#include <sg_xpt.h>

typedef Sint32 (*packet_copy_fn)(struct sfd_handle *, Sint32, char *, Sint32,
                                 Sint32);
static const packet_copy_fn sfmps_CopyPketFn[MPS_KIND_OTHER];

/* the packet the parser read last, which MPS_GetPketHd gives */
static struct mps_packet read_packet;

Sint32 MPS_GetPketHd(struct mps_handle *handle, struct mps_packet *out)
{
    (void)handle;
    *out = read_packet;
    return 0;
}

Sint32 SFLIB_SetErr(struct sfd_handle *handle, Sint32 code)
{
    (void)handle;
    return code;
}

#include "extracted.inc"

/* the copies each kind's entry made */
static Sint32 copies[MPS_KIND_OTHER];

#define COPY(function, kind)                                                   \
    static Sint32 function(struct sfd_handle *handle, Sint32 channel,          \
                           char *data, Sint32 size, Sint32 pts)                \
    {                                                                          \
        (void)handle;                                                          \
        (void)channel;                                                         \
        (void)data;                                                            \
        (void)size;                                                            \
        (void)pts;                                                             \
        copies[kind]++;                                                        \
        return TAKEN;                                                          \
    }
COPY(copy_audio, MPS_KIND_AUDIO)
COPY(copy_video, MPS_KIND_VIDEO)
COPY(copy_private, MPS_KIND_PRIVATE)
COPY(copy_padding, MPS_KIND_PADDING)

static const packet_copy_fn sfmps_CopyPketFn[MPS_KIND_OTHER] = {
    copy_audio, copy_video, copy_private, copy_padding};

static struct sfd_handle handle;

/* the bytes a packet of kind and a 16-byte payload uses, as the copy finds
 * them */
static Sint32 copy(Sint32 kind)
{
    char data[64];
    Sint32 used;

    memset(&read_packet, 0, sizeof(read_packet));
    read_packet.kind = kind;
    read_packet.payload_length = 16;
    assert(sfmps_CopyPketData(&handle, data, sizeof(data), &used) == 0);
    return used;
}

int main(void)
{
    Sint32 kind;

    for (kind = MPS_KIND_AUDIO; kind < MPS_KIND_OTHER; kind++) {
        assert(copy(kind) == 16);
        assert(copies[kind] == 1);
    }
    assert(copy(MPS_KIND_OTHER) == 16);
    for (kind = MPS_KIND_AUDIO; kind < MPS_KIND_OTHER; kind++) {
        assert(copies[kind] == 1);
    }
    return 0;
}

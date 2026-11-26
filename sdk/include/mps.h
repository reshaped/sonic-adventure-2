/* CRI's MPEG program stream parser. each handle keeps the pack, system and
 * packet headers it read last */
#ifndef MPS_H
#define MPS_H
#include <sg_xpt.h>

typedef void (*mps_error_fn)(void *user, Sint32 code);
struct mps_error {
    mps_error_fn report;
    void *user;
    Sint32 code;
};
/* a header field the parser hasn't read, as a cleared handle holds it */
#define MPS_UNKNOWN (-1)
/* the bytes of an MPEG-1 pack header, and of a packet header's start code,
 * stream id and 2-byte length */
#define MPS_PACK_HEADER_SIZE 12
#define MPS_PACKET_HEADER_SIZE 6
struct mps_pack {
    Bool mpeg1;
    Sint32 clock;
    Sint32 mux_rate;
};
struct mps_system {
    Sint32 length;
    Sint32 rate_bound;
    Sint32 audio_bound;
    Sint32 video_bound;
    Bool fixed_rate, constrained, audio_lock, video_lock;
};
struct mps_packet {
    Sint32 stream_id;
    Sint32 kind;
    Sint32 channel;
    Sint32 length;
    Sint32 buffer_size;
    /* the presentation and decoding time stamps, in 90 kHz ticks divided by 4.
     * the division keeps a 33-bit stamp a positive Sint32. each is -1 where the
     * packet has none. a padding or private stream 2 packet leaves both as they
     * were */
    Sint32 pts, dts;
    Sint32 payload_length;
};
/* pts or dts where the packet has none */
#define MPS_NO_STAMP (-1)
/* struct mps_packet's kinds. a stream id of no other kind is MPS_KIND_OTHER */
enum mps_packet_kind {
    MPS_KIND_AUDIO,
    MPS_KIND_VIDEO,
    MPS_KIND_PRIVATE,
    MPS_KIND_PADDING,
    MPS_KIND_OTHER
};
/* MPEG's stream ids: the private and padding streams', and each end of the
 * audio and video ranges */
enum mps_stream_id {
    MPS_STREAM_PRIVATE1 = 0xbd,
    MPS_STREAM_PADDING = 0xbe,
    MPS_STREAM_PRIVATE2 = 0xbf,
    MPS_STREAM_AUDIO_FIRST = 0xc0,
    MPS_STREAM_AUDIO_LAST = 0xdf,
    MPS_STREAM_VIDEO_FIRST = 0xe0,
    MPS_STREAM_VIDEO_LAST = 0xef
};
enum mps_system_kind { MPS_AUDIO, MPS_VIDEO, MPS_OTHER, MPS_SYSTEM_KINDS };
enum mps_delimiter {
    MPS_NONE = 0,
    MPS_PACK = 0x10000,
    MPS_SYSTEM = 0x20000,
    MPS_PACKET = 0x40000,
    MPS_END = 0x80000
};
enum mps_code {
    MPS_CODE_END = 0x1b9,
    MPS_CODE_PACK = 0x1ba,
    MPS_CODE_SYSTEM = 0x1bb,
    MPS_CODE_PRIVATE1 = 0x1bd,
    MPS_CODE_PADDING = 0x1be,
    MPS_CODE_PRIVATE2 = 0x1bf,
    MPS_CODE_AUDIO_FIRST = 0x1c0,
    MPS_CODE_VIDEO_LAST = 0x1ef
};

struct mps_handle {
    Sint32 state;
    struct mps_error error;
    Sint32 length_bytes;
    struct mps_pack pack;
    struct mps_system last_system;
    struct mps_system systems[MPS_SYSTEM_KINDS];
    struct mps_packet packet;
};
struct mps_library {
    char version[32];
    char copyright[32];
    struct mps_error error;
    Sint32 count;
    /* the first of count handles, which MPS_Init's work must hold */
    struct mps_handle handles[1];
};

#define MPS_ERR_SET_CALLBACK ((Sint32)0xff020101UL)
#define MPS_ERR_GET_ERROR ((Sint32)0xff020102UL)
#define MPS_ERR_DESTROY ((Sint32)0xff020103UL)
#define MPS_ERR_GET_PACK ((Sint32)0xff020201UL)
#define MPS_ERR_GET_SYSTEM ((Sint32)0xff020202UL)
#define MPS_ERR_GET_PACKET ((Sint32)0xff020203UL)
#define MPS_ERR_DECODE ((Sint32)0xff020301UL)
#define MPS_ERR_LENGTH ((Sint32)0xff020302UL)

extern struct mps_library *MPSLIB_libwork;
Sint32 MPS_Init(Sint32 count, void *work);
void MPS_Finish(void);
struct mps_handle *MPS_Create(void);
Sint32 MPS_Destroy(struct mps_handle *handle);
/* set the function that reports handle's errors, and the user it passes.
 * for this and MPS_GetErrInf, a NULL handle names the library's own error
 * record */
Sint32 MPS_SetErrFn(struct mps_handle *handle, mps_error_fn report, void *user);
Sint32 MPS_GetErrInf(struct mps_handle *handle, struct mps_error *out);
void MPSLIB_InitErrInf(struct mps_error *error);
Sint32 MPSLIB_SetErr(struct mps_handle *handle, Sint32 code);
Sint32 MPSLIB_CheckHn(struct mps_handle *handle);
void MPSDEC_Init(void);
void MPSDEC_Finish(void);
void MPSGET_Init(void);
void MPSGET_Finish(void);
Sint32 MPSDEL_CheckDelim(const Uint8 *data);
Sint32 MPS_SetPketLenField(struct mps_handle *handle, Sint32 bytes);
/* the bytes of packet_length, MPEG's packet length field, which a handle
 * reads until MPS_SetPketLenField sets another count */
#define MPS_LENGTH_BYTES 2
Sint32 MPS_DecHd(struct mps_handle *handle, const Uint8 *data, Sint32 available,
                 Sint32 *used, Sint32 *kinds);
Sint32 MPS_GetPackHd(struct mps_handle *handle, struct mps_pack *out);
Sint32 MPS_GetSysHd(struct mps_handle *handle, struct mps_system *out,
                    Sint32 index);
Sint32 MPS_GetLastSysHd(struct mps_handle *handle, struct mps_system *out);
Sint32 MPS_GetPketHd(struct mps_handle *handle, struct mps_packet *out);

#endif

/* a handle's stream header, which a private stream 2 pack carries. sfh.h's
 * header reader reads it into struct sfd_file_header */
#include <mps.h>
#include <sfd.h>
#include <sfh.h>
#include <sg_xpt.h>
#include <sofdec_uty.h>

#define HEADER_HANDLES 32
#define PRIVATE2_CHANNEL 2
/* the bytes a packet header takes past MPS_PACKET_HEADER_SIZE where its
 * length field takes 4 */
#define WIDE_LENGTH_EXTRA 2
/* a field the header reader gives no value for */
#define NOT_READ (-1)
/* sfhds_AnlyUsedStmid's result where the header lists no stream of the
 * range */
#define NO_STREAM 0

/* the header readers SFH_Init hands out */
static struct {
    struct sfh_handle handles[HEADER_HANDLES];
    Uint32 reserved;
} sfhds_sfhlib_work;
static Bool sfhds_IsPrvate2PketStart(const Uint8 *data);
static Sint32 sfhds_ReadLong(const Uint8 *data);
static Bool sfhds_IsSfdHeader(const Uint8 *data, Sint32 size);
static Bool sfhds_SetHdrRaw(struct sfd_handle *handle, const Uint8 *data,
                            Sint32 size);
static void sfhds_SetHdrSeek(struct sfd_handle *handle);
static void sfhds_CopyFhd(struct sfd_file_header *destination,
                          const struct sfd_file_header *source);
static struct sfd_file_header *sfhds_GetHd(struct sfd_handle *handle);
static void sfhds_ProcessHdr(struct sfd_handle *handle);
static void sfhds_DoProcessHdr(struct sfh_handle *reader,
                               struct sfd_file_header *header);
static void sfhds_AnlyHead(struct sfh_handle *reader,
                           struct sfd_file_header *header);
static void sfhds_AnlySys(struct sfh_handle *reader,
                          struct sfd_system_header *system);
static Sint32 sfhds_AnlyUsedStmid(struct sfh_handle *reader, Sint32 first,
                                  Sint32 last);
static void sfhds_AnlyAudio(struct sfh_handle *reader, Sint32 stream,
                            struct sfd_audio_header *audio);
static void sfhds_AnlyVideo(struct sfh_handle *reader, Sint32 stream,
                            struct sfd_video_header *video);
static Sint32 sfhds_CallN(struct sfh_handle *reader, sfh_query *query);
static Sint32 sfhds_CallS(struct sfh_handle *reader, Uint32 stream,
                          sfh_stream_query *query);

void SFHDS_Init(void)
{
    SFH_Init(HEADER_HANDLES, sfhds_sfhlib_work.handles);
}

void SFHDS_Finish(void)
{
    SFH_Finish();
}

void SFHDS_InitFhd(struct sfd_file_header *header)
{
    header->valid = FALSE;
    header->major = 0;
    header->minor = 0;
    header->byte_rate = 0;
    header->raw_size = 0;
}

void SFHDS_FinishFhd(struct sfd_file_header *header)
{
    header->valid = FALSE;
    header->byte_rate = 0;
    header->raw_size = 0;
}

/* whether data, a packet's payload of size bytes, lies in a pack that holds
 * the stream header. it looks only at private stream 2 packets. it reads
 * back to the pack's start, up to 20 bytes before data. those bytes must hold
 * the pack's headers. *set is TRUE where the handle had no header yet and keeps
 * this one */
Bool SFHDS_SetHdr(struct sfd_handle *handle, Sint32 channel, const Uint8 *data,
                  Sint32 size, Bool *set)
{
    *set = FALSE;
    if (channel != PRIVATE2_CHANNEL) {
        return FALSE;
    }
    data -= MPS_PACKET_HEADER_SIZE;
    size += MPS_PACKET_HEADER_SIZE;
    if (!sfhds_IsPrvate2PketStart(data)) {
        data -= WIDE_LENGTH_EXTRA;
        size += WIDE_LENGTH_EXTRA;
        if (!sfhds_IsPrvate2PketStart(data)) {
            return FALSE;
        }
    }
    size += MPS_PACK_HEADER_SIZE;
    data -= MPS_PACK_HEADER_SIZE;
    if (!sfhds_IsSfdHeader(data, size)) {
        return FALSE;
    }
    *set = sfhds_SetHdrRaw(handle, data, size);
    return TRUE;
}

static Bool sfhds_IsPrvate2PketStart(const Uint8 *data)
{
    Sint32 code;
    code = sfhds_ReadLong(data);
    return code == MPS_CODE_PRIVATE2 ? TRUE : FALSE;
}

static Sint32 sfhds_ReadLong(const Uint8 *data)
{
    const Uint8 *cursor;
    /* @hack redundant: word declared register, where a plain local would do */
    register Uint32 word;

    /* @hack spelling: cursor, a copy of data, where data would do */
    cursor = data;
    word = *cursor++;
    word = (word << 8) | *cursor++;
    word = (word << 8) | *cursor++;
    word = (word << 8) | *cursor;
    return word;
}

static Bool sfhds_IsSfdHeader(const Uint8 *data, Sint32 size)
{
    struct sfh_handle *reader;
    Bool recognized;

    reader = SFH_Create(data, size);
    if (reader == NULL) {
        return FALSE;
    }
    if (!SFH_IsSfdHeader(reader, &recognized)) {
        recognized = FALSE;
    }
    SFH_Destroy(reader);
    return recognized;
}

static Bool sfhds_SetHdrRaw(struct sfd_handle *handle, const Uint8 *data,
                            Sint32 size)
{
    struct sfd_file_header *header;
    const Uint8 *source;
    header = &handle->header;
    if (header->valid) {
        return FALSE;
    }
    size =
        size < (Sint32)sizeof(header->raw) ? size : (Sint32)sizeof(header->raw);
    /* @hack spelling: source, a copy of data, where data would do */
    source = data;
    UTY_MemcpyByte(header->raw, source, size);
    header->raw_size = size;
    sfhds_ProcessHdr(handle);
    sfhds_SetHdrSeek(handle);
    return TRUE;
}

static void sfhds_SetHdrSeek(struct sfd_handle *handle)
{
    struct sfd_file_header *saved;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((saved = sfhds_GetHd(handle)) != NULL) {
        sfhds_CopyFhd(saved, &handle->header);
    }
}
static void sfhds_CopyFhd(struct sfd_file_header *destination,
                          const struct sfd_file_header *source)
{
    *destination = *source;
}
void SFHDS_ReprocessHdr(struct sfd_handle *handle)
{
    struct sfd_file_header *saved;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((saved = sfhds_GetHd(handle)) != NULL) {
        sfhds_CopyFhd(&handle->header, saved);
        sfhds_ProcessHdr(handle);
    }
}
static struct sfd_file_header *sfhds_GetHd(struct sfd_handle *handle)
{
    if (handle->seek.info == NULL) {
        return NULL;
    }
    if (SFMPS_GetConcatCnt(handle) > 0) {
        return NULL;
    }
    return &handle->seek.info->head.header;
}

static void sfhds_ProcessHdr(struct sfd_handle *handle)
{
    struct sfd_file_header *header;
    struct sfh_handle *reader;

    header = &handle->header;
    reader = SFH_Create(header->raw, header->raw_size);
    if (reader != NULL) {
        sfhds_DoProcessHdr(reader, header);
        SFH_Destroy(reader);
    }
}

static void sfhds_DoProcessHdr(struct sfh_handle *reader,
                               struct sfd_file_header *header)
{
    Sint32 rate;
    Sint32 major;
    Sint32 minor;
    Bool recognized;
    Sint32 version;

    if (!SFH_IsSfdHeader(reader, &recognized)) {
        recognized = FALSE;
    }
    if (!recognized) {
        return;
    }
    if (!SFH_AnlyHdrToolVer(reader, &major, &minor)) {
        major = 0;
        minor = 0;
    }
    header->major = major;
    header->minor = minor;
    version = SFH_VERSION(header->major, header->minor);
    if (!SFH_AnlyByteRate(reader, &rate)) {
        rate = 0;
    }
    /* an older header's byte rate is kept negated, and sfsee_ExecHeadAnaly
     * takes only a positive rate */
    if (version < SFH_FEATURE_VERSION) {
        rate = -rate;
    }
    header->byte_rate = rate;
    sfhds_AnlyHead(reader, header);
    sfhds_AnlySys(reader, &header->system);
    header->private1_id =
        sfhds_AnlyUsedStmid(reader, MPS_STREAM_PRIVATE1, MPS_STREAM_PRIVATE1);
    header->private2_id =
        sfhds_AnlyUsedStmid(reader, MPS_STREAM_PRIVATE2, MPS_STREAM_PRIVATE2);
    header->audio_id = sfhds_AnlyUsedStmid(reader, MPS_STREAM_AUDIO_FIRST,
                                           MPS_STREAM_AUDIO_LAST);
    header->video_id = sfhds_AnlyUsedStmid(reader, MPS_STREAM_VIDEO_FIRST,
                                           MPS_STREAM_VIDEO_LAST);
    sfhds_AnlyAudio(reader, header->audio_id, &header->audio);
    sfhds_AnlyVideo(reader, header->video_id, &header->video);
    header->valid = TRUE;
}

static void sfhds_AnlyHead(struct sfh_handle *reader,
                           struct sfd_file_header *header)
{
    header->header_size = sfhds_CallN(reader, SFH_AnlyHdrSiz);
    header->pack_type = sfhds_CallN(reader, SFH_AnlyPackType);
    header->length_bytes = sfhds_CallN(reader, SFH_AnlyPketSizLen);
    if (header->length_bytes == NOT_READ) {
        header->length_bytes = MPS_LENGTH_BYTES;
    }
    header->pack_size = sfhds_CallN(reader, SFH_AnlyPackSiz);
}

static void sfhds_AnlySys(struct sfh_handle *reader,
                          struct sfd_system_header *system)
{
    system->elements = sfhds_CallN(reader, SFH_AnlyNumElemTot);
    system->audio = sfhds_CallN(reader, SFH_AnlyNumElemAud);
    system->video = sfhds_CallN(reader, SFH_AnlyNumElemVid);
    system->private_streams = sfhds_CallN(reader, SFH_AnlyNumElemPrv);
    system->max_audio_length = sfhds_CallN(reader, SFH_AnlyMaxPlyLenAud);
    system->max_video_length = sfhds_CallN(reader, SFH_AnlyMaxPlyLenVid);
    system->max_frames = sfhds_CallN(reader, SFH_AnlyMaxFrmNum);
}

static Sint32 sfhds_AnlyUsedStmid(struct sfh_handle *reader, Sint32 first,
                                  Sint32 last)
{
    Sint32 stream;
    Bool exists;

    for (stream = first; stream <= last; stream++) {
        if (!SFH_IsExistStmId(reader, stream, &exists)) {
            continue;
        }
        if (exists) {
            return stream;
        }
    }
    return NO_STREAM;
}

static void sfhds_AnlyAudio(struct sfh_handle *reader, Sint32 stream,
                            struct sfd_audio_header *audio)
{
    if (stream == NO_STREAM) {
        return;
    }
    audio->codec = sfhds_CallS(reader, stream, SFH_AnlyElemCodecAud);
    audio->layer = sfhds_CallS(reader, stream, SFH_AnlyElemLayer);
    audio->channels = sfhds_CallS(reader, stream, SFH_AnlyElemChNum);
    audio->sample_rate = sfhds_CallS(reader, stream, SFH_AnlyElemSmpHz);
}

static void sfhds_AnlyVideo(struct sfh_handle *reader, Sint32 stream,
                            struct sfd_video_header *video)
{
    Bool available;

    video->codec = sfhds_CallS(reader, stream, SFH_AnlyElemCodecVid);
    video->bit_rate = sfhds_CallS(reader, stream, SFH_AnlyElemBitRate);
    if (!SFH_AnlyElemPicSz(reader, stream, &video->width, &video->height)) {
        video->width = NOT_READ;
        video->height = NOT_READ;
    }
    video->picture_rate = sfhds_CallS(reader, stream, SFH_AnlyElemPicRate);
    if (!SFH_IsEffFtrInf(reader, stream, &available)) {
        available = FALSE;
    }
    video->features = available != 0 ? TRUE : FALSE;
    if (available != 0) {
        video->color = sfhds_CallS(reader, stream, SFH_AnlyFtrColType);
        video->picture_type = sfhds_CallS(reader, stream, SFH_AnlyFtrPicType);
        video->fixed = sfhds_CallS(reader, stream, SFH_AnlyFtrFixFlg);
        video->fixed_shc = sfhds_CallS(reader, stream, SFH_AnlyFtrShcFixFlg);
        video->expand = sfhds_CallS(reader, stream, SFH_AnlyFtrExpand);
        video->gop_n = sfhds_CallS(reader, stream, SFH_AnlyFtrGopN);
        video->gop_m = sfhds_CallS(reader, stream, SFH_AnlyFtrGopM);
    }
}

/* sfhds_CallN and sfhds_CallS return the value their query reads, or
 * NOT_READ */
static Sint32 sfhds_CallN(struct sfh_handle *reader, sfh_query *query)
{
    Sint32 value;
    if (!query(reader, &value)) {
        return NOT_READ;
    }
    return value;
}

static Sint32 sfhds_CallS(struct sfh_handle *reader, Uint32 stream,
                          sfh_stream_query *query)
{
    Sint32 value;
    if (!query(reader, stream, &value)) {
        return NOT_READ;
    }
    return value;
}

Sint32 SFHDS_GetPketLenField(void)
{
    return MPS_LENGTH_BYTES;
}

Sint32 SFHDS_GetMuxVerNum(struct sfd_handle *handle)
{
    struct sfd_file_header *header;
    header = &handle->header;
    if (!header->valid) {
        return 0;
    }
    return SFH_VERSION(header->major, header->minor);
}

Sint32 SFHDS_GetColType(struct sfd_handle *handle)
{
    struct sfd_file_header *header;
    struct sfd_video_header *video;
    header = &handle->header;
    video = &header->video;
    if (!header->valid) {
        return SFD_NO_COLOR;
    }
    if (!video->features) {
        return SFD_NO_COLOR;
    }
    return video->color;
}

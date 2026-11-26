/* the buffers between a handle's stages, one for each enum sfd_buffer_id.
 * SFBUF_InitHn lays them over the handle's work area one after another, in
 * that order. a ring hands out its data to read, or its room to write, as
 * two runs. struct sfd_ring_info's head runs from the position up to the
 * ring's end. its wrap goes on from the ring's start, and is empty unless the
 * data or the room wraps */
#include <sfd.h>
#include <sg_xpt.h>

enum buffer_kind { BYTE_RING, FRAME_QUEUE, PCM_RING, USER_CHANNELS };
/* the bytes kept past the video and audio rings' ends. a decoder copies the
 * bytes at its ring's start there, to read across the end in one run */
#define VIDEO_MARGIN 0x28000
#define AUDIO_MARGIN 0x800
#define NO_STORAGE ((Sint32)0xff000401)
#define RING_CLOSED ((Sint32)0xff000403)
#define BYTE_OVERFLOW ((Sint32)0xff000404)
#define BYTE_UNDERFLOW ((Sint32)0xff000405)
#define PCM_OVERFLOW ((Sint32)0xff000406)
#define PCM_UNDERFLOW ((Sint32)0xff000407)

static void sfbuf_MakeBufPtr(char **areas, const Sint32 *sizes, char *work);
static void sfbuf_InitRingBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer,
                              Sint32 margin);
static void sfbuf_InitVfrmBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer);
static void sfbuf_InitAringBuf(struct sfd_buffer *records, char **areas,
                               const Sint32 *sizes, Sint32 buffer);
static void sfbuf_InitUoSjBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer);
static void sfbuf_InitUoSj(struct sfd_user_outputs *outputs);
static void sfbuf_RefRingbuf(const struct sfd_ring *ring, char **read,
                             Sint32 *total_read, char **write,
                             Sint32 *total_written, Sint32 *fill);

void SFBUF_Init(Sint32 *work)
{
}
void SFBUF_Finish(Sint32 *work)
{
}

void SFBUF_InitHn(struct sfd_handle *handle, struct sfd_buffer_table *table,
                  struct sfd_create_params *params)
{
    struct sfd_buffer *records;
    Sint32 *sizes;
    Sint32 margin;
    char *areas[SFD_BUFFERS];
    records = table->records;
    sizes = params->sizes;
    sfbuf_MakeBufPtr(areas, sizes, params->work);
    /* the system ring holds whole read units, and the rest of its buffer is
     * margin */
    margin = sizes[SFD_SYSTEM_BYTES] % params->read_unit;
    sfbuf_InitRingBuf(records, areas, sizes, SFD_SYSTEM_BYTES, margin);
    sfbuf_InitRingBuf(records, areas, sizes, SFD_VIDEO_BYTES, VIDEO_MARGIN);
    sfbuf_InitRingBuf(records, areas, sizes, SFD_AUDIO_BYTES, AUDIO_MARGIN);
    sfbuf_InitVfrmBuf(records, areas, sizes, SFD_VIDEO_FRAMES);
    sfbuf_InitAringBuf(records, areas, sizes, SFD_AUDIO_SAMPLES);
    sfbuf_InitVfrmBuf(records, areas, sizes, SFD_POST_FRAMES);
    sfbuf_InitAringBuf(records, areas, sizes, SFD_POST_SAMPLES);
    sfbuf_InitUoSjBuf(records, areas, sizes, SFD_USER_DATA);
}

static void sfbuf_MakeBufPtr(char **areas, const Sint32 *sizes, char *work)
{
    Sint32 i;
    char *cursor;
    cursor = work;
    for (i = 0; i < SFD_BUFFERS; i++) {
        areas[i] = cursor;
        cursor += sizes[i];
    }
}

#define INIT_BUFFER(record, type, storage)                                     \
    do {                                                                       \
        (record)->kind = type;                                                 \
        (record)->has_storage = storage;                                       \
        (record)->prepared = FALSE;                                            \
        (record)->ended = FALSE;                                               \
        (record)->writer = SFD_NO_STAGE;                                       \
        (record)->reader = SFD_NO_STAGE;                                       \
    } while (0)
/* @hack spelling: records, areas and sizes stepped on to buffer, where
 * indexing them by buffer would do */
#define STEP_TO_BUFFER(buffer)                                                 \
    do {                                                                       \
        records += (buffer);                                                   \
        areas += (buffer);                                                     \
        sizes += (buffer);                                                     \
    } while (0)

static void sfbuf_InitRingBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer, Sint32 margin)
{
    struct sfd_ring *ring;
    Bool storage;
    STEP_TO_BUFFER(buffer);
    ring = &records->data.bytes;
    if (*sizes == 0) {
        margin = 0;
        storage = FALSE;
    } else {
        storage = TRUE;
    }
    INIT_BUFFER(records, BYTE_RING, storage);
    ring->data = *areas;
    ring->size = *sizes - margin;
    ring->end = ring->data + ring->size;
    ring->margin = margin;
    ring->write = ring->data;
    ring->read = ring->data;
    ring->delimiter = NULL;
    ring->scan = NULL;
    ring->margin_end = NULL;
    ring->copied = 0;
    ring->total_written = 0;
    ring->total_read = 0;
    ring->fill = 0;
    ring->closed = FALSE;
}

static void sfbuf_InitVfrmBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer)
{
    struct sfd_frame_queue *frames;
    Bool storage;
    Sint32 i;
    STEP_TO_BUFFER(buffer);
    if (*sizes == 0) {
        storage = FALSE;
    } else {
        storage = TRUE;
    }
    frames = &records->data.frames;
    INIT_BUFFER(records, FRAME_QUEUE, storage);
    frames->data = *areas;
    frames->size = *sizes;
    frames->reserved[0] = 0;
    frames->reserved[1] = 0;
    /* @hack spelling: the slots cleared by a do loop, where a for loop would
     * do */
    i = 0;
    do {
        frames->slots[i].state = SFD_SLOT_FREE;
    } while (++i < SFD_FRAME_POOL);
}

static void sfbuf_InitAringBuf(struct sfd_buffer *records, char **areas,
                               const Sint32 *sizes, Sint32 buffer)
{
    struct sfd_pcm_ring *pcm;
    Bool storage;
    STEP_TO_BUFFER(buffer);
    if (*sizes == 0) {
        storage = FALSE;
    } else {
        storage = TRUE;
    }
    pcm = &records->data.pcm;
    INIT_BUFFER(records, PCM_RING, storage);
    pcm->data = *areas;
    pcm->size = *sizes;
    pcm->rate = 0;
    pcm->bits = 0;
    pcm->channels = 0;
    pcm->left = NULL;
    pcm->right = NULL;
    pcm->samples = 0;
    pcm->write = 0;
    pcm->read = 0;
    pcm->total_written = 0;
    pcm->total_read = 0;
}

void SFBUF_FixAringBuf(struct sfd_handle *handle, Sint32 buffer, Sint32 bits,
                       Sint32 channels, Sint32 rate)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_pcm_ring *pcm;
    char *data;
    Sint32 plane_size;
    Sint32 sample_size;
    table = &handle->buffers;
    record = &table->records[buffer];
    pcm = &record->data.pcm;
    data = pcm->data;
    plane_size = pcm->size;
    if (channels > 1) {
        plane_size /= channels;
    }
    sample_size = (bits + 7) / 8;
    pcm->bits = bits;
    pcm->channels = channels;
    pcm->rate = rate;
    pcm->left = data;
    pcm->right = data + plane_size;
    pcm->samples = plane_size / sample_size;
}

static void sfbuf_InitUoSjBuf(struct sfd_buffer *records, char **areas,
                              const Sint32 *sizes, Sint32 buffer)
{
    struct sfd_user_outputs *outputs;
    /* @hack spelling: records stepped on to buffer, where indexing it by
     * buffer would do */
    records += buffer;
    outputs = &records->data.user;
    /* @hack spelling: the record's fields stored one by one, where
     * INIT_BUFFER would do */
    records->kind = USER_CHANNELS;
    records->has_storage = TRUE;
    records->prepared = FALSE;
    records->ended = FALSE;
    records->writer = SFD_NO_STAGE;
    records->reader = SFD_NO_STAGE;
    sfbuf_InitUoSj(outputs);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void sfbuf_InitUoSj(struct sfd_user_outputs *outputs)
{
    Sint32 i;
    struct sfd_user_channel *channel;
    for (i = 0; i < SFD_USER_CHANNELS; i++) {
        channel = &outputs->channels[i];
        channel->stream = NULL;
        channel->notify = NULL;
    }
}

void SFBUF_SetUoch(struct sfd_handle *handle, Sint32 buffer, Sint32 channel,
                   const struct sfd_user_channel *source)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_user_outputs *outputs;
    table = &handle->buffers;
    record = &table->records[buffer];
    outputs = &record->data.user;
    outputs->channels[channel] = *source;
}
void SFBUF_GetUoch(struct sfd_handle *handle, Sint32 buffer, Sint32 channel,
                   struct sfd_user_channel *out)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_user_outputs *outputs;
    table = &handle->buffers;
    record = &table->records[buffer];
    outputs = &record->data.user;
    *out = outputs->channels[channel];
}

/* return NO_STORAGE from the caller, set as the error of the caller's
 * handle, where record has no storage */
#define REQUIRE_STORAGE(record)                                                \
    do {                                                                       \
        if (!(record)->has_storage) {                                          \
            return SFLIB_SetErr(handle, NO_STORAGE);                           \
        }                                                                      \
    } while (0)

/* set *out to ring's bytes from start up to limit. where unavailable holds,
 * it sets two empty runs at start instead. out, ring, total_written and
 * total_read are the caller's locals */
#define BYTE_SPANS(unavailable, start, limit)                                  \
    do {                                                                       \
        if (unavailable) {                                                     \
            out->head.data = start;                                            \
            out->head.size = 0;                                                \
            out->wrap.data = start;                                            \
            out->wrap.size = 0;                                                \
        } else if ((start) < (limit)) {                                        \
            out->head.data = start;                                            \
            out->head.size = (limit) - (start);                                \
            out->wrap.data = start;                                            \
            out->wrap.size = 0;                                                \
        } else {                                                               \
            out->head.data = start;                                            \
            out->head.size = ring->end - (start);                              \
            out->wrap.data = ring->data;                                       \
            out->wrap.size = (limit) - ring->data;                             \
        }                                                                      \
        out->closed = ring->closed;                                            \
        out->written = total_written;                                          \
        out->read = total_read;                                                \
    } while (0)

Sint32 SFBUF_RingGetWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_ring_info *out)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    char *write;
    char *read;
    Sint32 total_read;
    Sint32 total_written;
    Sint32 fill;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    ring = &record->data.bytes;
    sfbuf_RefRingbuf(ring, &read, &total_read, &write, &total_written, &fill);
    BYTE_SPANS(fill >= ring->size, write, read);
    return 0;
}

Sint32 SFBUF_RingAddWrite(struct sfd_handle *handle, Sint32 buffer,
                          Sint32 bytes, Sint32 written)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    Sint32 mask;
    Sint32 error;
    error = 0;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    ring = &record->data.bytes;
    if (ring->closed) {
        return SFLIB_SetErr(handle, RING_CLOSED);
    }
    SFLIB_LockCs(&mask);
    ring->write += bytes;
    if (ring->write >= ring->end) {
        ring->write -= ring->size;
    }
    ring->fill += bytes;
    if (ring->fill > ring->size) {
        error = SFLIB_SetErr(handle, BYTE_OVERFLOW);
    }
    ring->total_written += bytes;
    SFLIB_UnlockCs(&mask);
    return error;
}

Sint32 SFBUF_RingGetRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_ring_info *out)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    char *read;
    char *write;
    Sint32 total_read;
    Sint32 total_written;
    Sint32 fill;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    ring = &record->data.bytes;
    sfbuf_RefRingbuf(ring, &read, &total_read, &write, &total_written, &fill);
    BYTE_SPANS(fill <= 0, read, write);
    return 0;
}

Sint32 SFBUF_RingAddRead(struct sfd_handle *handle, Sint32 buffer, Sint32 bytes)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    Sint32 mask;
    Sint32 error;
    error = 0;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    ring = &record->data.bytes;
    SFLIB_LockCs(&mask);
    /* a read past the delimiter clears the video stage's search state,
     * delimiter and scan. a read that wraps clears the stage's copy into the
     * margin, margin_end and copied */
    ring->read += bytes;
    if (ring->read >= ring->delimiter) {
        ring->delimiter = NULL;
        ring->scan = NULL;
    }
    if (ring->read >= ring->end) {
        ring->read -= ring->size;
        ring->margin_end = NULL;
        ring->copied = 0;
    }
    ring->fill -= bytes;
    if (ring->fill < 0) {
        error = SFLIB_SetErr(handle, BYTE_UNDERFLOW);
    }
    ring->total_read += bytes;
    SFLIB_UnlockCs(&mask);
    return error;
}

static void sfbuf_RefRingbuf(const struct sfd_ring *ring, char **read,
                             Sint32 *total_read, char **write,
                             Sint32 *total_written, Sint32 *fill)
{
    Sint32 mask;
    SFLIB_LockCs(&mask);
    *read = ring->read;
    *total_read = ring->total_read;
    *write = ring->write;
    *total_written = ring->total_written;
    *fill = ring->fill;
    SFLIB_UnlockCs(&mask);
}
void SFBUF_RingGetDlm(struct sfd_handle *handle, Sint32 buffer,
                      char **delimiter, char **scan, Sint32 *copied,
                      char **margin_end, Sint32 *margin)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    Sint32 mask;
    SFLIB_LockCs(&mask);
    table = &handle->buffers;
    record = &table->records[buffer];
    ring = &record->data.bytes;
    *delimiter = ring->delimiter;
    *scan = ring->scan;
    *margin_end = ring->margin_end;
    *copied = ring->copied;
    *margin = ring->margin;
    SFLIB_UnlockCs(&mask);
}

void SFBUF_RingSetDlm(struct sfd_handle *handle, Sint32 buffer, char *delimiter,
                      char *scan, Sint32 copied, char *margin_end)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    Sint32 mask;
    SFLIB_LockCs(&mask);
    table = &handle->buffers;
    record = &table->records[buffer];
    ring = &record->data.bytes;
    ring->delimiter = delimiter;
    ring->scan = scan;
    ring->margin_end = margin_end;
    ring->copied = copied;
    SFLIB_UnlockCs(&mask);
}

/* @hack redundant: a test of bits whose arms are both null, where a null
 * would do */
#define EMPTY_SAMPLE(bits) ((bits) <= 8 ? (char *)0 : (char *)0)
#define SAMPLE_AT(bits, plane, position)                                       \
    ((bits) <= 8 ? (plane) + (position)                                        \
                 : (char *)((Sint16 *)(plane) + (position)))

/* define function, which sets *out to a PCM ring's samples from start up to
 * limit. where unavailable holds, it sets no samples. the arguments are
 * expressions of the locals it copies from the ring in one critical section */
#define PCM_SPANS(function, unavailable, start, limit)                         \
    Sint32 function(struct sfd_handle *handle, Sint32 buffer,                  \
                    struct sfd_pcm_info *out)                                  \
    {                                                                          \
        struct sfd_buffer_table *table;                                        \
        struct sfd_buffer *record;                                             \
        struct sfd_pcm_ring *pcm;                                              \
        Sint32 mask;                                                           \
        Sint32 rate;                                                           \
        Sint32 bits;                                                           \
        Sint32 channels;                                                       \
        char *left;                                                            \
        char *right;                                                           \
        Sint32 samples;                                                        \
        Sint32 write;                                                          \
        Sint32 read;                                                           \
        Sint32 total_written;                                                  \
        Sint32 total_read;                                                     \
        table = &handle->buffers;                                              \
        record = &table->records[buffer];                                      \
        REQUIRE_STORAGE(record);                                               \
        pcm = &record->data.pcm;                                               \
        SFLIB_LockCs(&mask);                                                   \
        rate = pcm->rate;                                                      \
        bits = pcm->bits;                                                      \
        channels = pcm->channels;                                              \
        left = pcm->left;                                                      \
        right = pcm->right;                                                    \
        samples = pcm->samples;                                                \
        write = pcm->write;                                                    \
        read = pcm->read;                                                      \
        total_written = pcm->total_written;                                    \
        total_read = pcm->total_read;                                          \
        SFLIB_UnlockCs(&mask);                                                 \
        out->rate = rate;                                                      \
        out->bits = bits;                                                      \
        out->channels = channels;                                              \
        out->written = total_written;                                          \
        out->read = total_read;                                                \
        if (unavailable) {                                                     \
            out->count[0] = 0;                                                 \
            out->head[0] = EMPTY_SAMPLE(bits);                                 \
            out->head[1] = EMPTY_SAMPLE(bits);                                 \
            out->count[1] = 0;                                                 \
            out->wrap[0] = EMPTY_SAMPLE(bits);                                 \
            out->wrap[1] = EMPTY_SAMPLE(bits);                                 \
        } else if ((start) < (limit)) {                                        \
            out->count[0] = (limit) - (start);                                 \
            out->head[0] = SAMPLE_AT(bits, left, start);                       \
            out->head[1] = SAMPLE_AT(bits, right, start);                      \
            out->count[1] = 0;                                                 \
            out->wrap[0] = EMPTY_SAMPLE(bits);                                 \
            out->wrap[1] = EMPTY_SAMPLE(bits);                                 \
        } else {                                                               \
            out->count[0] = samples - (start);                                 \
            out->head[0] = SAMPLE_AT(bits, left, start);                       \
            out->head[1] = SAMPLE_AT(bits, right, start);                      \
            out->count[1] = limit;                                             \
            out->wrap[0] = SAMPLE_AT(bits, left, 0);                           \
            out->wrap[1] = SAMPLE_AT(bits, right, 0);                          \
        }                                                                      \
        return 0;                                                              \
    }

PCM_SPANS(SFBUF_AringGetWrite, total_written >= total_read + samples, write,
          read)

Sint32 SFBUF_AringAddWrite(struct sfd_handle *handle, Sint32 buffer,
                           Sint32 samples)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_pcm_ring *pcm;
    Sint32 mask;
    Sint32 error;
    error = 0;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    pcm = &record->data.pcm;
    SFLIB_LockCs(&mask);
    pcm->write += samples;
    if (pcm->write >= pcm->samples) {
        pcm->write -= pcm->samples;
    }
    pcm->total_written += samples;
    if (pcm->total_written > pcm->total_read + pcm->samples) {
        error = SFLIB_SetErr(handle, PCM_OVERFLOW);
    }
    SFLIB_UnlockCs(&mask);
    return error;
}

PCM_SPANS(SFBUF_AringGetRead, total_written <= total_read, read, write)

Sint32 SFBUF_AringAddRead(struct sfd_handle *handle, Sint32 buffer,
                          Sint32 samples)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_pcm_ring *pcm;
    Sint32 mask;
    Sint32 error;
    error = 0;
    table = &handle->buffers;
    record = &table->records[buffer];
    REQUIRE_STORAGE(record);
    pcm = &record->data.pcm;
    SFLIB_LockCs(&mask);
    pcm->read += samples;
    if (pcm->read >= pcm->samples) {
        pcm->read -= pcm->samples;
    }
    pcm->total_read += samples;
    if (pcm->total_read > pcm->total_written) {
        error = SFLIB_SetErr(handle, PCM_UNDERFLOW);
    }
    SFLIB_UnlockCs(&mask);
    return error;
}

/* SFBUF_VfrmGetWrite, SFBUF_VfrmAddWrite, SFBUF_VfrmGetRead and
 * SFBUF_VfrmAddRead keep no frames in the queue. a read of a queue without
 * storage goes to the stage that writes it. every other call does nothing
 * and returns 0 */

Sint32 SFBUF_VfrmGetWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_frame_info **out)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    /* @hack redundant: a test of has_storage whose branch is empty */
    if (!record->has_storage) {
    }
    return 0;
}
Sint32 SFBUF_VfrmAddWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_frame_info *frame)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    /* @hack redundant: a test of has_storage whose branch is empty */
    if (!record->has_storage) {
    }
    return 0;
}
Sint32 SFBUF_VfrmGetRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_frame_info **out)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    if (!record->has_storage) {
        return SFTRN_CallTrtTrif(handle, record->writer, SFD_OP_GET_READ,
                                 (Sint32)out, 0);
    }
    return 0;
}
Sint32 SFBUF_VfrmAddRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_frame_info *frame)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    if (!record->has_storage) {
        return SFTRN_CallTrtTrif(handle, record->writer, SFD_OP_ADD_READ,
                                 (Sint32)frame, 0);
    }
    return 0;
}

void SFBUF_TakeBuf(struct sfd_handle *handle, Sint32 buffer, char **data,
                   Sint32 *size)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    struct sfd_ring *ring;
    struct sfd_frame_queue *frames;
    struct sfd_pcm_ring *pcm;
    Sint32 kind;
    table = &handle->buffers;
    record = &table->records[buffer];
    record->has_storage = FALSE;
    kind = record->kind;
    switch (kind) {
    case BYTE_RING:
        ring = &record->data.bytes;
        *data = ring->data;
        *size = ring->size + ring->margin;
        break;
    case FRAME_QUEUE:
        frames = &record->data.frames;
        *data = frames->data;
        *size = frames->size;
        break;
    case PCM_RING:
        pcm = &record->data.pcm;
        *data = pcm->data;
        *size = pcm->size;
        break;
    }
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}
void SFBUF_SetPrepFlg(struct sfd_handle *handle, Sint32 buffer, Bool prepared)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    record->prepared = prepared;
}
Bool SFBUF_GetPrepFlg(struct sfd_handle *handle, Sint32 buffer)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    return record->prepared;
}
void SFBUF_SetTermFlg(struct sfd_handle *handle, Sint32 buffer, Bool ended)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    record->ended = ended;
}
Bool SFBUF_GetTermFlg(struct sfd_handle *handle, Sint32 buffer)
{
    struct sfd_buffer_table *table;
    struct sfd_buffer *record;
    table = &handle->buffers;
    record = &table->records[buffer];
    return record->ended;
}

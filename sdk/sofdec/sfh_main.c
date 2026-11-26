/* the Sofdec stream header, 2048 bytes. it describes the mux, the tool that
 * made it and each of up to STREAMS streams. a handle reads the caller's
 * bytes in place */
#include <ctype.h>
#include <mps.h>
#include <sfh.h>
#include <sg_xpt.h>
#include <string.h>

#define STREAMS 26
#define NAME_BYTES 12
#define TOOL_BYTES 32
#define COMMENT_BYTES 64
/* the tool versions sfh_is_valid takes: OLD_VERSION, and SFH_FEATURE_VERSION
 * or later */
#define OLD_VERSION SFH_VERSION(1, 7)
/* the bytes of "Ver.", which the tool's text puts before its version */
#define VERSION_MARK_BYTES 4
/* the one feature_version whose features a query reads */
#define FEATURE_FORMAT 1
/* the audio codec whose element gives a layer, which only MPEG audio has */
#define MPEG_AUDIO 1
/* the bits a second one unit of an element's bit_rate counts, as MPEG's
 * sequence header counts it */
#define BIT_RATE_BITS 400
/* the largest GOP N or M a query gives. a larger one reads as GOP_UNKNOWN */
#define GOP_MOST 63
#define GOP_UNKNOWN (-1)
/* a picture dimension's mask, 12 bits. the middle of an element's three
 * dimension bytes is split. its high half holds the width's low SHARED_BITS
 * bits, and its low half the height's high ones */
#define DIMENSION_MASK 0xfff
#define SHARED_BITS 4
#define SHARED_MASK 15
/* a handle's status. it is FREE in the pool and OPEN once created. then
 * SFH_IsSfdHeader judges its header INVALID or SOFDEC */
enum header_state { INVALID = -1, FREE, OPEN, SOFDEC };

struct sfh_element {
    char filename[NAME_BYTES], timestamp[NAME_BYTES];
    Uint8 id, codec;
    union {
        struct {
            Uint8 layer, channels;
        } audio;
        Uint16 bit_rate;
    } format;
    union {
        Sint32 sample_rate;
        struct {
            /* the width and then the height, 12 bits each */
            Uint8 dimensions[3];
            Uint8 picture_rate;
        } video;
    } media;
    /* feature_version is FEATURE_FORMAT where the fields from color on hold
     * the stream's features */
    Uint8 feature_version, color, picture_type;
    struct {
        Uint8 fixed : 1;
        Uint8 reserved_upper : 3;
        Uint8 fixed_sequence : 1;
        Uint8 reserved_lower : 3;
    } flags;
    Uint8 expand, gop_n, gop_m;
    Uint8 reserved[25];
};
struct sfh_file {
    char filename[NAME_BYTES], timestamp[NAME_BYTES];
};
struct sfh_tool {
    char text[TOOL_BYTES];
};
struct sfh_comment {
    char text[COMMENT_BYTES];
};
struct sfh_mux {
    Sint32 size;
    Uint8 pack_type, reserved[3];
    /* the bytes of a packet's length field */
    Sint16 length_bytes;
    Uint8 padding[2];
    Sint32 pack_size;
};
struct sfh_streams {
    Uint8 count, audio, video, private_streams;
    Sint32 byte_rate, max_audio_length, max_video_length, max_frames;
};
struct sfh_header {
    Uint8 reserved_00[32];
    char name[24];
    Uint8 reserved_38[8];
    struct sfh_file file;
    Uint8 reserved_58[8];
    struct sfh_tool tool;
    struct sfh_mux mux;
    Uint8 reserved_90[32];
    struct sfh_streams streams;
    Uint8 reserved_c4[28];
    struct sfh_comment comment;
    Uint8 reserved_120[96];
    struct sfh_element elements[STREAMS];
};
struct sfh_pool {
    Sint32 count, used;
    struct sfh_handle *handles;
};

/* the library's version and copyright */
static const char sfh_version[] = "SFH Version 1.06 1999-10-08";
static const char sfh_copyright[] = "Copyright (c) 1999 CRI     ";
/* the version and build time SFH_GetSbverStr returns. the member exports it,
 * though no header of the SDK declares it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
const char SFH_sbver_str[] =
    "\r\nCRI SFH  Ver.1.06 Build:Oct 08 1999 19:24:14\r\n";
/* SFH_GetSbverStr's string, which SFH_Init keeps here */
static const char *sfh_sbver;
static struct sfh_pool sfh_pool;

static void sfh_reset_pool(struct sfh_pool *pool);
static void sfh_init_pool(struct sfh_pool *pool, Sint32 count,
                          struct sfh_handle *handles);
static void sfh_clear_obj(struct sfh_handle *handle);
static void sfh_set_obj(struct sfh_handle *handle,
                        const struct sfh_header *header, Uint32 size);
static void sfh_clear_objs(Sint32 count, struct sfh_handle *handles);
static Bool sfh_is_free(const struct sfh_handle *handle);
static Bool sfh_is_valid(const struct sfh_handle *handle);
static Bool sfh_is_analyzed(const struct sfh_handle *handle);
static const struct sfh_element *
sfh_search_elem(const struct sfh_header *header, Sint32 stream);
static Bool sfh_parse_tool_ver(char *tool, Sint32 *major, Sint32 *minor);
static char *sfh_parse_dec(char *text, Sint32 *out);
static const struct sfh_element *sfh_get_elem(struct sfh_handle *handle,
                                              Sint32 stream);
static Sint32 sfh_pic_rate(Sint32 code);
static Bool sfh_is_aud_ftr(Sint32 stream, const struct sfh_element *element);
static Bool sfh_is_vid_ftr(Sint32 stream, const struct sfh_element *element);
static Sint32 sfh_stm_type(Sint32 stream);

const char *SFH_GetSbverStr(void)
{
    return SFH_sbver_str;
}
void SFH_Init(Sint32 count, struct sfh_handle *handles)
{
    sfh_sbver = SFH_GetSbverStr();
    sfh_clear_objs(count, handles);
    sfh_init_pool(&sfh_pool, count, handles);
}
void SFH_Finish(void)
{
    sfh_reset_pool(&sfh_pool);
}
struct sfh_handle *SFH_Create(const void *data, Sint32 size)
{
    struct sfh_handle *handles;
    struct sfh_handle *handle;
    Sint32 used;
    Sint32 count;
    Sint32 i;
    /* @hack spelling: the pool's fields read into locals, where reading
     * sfh_pool's would do */
    count = sfh_pool.count;
    used = sfh_pool.used;
    handles = sfh_pool.handles;
    if (used >= count) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        handle = &handles[i];
        if (sfh_is_free(handle)) {
            break;
        }
    }
#ifdef NONMATCHING
    /* this side returns NULL where no handle is free, though used is below
     * count. the matching side then takes the last handle, which is in use */
    if (i >= count) {
        return NULL;
    }
#endif
    sfh_set_obj(handle, data, size);
    sfh_pool.used++;
    return handle;
}
void SFH_Destroy(struct sfh_handle *handle)
{
    sfh_clear_obj(handle);
    sfh_pool.used--;
}
static void sfh_reset_pool(struct sfh_pool *pool)
{
    pool->count = 0;
    pool->used = 0;
    pool->handles = NULL;
}
static void sfh_init_pool(struct sfh_pool *pool, Sint32 count,
                          struct sfh_handle *handles)
{
    pool->count = count;
    pool->used = 0;
    pool->handles = handles;
}
static void sfh_clear_obj(struct sfh_handle *handle)
{
    handle->status = FREE;
    handle->header = NULL;
    handle->size = 0;
    handle->version = 0;
}
static void sfh_set_obj(struct sfh_handle *handle,
                        const struct sfh_header *header, Uint32 size)
{
    handle->status = OPEN;
    handle->header = header;
    handle->size = size;
}
static void sfh_clear_objs(Sint32 count, struct sfh_handle *handles)
{
    Sint32 i;
    for (i = 0; i < count; i++) {
        sfh_clear_obj(&handles[i]);
    }
}
static Bool sfh_is_free(const struct sfh_handle *handle)
{
    /* @hack spelling: the result held in free_slot, where returning the test
     * would do */
    Bool free_slot = handle->status == FREE ? TRUE : FALSE;
    return free_slot;
}
static Bool sfh_is_valid(const struct sfh_handle *handle)
{
    if (!sfh_is_analyzed(handle)) {
        return FALSE;
    }
    if (handle->version != OLD_VERSION &&
        handle->version < SFH_FEATURE_VERSION) {
        return FALSE;
    }
    return TRUE;
}
static Bool sfh_is_analyzed(const struct sfh_handle *handle)
{
    /* @hack spelling: a switch that returns FALSE for each other status,
     * where testing for SOFDEC would do */
    switch (handle->status) {
    case INVALID:
    case FREE:
    case OPEN:
        return FALSE;
    }
    return TRUE;
}
Bool SFH_IsSfdHeader(struct sfh_handle *handle, Bool *is_sfd)
{
    struct sfh_handle *reader;
    Sint32 major;
    Sint32 minor;
    Bool success;
    Sint32 version;
    const char *name;
    *is_sfd = FALSE;
    /* @hack spelling: reader, a copy of handle, where handle would do */
    reader = handle;
    /* @hack spelling: the name's address as the header's plus the size of
     * reserved_00, where reader->header->name would do */
    name = (const char *)reader->header + sizeof(reader->header->reserved_00);
    /* @hack spelling: sfh_is_free's result compared with TRUE, where testing
     * it would do */
    if (sfh_is_free(reader) == TRUE) {
        return FALSE;
    }
    if (reader->size < sizeof(struct sfh_header)) {
        reader->status = INVALID;
        return FALSE;
    }
    if (memcmp(name, "SofdecStream            ",
               sizeof(reader->header->name)) != 0) {
        reader->status = INVALID;
        return FALSE;
    }
    reader->status = SOFDEC;
    /* @hack spelling: the result held in success, where testing the call
     * would do */
    success = SFH_AnlyHdrToolVer(handle, &major, &minor);
    if (!success) {
        return FALSE;
    }
    /* @hack spelling: the version held in a local, where storing it would
     * do */
    version = SFH_VERSION(major, minor);
    reader->version = version;
    *is_sfd = TRUE;
    return TRUE;
}
Bool SFH_IsExistStmId(struct sfh_handle *handle, Uint8 stream, Bool *exists)
{
    const struct sfh_header *header;
    const struct sfh_element *element;
    *exists = FALSE;
    /* @hack spelling: the header read before the test, where reading it
     * after would do */
    header = handle->header;
    if (!sfh_is_valid(handle)) {
        return FALSE;
    }
    element = sfh_search_elem(header, stream);
    /* @hack spelling: an if and an else that store TRUE and FALSE, where
     * storing the test would do */
    if (element != NULL) {
        *exists = TRUE;
    } else {
        *exists = FALSE;
    }
    return TRUE;
}
Bool SFH_IsEffFtrInf(struct sfh_handle *handle, Uint8 stream, Bool *available)
{
    /* @hack spelling: reader, a copy of handle, where handle would do */
    struct sfh_handle *reader = handle;
    const struct sfh_element *element;
    /* @hack spelling: the version held in a local, where the test could read
     * reader->version */
    Sint32 version = reader->version;
    if (version < SFH_FEATURE_VERSION) {
        return FALSE;
    }
    switch (sfh_stm_type(stream)) {
    case MPS_STREAM_AUDIO_FIRST:
        element = sfh_get_elem(handle, stream);
        if (element == NULL) {
            return FALSE;
        }
        *available = sfh_is_aud_ftr(stream, element);
        break;
    case MPS_STREAM_VIDEO_FIRST:
        element = sfh_get_elem(handle, stream);
        if (element == NULL) {
            return FALSE;
        }
        *available = sfh_is_vid_ftr(stream, element);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
static const struct sfh_element *
sfh_search_elem(const struct sfh_header *header, Sint32 stream)
{
    const struct sfh_element *element;
    const struct sfh_element *found;
    Sint32 i;
    Uint8 id;
    /* @hack spelling: the match kept in found past a break, and i tested for
     * none, where returning from the loop would do */
    for (i = 0; i < STREAMS; i++) {
        element = &header->elements[i];
        id = element->id;
        if (id == stream) {
            found = element;
            break;
        }
    }
    if (i == STREAMS) {
        found = NULL;
    }
    return found;
}

#define TEXT_QUERY(name, record_type, member, field, valid)                    \
    Bool name(struct sfh_handle *handle, char *out)                            \
    {                                                                          \
        /* @hack spelling: the header and the record read before the test,     \
         * where reading them after it would do */                             \
        const struct sfh_header *header = handle->header;                      \
        const struct record_type *source = &header->member;                    \
        if (!valid(handle)) {                                                  \
            return FALSE;                                                      \
        }                                                                      \
        memset(out, 0, sizeof(source->field) + 1);                             \
        memcpy(out, source->field, sizeof(source->field));                     \
        return TRUE;                                                           \
    }
TEXT_QUERY(SFH_AnlySfdFname, sfh_file, file, filename, sfh_is_valid)
TEXT_QUERY(SFH_AnlySfdTimStmp, sfh_file, file, timestamp, sfh_is_valid)
TEXT_QUERY(SFH_AnlyHdrToolInf, sfh_tool, tool, text, sfh_is_analyzed)
Bool SFH_AnlyHdrToolVer(struct sfh_handle *handle, Sint32 *major, Sint32 *minor)
{
    char tool[TOOL_BYTES + 1];
    /* @hack spelling: each result held in success and tested, where
     * returning sfh_parse_tool_ver's would do */
    Bool success = SFH_AnlyHdrToolInf(handle, tool);
    if (!success) {
        return FALSE;
    }
    success = sfh_parse_tool_ver(tool, major, minor);
    if (!success) {
        return FALSE;
    }
    return TRUE;
}
TEXT_QUERY(SFH_AnalyHdrComment, sfh_comment, comment, text, sfh_is_valid)
#define HEADER_QUERY(name, record_type, member, field)                         \
    Bool name(struct sfh_handle *handle, Sint32 *value)                        \
    {                                                                          \
        /* @hack spelling: the header and the record read before the test,     \
         * where reading the field after it would do */                        \
        const struct sfh_header *header = handle->header;                      \
        const struct record_type *source = &header->member;                    \
        if (!sfh_is_valid(handle)) {                                           \
            return FALSE;                                                      \
        }                                                                      \
        *value = source->field;                                                \
        return TRUE;                                                           \
    }
HEADER_QUERY(SFH_AnlyHdrSiz, sfh_mux, mux, size)
HEADER_QUERY(SFH_AnlyPackType, sfh_mux, mux, pack_type)
HEADER_QUERY(SFH_AnlyPketSizLen, sfh_mux, mux, length_bytes)
HEADER_QUERY(SFH_AnlyPackSiz, sfh_mux, mux, pack_size)

static Bool sfh_parse_tool_ver(char *tool, Sint32 *major, Sint32 *minor)
{
    char *number;
    /* strpbrk finds the first of V, e, r and ., not the string Ver. */
    number = strpbrk(tool, "Ver.");
    if (number == NULL) {
        return FALSE;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below, with the test of the NUL further
     * on: it refuses a mark with no room for its VERSION_MARK_BYTES before the
     * field's end */
    if (tool + TOOL_BYTES - number < VERSION_MARK_BYTES) {
        return FALSE;
    }
#endif
    /* @bug memory: reads past the 33 bytes at tool where strpbrk finds tool[28]
     * to tool[31], or where the major number runs to tool[31]. this step and
     * the step past the major number each pass over the NUL at tool[32]
     * unchecked (0x8c54f9c0) */
    number += VERSION_MARK_BYTES;
    number = sfh_parse_dec(number, major);
#ifdef NONMATCHING
    /* this side repairs the bug tagged above, with the test of the mark's room:
     * it stops at the NUL past the major number */
    if (*number == '\0') {
        return FALSE;
    }
#endif
    number++;
    sfh_parse_dec(number, minor);
    return TRUE;
}
static char *sfh_parse_dec(char *text, Sint32 *out)
{
    Sint32 value;
    Sint32 digit;
    char *cursor = text;
    for (value = 0;; value = (value * 10) + digit - '0', cursor++) {
        digit = *cursor;
        /* @hack redundant: the test for ., a blank and NUL, which isdigit
         * refuses too */
        if (digit == '.' || digit == ' ' || digit == '\0') {
            break;
        }
        if (!isdigit(digit)) {
            break;
        }
    }
    *out = value;
    return cursor;
}
HEADER_QUERY(SFH_AnlyNumElemTot, sfh_streams, streams, count)
HEADER_QUERY(SFH_AnlyNumElemAud, sfh_streams, streams, audio)
HEADER_QUERY(SFH_AnlyNumElemVid, sfh_streams, streams, video)
HEADER_QUERY(SFH_AnlyNumElemPrv, sfh_streams, streams, private_streams)
Bool SFH_AnlyByteRate(struct sfh_handle *handle, Sint32 *value)
{
    /* @hack spelling: the header and its streams read before the test, where
     * reading them after would do */
    const struct sfh_header *header = handle->header;
    const struct sfh_streams *streams = &header->streams;
    Sint32 version;
    if (!sfh_is_valid(handle)) {
        return FALSE;
    }
    /* @hack spelling: the version held in a local, where the test could read
     * handle->version */
    version = handle->version;
    if (version < SFH_FEATURE_VERSION) {
        return FALSE;
    }
    *value = streams->byte_rate;
    return TRUE;
}
HEADER_QUERY(SFH_AnlyMaxPlyLenAud, sfh_streams, streams, max_audio_length)
HEADER_QUERY(SFH_AnlyMaxPlyLenVid, sfh_streams, streams, max_video_length)
HEADER_QUERY(SFH_AnlyMaxFrmNum, sfh_streams, streams, max_frames)

#define STREAM_TEXT(name, field)                                               \
    Bool name(struct sfh_handle *handle, Uint8 stream, char *out)              \
    {                                                                          \
        const struct sfh_element *element = sfh_get_elem(handle, stream);      \
        if (element == NULL) {                                                 \
            return FALSE;                                                      \
        }                                                                      \
        memset(out, 0, sizeof(element->field) + 1);                            \
        memcpy(out, element->field, sizeof(element->field));                   \
        return TRUE;                                                           \
    }
STREAM_TEXT(SFH_AnlyElemFname, filename)
STREAM_TEXT(SFH_AnlyElemTimStmp, timestamp)
static const struct sfh_element *sfh_get_elem(struct sfh_handle *handle,
                                              Sint32 stream)
{
    /* @hack spelling: the header read before the test, where reading it
     * after would do */
    const struct sfh_header *header = handle->header;
    const struct sfh_element *element;
    if (!sfh_is_valid(handle)) {
        return NULL;
    }
    /* @hack spelling: the result held in element, where returning the call
     * would do */
    element = sfh_search_elem(header, stream);
    return element;
}
/* tests of the stream and element of the query they stand in */
#define IS_AUDIO (sfh_stm_type(stream) == MPS_STREAM_AUDIO_FIRST)
#define IS_VIDEO (sfh_stm_type(stream) == MPS_STREAM_VIDEO_FIRST)
#define HAS_VIDEO_FEATURES sfh_is_vid_ftr(stream, element)
#define STREAM_QUERY(name, valid, expression)                                  \
    Bool name(struct sfh_handle *handle, Uint8 stream, Sint32 *value)          \
    {                                                                          \
        const struct sfh_element *element = sfh_get_elem(handle, stream);      \
        if (element == NULL || !(valid)) {                                     \
            return FALSE;                                                      \
        }                                                                      \
        *value = expression;                                                   \
        return TRUE;                                                           \
    }
STREAM_QUERY(SFH_AnlyElemCodecAud, IS_AUDIO, element->codec)
STREAM_QUERY(SFH_AnlyElemLayer, IS_AUDIO && element->codec == MPEG_AUDIO,
             element->format.audio.layer)
STREAM_QUERY(SFH_AnlyElemChNum, IS_AUDIO, element->format.audio.channels)
STREAM_QUERY(SFH_AnlyElemSmpHz, IS_AUDIO, element->media.sample_rate)
STREAM_QUERY(SFH_AnlyElemCodecVid, IS_VIDEO, element->codec)
STREAM_QUERY(SFH_AnlyElemBitRate, IS_VIDEO,
             (element->format.bit_rate * BIT_RATE_BITS))

Bool SFH_AnlyElemPicSz(struct sfh_handle *handle, Uint8 stream, Sint32 *width,
                       Sint32 *height)
{
    const struct sfh_element *element = sfh_get_elem(handle, stream);
    const Uint8 *size;
    if (element == NULL || !IS_VIDEO) {
        return FALSE;
    }
    size = element->media.video.dimensions;
    /* @hack spelling: each dimension built through its output in three
     * steps, where one expression would do */
    *width = size[0];
    *width = (*width << SHARED_BITS) | ((size[1] >> SHARED_BITS) & SHARED_MASK);
    *width = *width & DIMENSION_MASK;
    *height = size[1];
    *height = (*height << 8) | size[2];
    *height = *height & DIMENSION_MASK;
    return TRUE;
}
STREAM_QUERY(SFH_AnlyElemPicRate, IS_VIDEO,
             sfh_pic_rate(element->media.video.picture_rate))
STREAM_QUERY(SFH_AnlyFtrColType, HAS_VIDEO_FEATURES, element->color)
STREAM_QUERY(SFH_AnlyFtrPicType, HAS_VIDEO_FEATURES, element->picture_type)
#ifdef NONMATCHING
/* GCC lays bit-fields out from the low bit, so this side reads SHC's by mask */
#define FIXED_FLAG(element)                                                    \
    ((((const Uint8 *)&(element)->flags)[0] & 0x80) != 0)
#define SHC_FIXED_FLAG(element)                                                \
    ((((const Uint8 *)&(element)->flags)[0] & 0x08) != 0)
#else
#define FIXED_FLAG(element) ((element)->flags.fixed)
#define SHC_FIXED_FLAG(element) ((element)->flags.fixed_sequence)
#endif
STREAM_QUERY(SFH_AnlyFtrFixFlg, HAS_VIDEO_FEATURES, FIXED_FLAG(element))
STREAM_QUERY(SFH_AnlyFtrShcFixFlg, HAS_VIDEO_FEATURES, SHC_FIXED_FLAG(element))
STREAM_QUERY(SFH_AnlyFtrExpand, HAS_VIDEO_FEATURES, element->expand)
#define GOP_QUERY(name, field)                                                 \
    Bool name(struct sfh_handle *handle, Uint8 stream, Sint32 *value)          \
    {                                                                          \
        const struct sfh_element *element = sfh_get_elem(handle, stream);      \
        if (element == NULL || !HAS_VIDEO_FEATURES) {                          \
            return FALSE;                                                      \
        }                                                                      \
        *value = element->field;                                               \
        if (*value > GOP_MOST) {                                               \
            *value = GOP_UNKNOWN;                                              \
        }                                                                      \
        return TRUE;                                                           \
    }
GOP_QUERY(SFH_AnlyFtrGopN, gop_n)
GOP_QUERY(SFH_AnlyFtrGopM, gop_m)
/* an MPEG picture rate code's rate, in pictures per 1000 seconds. a code MPEG
 * doesn't define gives 0 */
static Sint32 sfh_pic_rate(Sint32 code)
{
    Sint32 rate;
    switch (code) {
    case 1:
        rate = 23976;
        break;
    case 2:
        rate = 24000;
        break;
    case 3:
        rate = 25000;
        break;
    case 4:
        rate = 29970;
        break;
    case 5:
        rate = 30000;
        break;
    case 6:
        rate = 50000;
        break;
    case 7:
        rate = 59940;
        break;
    case 8:
        rate = 60000;
        break;
    default:
        rate = 0;
        break;
    }
    return rate;
}
#define FEATURE_TEST(name, type)                                               \
    static Bool name(Sint32 stream, const struct sfh_element *element)         \
    {                                                                          \
        if (sfh_stm_type(stream) != (type)) {                                  \
            return FALSE;                                                      \
        }                                                                      \
        /* @hack spelling: tests above FEATURE_FORMAT and of 0, where a test   \
         * of FEATURE_FORMAT would do */                                       \
        if (element->feature_version > FEATURE_FORMAT) {                       \
            return FALSE;                                                      \
        }                                                                      \
        if (element->feature_version == 0) {                                   \
            return FALSE;                                                      \
        }                                                                      \
        return TRUE;                                                           \
    }
FEATURE_TEST(sfh_is_aud_ftr, MPS_STREAM_AUDIO_FIRST)
FEATURE_TEST(sfh_is_vid_ftr, MPS_STREAM_VIDEO_FIRST)

/* a stream's kind, as the first id of its range. either private stream is
 * MPS_STREAM_PRIVATE1, and an id of no range is 0 */
static Sint32 sfh_stm_type(Sint32 stream)
{
    if (stream >= MPS_STREAM_AUDIO_FIRST && stream <= MPS_STREAM_AUDIO_LAST) {
        return MPS_STREAM_AUDIO_FIRST;
    }
    if (stream >= MPS_STREAM_VIDEO_FIRST && stream <= MPS_STREAM_VIDEO_LAST) {
        return MPS_STREAM_VIDEO_FIRST;
    }
    if (stream == MPS_STREAM_PRIVATE1 || stream == MPS_STREAM_PRIVATE2) {
        return MPS_STREAM_PRIVATE1;
    }
    return 0;
}

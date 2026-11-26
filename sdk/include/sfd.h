/* the Sofdec decoder's records and the calls between its members. a
 * handle's stages, enum sfd_stage, carry the stream from input through
 * demultiplexing and decoding to output. they pass it on through the buffers
 * enum sfd_buffer_id names: bytes, decoded pictures and PCM samples */
#ifndef SFD_H
#define SFD_H
#include <sg_xpt.h>
#include <sj.h>
#include <sofdec.h>

struct sfd_handle;
struct mps_handle;
struct _adx_talk;
struct _adxrna_obj;

enum sfd_stage {
    SFD_INPUT,
    SFD_SYSTEM,
    SFD_VIDEO,
    SFD_AUDIO,
    SFD_VIDEO_POST,
    SFD_AUDIO_POST,
    SFD_VIDEO_OUT,
    SFD_AUDIO_OUT,
    SFD_USER,
    SFD_STAGES
};
enum sfd_buffer_id {
    SFD_SYSTEM_BYTES,
    SFD_VIDEO_BYTES,
    SFD_AUDIO_BYTES,
    SFD_VIDEO_FRAMES,
    SFD_AUDIO_SAMPLES,
    SFD_POST_FRAMES,
    SFD_POST_SAMPLES,
    SFD_USER_DATA,
    SFD_BUFFERS
};
/* SFD_NO_STAGE is a buffer's writer or reader while no stage is connected to
 * it. SFD_NO_BUFFER is a stage's input or output while no buffer is */
#define SFD_NO_STAGE SFD_STAGES
#define SFD_NO_BUFFER SFD_BUFFERS
enum sfd_operation {
    SFD_OP_INIT,
    SFD_OP_FINISH,
    SFD_OP_EXEC,
    SFD_OP_CREATE,
    SFD_OP_DESTROY,
    SFD_OP_STANDBY,
    SFD_OP_START,
    SFD_OP_STOP,
    SFD_OP_PAUSE,
    SFD_OP_GET_WRITE,
    SFD_OP_ADD_WRITE,
    SFD_OP_GET_READ,
    SFD_OP_ADD_READ,
    SFD_OP_SEEK,
    SFD_OPERATIONS
};
enum sfd_state {
    SFD_FREE,
    SFD_STOPPED,
    SFD_PREPARING,
    SFD_STANDBY,
    SFD_PLAYING,
    SFD_PAUSED,
    SFD_FINISHED
};

#define SFD_COND_VIDEO 5
#define SFD_COND_AUDIO 6
#define SFD_COND_VIDEO_CHANNEL 29
#define SFD_COND_AUDIO_CHANNEL 30
/* the channel condition under which the system stage takes every channel */
#define SFD_ANY_CHANNEL (-1)

#define SFD_COND_DISPLAY_ALWAYS 14
#define SFD_COND_DISPLAY_DELAY 44

#define SFD_COND_DECODE_I 2
#define SFD_COND_DECODE_P 3
#define SFD_COND_DECODE_B 4
#define SFD_COND_DECODE_EMPTY_B 7
/* whether sfply_ResetHn keeps what was written to a handle's input, where
 * SFD_Create gave the handle no work. the input then holds those bytes again,
 * and no more */
#define SFD_COND_REFEED 8
/* the clock that times the pictures, an enum sfd_clock_source */
#define SFD_COND_CLOCK 15
/* the time a playing handle finishes at, in units of 1/SFD_COND_END_UNIT
 * second, or SFD_NO_END_TIME */
#define SFD_COND_END_TIME 20
#define SFD_COND_END_UNIT 21
#define SFD_NO_END_TIME (-4)
/* the frames the video stage decodes before it marks its output prepared, or
 * SFD_ALL_FRAMES */
#define SFD_COND_PREP_FRAMES 23
#define SFD_ALL_FRAMES (-1)
/* the outputs whose end finishes a playing handle, an enum sfd_end_trigger */
#define SFD_COND_END_TRIGGER 25
/* SFD_COND_END_TRIGGER's values. SFD_END_AUDIO and SFD_END_VIDEO are bits.
 * SFD_END_EITHER, both bits, finishes the handle when either output ends.
 * SFD_END_BOTH, neither bit, waits for both */
enum sfd_end_trigger {
    SFD_END_BOTH,
    SFD_END_AUDIO,
    SFD_END_VIDEO,
    SFD_END_EITHER
};
/* whether the audio stage records its player's errors as the handle's */
#define SFD_COND_AUDIO_ERRORS 26
/* the audio player's server calls a second at normal speed or slower */
#define SFD_COND_AUDIO_SERVER_FREQUENCY 27
/* whether the video stage takes the decoder's luma offset from the stream
 * header's color type */
#define SFD_COND_Y16 28
/* the functions called with each picture decoded and each picture skipped,
 * or NULL */
#define SFD_COND_DECODE_CALLBACK 36
#define SFD_COND_SKIP_CALLBACK 37
/* the late pictures skipped in a row, at most, at normal speed or slower */
#define SFD_COND_LATE_SKIPS 38
#define SFD_COND_DECODE_ALL 39
/* the time a picture takes to decode, in units of 1/SFD_COND_DECODE_UNIT
 * second */
#define SFD_COND_DECODE_TIME 42
#define SFD_COND_DECODE_UNIT 43
/* how far ahead of its time SFD_IsDrawTime finds a picture due. it is also
 * how long sfply_IsStartSync lets a handle wait for its first picture to
 * show. it is in ticks of the library's clock */
#define SFD_COND_DRAW_DELAY 45
/* the margin either side of now under SFD_CLOCK_AUDIO. a picture within it
 * may be judged as the last one within it was. SFD_IsDrawTime and
 * SFD_GetFrm judge so. it is in tenths of a millisecond */
#define SFD_COND_DRAW_MARGIN 46
/* whether decoding can resume after a seek at a group start, not only at a
 * sequence header */
#define SFD_COND_SEEK_GROUP 48

/* SFD_COND_CLOCK's clocks. SFD_CLOCK_VSYNC counts the vertical blanks at the
 * handle's speed, and SFD_CLOCK_AUDIO the audio stage's samples. the user
 * clocks are the application's frames and function */
enum sfd_clock_source {
    SFD_CLOCK_NONE,
    SFD_CLOCK_VSYNC,
    SFD_CLOCK_AUDIO,
    SFD_CLOCK_USER_FRAME,
    SFD_CLOCK_USER
};
/* the latest time, which a time not known yet holds */
#define SFD_NO_TIME 0x7fffffff
/* the unit of a time that counts no ticks. such a time is a sentinel, or the
 * 0 or SFD_NO_TIME of a time not known yet */
#define SFD_NO_UNIT 1
/* struct sfd_seek_time's time while no seek is asked for */
#define SFD_NO_SEEK (-8)
/* the speed of normal play, in the thousandths of it SFD_SetSpeed takes */
#define SFD_NORMAL_SPEED 1000

#define SFD_CONDITIONS 64
#define SFD_FRAME_POOL 16
#define SFD_FILE_HISTORY 32
#define SFD_COND_CONCAT 49
#define SFD_COND_PLAY_MODE 47
/* SFD_COND_PLAY_MODE's modes. under SFD_PLAY_HEADERS, which
 * SFD_StartHeadAnaly sets, the video stage decodes no picture and SFD_Start
 * only stands the handle by */
#define SFD_PLAY_NORMAL 0
#define SFD_PLAY_HEADERS 1
#define SFD_TRANSFER_LIST 15

typedef Sint32 (*sfd_stage_fn)(struct sfd_handle *handle, Sint32 first,
                               Sint32 second, Sint32 third);
struct sfd_stage_ops {
    sfd_stage_fn apply[SFD_OPERATIONS];
};
struct sfd_stage_list {
    const struct sfd_stage_ops *entries[SFD_TRANSFER_LIST];
};
struct sfd_setup {
    const struct sfd_stage_ops *entries[SFD_STAGES];
};
struct sfd_create_params {
    const struct sfd_setup *setup;
    char *work;
    Sint32 sizes[SFD_BUFFERS];
    Sint32 read_unit, frames, width, height, serial;
};
struct sfd_init_params {
    const struct sfd_stage_list *stages;
    struct sfd_handle *handles;
    Sint32 count, time_unit;
};
struct sfd_video_params {
    Sint32 chroma_width, chroma_height, luma_width, luma_height;
    Uint8 *references;
    Sint32 output_width, output_height, frames;
    Uint8 *output;
};
/* the alignment the library gives a handle's work and the video stage's
 * references and output */
#define SFD_ALIGNMENT 32
/* the bytes of a 4:2:0 picture: its luma, and two chroma planes a quarter of
 * its size each */
#define SFD_PICTURE_BYTES(width, height) ((width) * (height) * 3 / 2)
struct sfd_audio_params {
    Sint32 ring_size, margin;
    char *ring;
    Sint32 channels, sample_rate, work_size;
    char *work;
};
struct sfd_error {
    void (*report)(void *user, Sint32 code);
    void *user;
    Sint32 code, detail, extra;
};
struct sfd_clock {
    volatile Sint32 vsyncs;
    Sint32 reserved, unit;
};

struct sfd_system_header {
    Sint32 elements, audio, video, private_streams;
    Sint32 max_audio_length, max_video_length, max_frames;
};
struct sfd_audio_header {
    Sint32 codec, layer, channels, sample_rate;
};
struct sfd_video_header {
    Sint32 codec, bit_rate, width, height, picture_rate;
    Sint32 features, color, picture_type, fixed, fixed_shc, expand, gop_n,
        gop_m;
};
struct sfd_file_header {
    Bool valid;
    Sint32 major, minor, byte_rate, header_size, pack_type, length_bytes,
        pack_size;
    struct sfd_system_header system;
    Sint32 private1_id, private2_id, audio_id, video_id;
    struct sfd_audio_header audio;
    struct sfd_video_header video;
    Sint32 raw_size;
    Uint8 raw[2048];
};
struct sfd_movie_info {
    Sint32 width, height, mb_width, mb_height, bit_rate, picture_rate, mux_rate;
    Bool fixed;
    Sint32 reserved[8];
};
struct sfd_play_info {
    Sint32 decoded, skipped, dct_intra, dct_inter, empty_pictures;
    Sint32 reserved[11];
};
struct sfd_timecode {
    Sint32 rate, drop, hour, minute, second, frame, frame_adjust;
};
struct sfd_time {
    Bool valid;
    struct sfd_timecode code;
    Sint32 time, unit;
};
typedef Sint32 (*sfd_time_fn)(struct sfd_handle *handle, Sint32 *time,
                              Sint32 *unit);
struct sfd_timer {
    sfd_time_fn sources[5];
    sfd_stage_fn skip;
    struct sfd_time first_picture, latest_picture, current_picture,
        skipped_picture, start;
    Sint32 start_time, start_unit, concat_offset, supply_time, concat_count;
    Sint32 history[SFD_FILE_HISTORY];
    Sint32 end_time, end_unit, now_time, now_unit, last_input_time, interval;
    Sint32 max_delta, min_delta, user_frames;
    volatile Sint32 vsyncs;
    Sint32 speed, display_count, display_time, display_unit, hold_count;
    Float last_exec;
    Sint32 last_result;
    Float judged_exec;
    volatile Sint32 vblank_time;
};
struct sfd_span {
    char *data;
    Sint32 size;
};
struct sfd_ring_info {
    struct sfd_span head, wrap;
    Bool closed;
    Sint32 written, read;
};
struct sfd_ring {
    char *data;
    Sint32 size;
    char *end;
    Sint32 margin;
    char *write, *read, *delimiter, *scan, *margin_end;
    Sint32 copied, total_written, total_read, fill;
    Bool closed;
};
struct sfd_frame_info {
    Sint32 width, height, mb_width, mb_height, coding_type, time, time_unit,
        serial;
    Uint8 *image;
    Sint32 flags, reserved, sequence;
};
/* struct sfd_frame_slot's states. a slot is SFD_SLOT_HELD while the
 * application holds its frame, from SFD_GetFrm until SFD_RelFrm */
enum sfd_slot_state { SFD_SLOT_FREE, SFD_SLOT_HELD };
struct sfd_frame_slot {
    Sint32 state;
    struct sfd_frame_info frame;
};
struct sfd_frame_queue {
    char *data;
    Sint32 size, reserved[2];
    struct sfd_frame_slot slots[SFD_FRAME_POOL];
};
struct sfd_pcm_info {
    Sint32 rate, bits, channels, count[2];
    char *head[2], *wrap[2];
    Sint32 written, read;
};
struct sfd_pcm_ring {
    char *data;
    Sint32 size, rate, bits, channels;
    char *left, *right;
    Sint32 samples, write, read, total_written, total_read;
};
#define SFD_USER_CHANNELS 3
struct sfd_user_channel {
    SJ stream;
    void (*notify)(struct sfd_handle *, Sint32);
};
struct sfd_user_outputs {
    struct sfd_user_channel channels[SFD_USER_CHANNELS];
};
struct sfd_buffer {
    Sint32 kind;
    Bool has_storage, prepared, ended;
    union {
        struct sfd_ring bytes;
        struct sfd_frame_queue frames;
        struct sfd_pcm_ring pcm;
        struct sfd_user_outputs user;
    } data;
    Sint32 writer, reader;
};

struct sfd_buffer_table {
    struct sfd_buffer records[SFD_BUFFERS];
};

struct sfd_output_frame {
    Sint32 state;
    Uint8 *pixels;
    Sint32 time, unit, sequence;
    struct mpv_picture picture;
};
struct sfd_video_work {
    struct mpv_obj *decoder;
    struct sfd_video_params params;
    struct sfd_output_frame *held;
    Sint32 defect, resume_codes;
    Bool ended;
    /* TRUE from a sequence header, group start or sequence end code until
     * the next decoded picture */
    Bool at_boundary;
    Sint32 late, sequence_ends;
    struct mpv_picture picture;
    Sint32 gop, first_reference, second_reference;
    struct mpv_frame references[2];
    struct sfd_output_frame frames[SFD_FRAME_POOL];
};
struct sfd_audio_work {
    struct _adx_talk *player;
    SJ stream;
    struct sfd_audio_params params;
    Sint32 time, unit, awaits_start, discard, seeked, adjusted;
    void (*copy)(struct sfd_handle *, char *, Sint32, Sint32 *);
    Sint32 no_header, server_frequency;
};
struct sfd_audio_output {
    struct _adxrna_obj *converter;
};
struct sfd_system_work {
    struct mps_handle *parser;
    Sint32 video_count, audio_count, video_pts, audio_pts, concat_count;
};
/* the slots of struct sfd_transfer's outputs. the system stage writes each
 * stream it demultiplexes through one, and any other stage writes through
 * SFD_OUTPUT alone */
enum sfd_system_output {
    SFD_SYSTEM_VIDEO,
    SFD_SYSTEM_AUDIO,
    SFD_SYSTEM_PRIVATE,
    SFD_SYSTEM_OUTPUTS
};
#define SFD_OUTPUT 0
struct sfd_transfer {
    Bool prepared, ended;
    union {
        void *handle;
        struct sfd_video_work video;
        struct sfd_system_work system;
        struct sfd_audio_work audio;
        struct sfd_audio_output audio_output;
        Uint8 bytes[0x524];
    } work;
    const struct sfd_stage_ops *ops;
    Sint32 input, outputs[3];
};
#define SFD_RAW_PACKET_SIZE 176
struct sfd_system_packets {
    Uint8 video[SFD_RAW_PACKET_SIZE], audio[SFD_RAW_PACKET_SIZE];
    Sint32 video_size, audio_size;
};
struct sfd_system_seek {
    Bool saved;
    Sint32 byte_rate, fixed, video_count, audio_count, video_pts, audio_pts;
    struct sfd_system_packets packets;
    Uint8 reserved[0x98];
};
struct sfd_sequence_header {
    Uint8 bytes[512];
    Uint32 size;
};
struct sfd_video_seek {
    Bool valid;
    Sint32 byte_rate;
    Bool constant;
    struct sfd_time first;
    struct sfd_sequence_header sequence;
};
struct sfd_audio_seek {
    Bool valid;
    Sint32 byte_rate;
    Bool constant;
    Sint32 channels, sample_rate, samples;
    Uint8 reserved[128];
};
struct sfd_analysis {
    Bool analyzed;
    Sint32 byte_rate;
    Bool fixed;
    struct sfd_file_header header;
    struct sfd_system_seek system;
    struct sfd_video_seek video;
    struct sfd_audio_seek audio;
};
struct sfd_seek_info {
    struct sfd_analysis head;
    void *positions;
};
struct sfd_seek_time {
    Sint32 reserved, time, unit;
};
struct sfd_seek_state {
    struct sfd_seek_info *info;
    struct sfd_seek_time position;
};
struct sfd_handle {
    struct sfd_create_params params;
    void *work;
    Sint32 status, request, pause;
    struct sfd_file_header header;
    struct sfd_movie_info movie;
    struct sfd_play_info play;
    struct sfd_error error;
    Sint32 conditions[SFD_CONDITIONS];
    struct sfd_timer timer;
    struct sfd_buffer_table buffers;
    struct sfd_transfer stages[SFD_STAGES];
    struct sfd_seek_state seek;
};
struct sfd_library {
    char version[32], copyright[32];
    Sint32 conditions[SFD_CONDITIONS];
    struct sfd_init_params params;
    Uint32 server_status;
    struct sfd_error error;
    struct sfd_clock clock;
    Sint32 buffer_work;
    struct sfd_stage_list stages;
    Sint32 handle_count;
    struct sfd_handle *handles;
};
extern struct sfd_library SFLIB_libwork;
extern const Sint32 SFPLY_cond_dfl[SFD_CONDITIONS + 1];
extern const struct sfd_stage_ops SFD_tr_in_mem, SFD_tr_sd_mps, SFD_tr_vd_mpv;
extern const struct sfd_stage_ops SFD_tr_ad_adxt, SFD_tr_ao_auto_p,
    SFD_tr_vo_manu;

Sint32 SFD_Init(struct sfd_init_params *params);
void SFLIB_InitErrInf(struct sfd_error *error);
Sint32 SFLIB_SetErr(struct sfd_handle *handle, Sint32 code);
Sint32 SFLIB_CheckHn(struct sfd_handle *handle);
void SFLIB_LockCs(Sint32 *mask);
void SFLIB_UnlockCs(const Sint32 *mask);
void SFBUF_Init(Sint32 *work);
void SFBUF_Finish(Sint32 *work);
void SFHDS_Init(void);
void SFHDS_Finish(void);
void SFPLY_Init(void);
void SFTIM_Init(struct sfd_clock *clock, Sint32 unit);
void SFTIM_Finish(struct sfd_clock *clock);
Sint32 SFTRN_Init(struct sfd_stage_list *list,
                  const struct sfd_stage_list *source);
Sint32 SFTRN_Finish(struct sfd_stage_list *list);
Sint32 SFD_Destroy(struct sfd_handle *handle);
/* SFPLY_DecideSvrStat's result, which SFD_ExecServer and SFD_GetSvrStat
 * return. it is BUSY while any handle in use has neither finished nor
 * failed. otherwise it is FINISHED where any handle is in use, and IDLE
 * where none is */
enum sfd_server_status {
    SFD_SERVER_IDLE,
    SFD_SERVER_BUSY,
    SFD_SERVER_FINISHED
};
Sint32 SFPLY_DecideSvrStat(void);
Sint32 SFTRN_IsSetup(struct sfd_handle *handle, Sint32 stage);
Sint32 SFTRN_CallTrtTrif(struct sfd_handle *handle, Sint32 stage,
                         Sint32 operation, Sint32 first, Sint32 second);
void SFSET_SetCond(struct sfd_handle *handle, Sint32 id, Sint32 value);
Sint32 SFSET_GetCond(struct sfd_handle *handle, Sint32 id);
void SFSET_GetTrHn(struct sfd_handle *handle, Sint32 stage, void **out);
void SFRNA_SetSpeed(struct sfd_handle *handle, Sint32 speed);
/* the modes SFTIM_Pause and the stages' SFD_OP_PAUSE take. SFD_STEP, a pause
 * while paused, moves the handle's time on by a video frame */
enum sfd_pause_mode { SFD_RESUME, SFD_PAUSE, SFD_STEP };
void SFTIM_Pause(struct sfd_handle *handle, Sint32 mode);
void SFTIM_SetSpeed(struct sfd_handle *handle, Sint32 speed);
Sint32 SFPL2_Standby(struct sfd_handle *handle);
const char *SFD_GetVersionStr(void);
Sint32 SFD_GetErrInf(struct sfd_handle *handle, struct sfd_error *out);
Sint32 SFD_GetSvrStat(void);
Sint32 SFD_GetHnStat(struct sfd_handle *handle);
Sint32 SFD_SetCond(struct sfd_handle *handle, Sint32 id, Sint32 value);
Sint32 SFD_GetCond(struct sfd_handle *handle, Sint32 id, Sint32 *value);
Sint32 SFD_GetMvInf(struct sfd_handle *handle, struct sfd_movie_info *info);
Sint32 SFD_GetPlyInf(struct sfd_handle *handle, struct sfd_play_info *info);
Sint32 SFD_GetTrHn(struct sfd_handle *handle, Sint32 stage, void **out);
Sint32 SFD_GetPaStat(struct sfd_handle *handle);
Sint32 SFD_Pause(struct sfd_handle *handle, Sint32 paused);
Sint32 SFD_Standby(struct sfd_handle *handle);
Sint32 SFD_SetSpeed(struct sfd_handle *handle, Sint32 speed);
void SFBUF_SetPrepFlg(struct sfd_handle *handle, Sint32 buffer, Bool prepared);
Bool SFBUF_GetPrepFlg(struct sfd_handle *handle, Sint32 buffer);
Bool SFBUF_GetTermFlg(struct sfd_handle *handle, Sint32 buffer);
Sint32 SFBUF_RingGetWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_ring_info *out);
Sint32 SFBUF_RingAddWrite(struct sfd_handle *handle, Sint32 buffer,
                          Sint32 bytes, Sint32 written);
void SFTRN_SetPrepFlg(struct sfd_handle *handle, Sint32 stage, Bool prepared);
Bool SFTRN_GetPrepFlg(struct sfd_handle *handle, Sint32 stage);
void SFTRN_SetTermFlg(struct sfd_handle *handle, Sint32 stage, Bool ended);
Bool SFTRN_GetTermFlg(struct sfd_handle *handle, Sint32 stage);
Sint32 SFTRN_InitHn(struct sfd_handle *handle, struct sfd_transfer *stages,
                    struct sfd_create_params *params, void *work);
Sint32 SFTRN_CallTrSetup(struct sfd_handle *handle, Sint32 operation);
Bool SFTIM_IsVideoTerm(struct sfd_handle *handle);
void SFTIM_IsExecTime(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                      Bool *due, Sint32 delay);
Sint32 SFBUF_VfrmGetRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_frame_info **out);
Sint32 SFBUF_VfrmAddRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_frame_info *frame);
Sint32 SFD_SetConcatPlay(struct sfd_handle *handle);
Bool SFCON_IsEndcodeSkip(struct sfd_handle *handle);
void SFCON_UpdateConcatTime(struct sfd_handle *handle, Sint32 duration);
Sint32 SFD_GetTimePerFile(struct sfd_handle *handle, Sint32 *time, Sint32 *unit,
                          Sint32 *file);
Sint32 SFTIM_GetTimeSub(struct sfd_handle *handle, Sint32 *time, Sint32 *unit);
void SFMPVF_InitRfb(struct sfd_handle *handle);
void SFMPVF_TermDec(struct sfd_handle *handle);
Bool SFMPVF_IsTermDec(struct sfd_handle *handle);
void SFMPVF_SetGopStat(struct sfd_handle *handle, Bool at_boundary);
/* SFMPVF_GetNumFrm's result once decoding has ended and every frame is
 * free. until then, it returns the ready frames */
#define SFMPVF_ENDED (-1)
Sint32 SFMPVF_GetNumFrm(struct sfd_handle *handle);
struct sfd_output_frame *SFMPVF_AllocFrm(struct sfd_handle *handle);
void SFMPVF_FreeFrm(struct sfd_output_frame *frame);
void SFMPVF_StbyFrm(struct sfd_output_frame *frame);
struct sfd_output_frame *SFMPVF_HoldFrm(struct sfd_handle *handle, Bool *last);
Sint32 SFMPS_GetConcatCnt(struct sfd_handle *handle);
void SFHDS_InitFhd(struct sfd_file_header *header);
void SFHDS_FinishFhd(struct sfd_file_header *header);
Bool SFHDS_SetHdr(struct sfd_handle *handle, Sint32 channel, const Uint8 *data,
                  Sint32 size, Bool *set);
void SFHDS_ReprocessHdr(struct sfd_handle *handle);
Sint32 SFHDS_GetPketLenField(void);
Sint32 SFHDS_GetMuxVerNum(struct sfd_handle *handle);
/* SFHDS_GetColType's result where the stream header gives no color type */
#define SFD_NO_COLOR (-1)
Sint32 SFHDS_GetColType(struct sfd_handle *handle);
void SFSEE_InitHn(struct sfd_seek_state *state);
void SFD_InitSeek(struct sfd_seek_info *info, Sint32 count);
Sint32 SFD_EntrySeek(struct sfd_handle *handle, struct sfd_seek_info *info);
Sint32 SFD_SetSeekPosTbl(struct sfd_handle *handle, void *table);
Sint32 SFD_StartHeadAnaly(struct sfd_handle *handle);
Sint32 SFD_IsHeadAnalyEnd(struct sfd_handle *handle, Bool *ended);
void SFSEE_ExecServer(struct sfd_handle *handle);
Sint32 SFD_IsSeekAble(struct sfd_handle *handle, Bool *able);
Sint32 SFD_CnvTimeToPos(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                        Sint32 *position);
Sint32 SFD_CnvPosToTime(struct sfd_handle *handle, Sint32 position,
                        Sint32 *time, Sint32 *unit);
Sint32 SFD_Seek(struct sfd_handle *handle, struct sfd_seek_time *position);
void SFTIM_InitTtu(struct sfd_time *stamp, Sint32 time);
Sint32 SFPLY_Stop(struct sfd_handle *handle);
void SFBUF_InitHn(struct sfd_handle *handle, struct sfd_buffer_table *table,
                  struct sfd_create_params *params);
void SFBUF_FixAringBuf(struct sfd_handle *handle, Sint32 buffer, Sint32 bits,
                       Sint32 channels, Sint32 rate);
void SFBUF_SetUoch(struct sfd_handle *handle, Sint32 buffer, Sint32 channel,
                   const struct sfd_user_channel *source);
void SFBUF_GetUoch(struct sfd_handle *handle, Sint32 buffer, Sint32 channel,
                   struct sfd_user_channel *out);
Sint32 SFBUF_RingGetRead(struct sfd_handle *handle, Sint32 buffer,
                         struct sfd_ring_info *out);
Sint32 SFBUF_RingAddRead(struct sfd_handle *handle, Sint32 buffer,
                         Sint32 bytes);
void SFBUF_RingGetDlm(struct sfd_handle *handle, Sint32 buffer,
                      char **delimiter, char **scan, Sint32 *copied,
                      char **margin_end, Sint32 *margin);
void SFBUF_RingSetDlm(struct sfd_handle *handle, Sint32 buffer, char *delimiter,
                      char *scan, Sint32 copied, char *margin_end);
Sint32 SFBUF_AringGetWrite(struct sfd_handle *handle, Sint32 buffer,
                           struct sfd_pcm_info *out);
Sint32 SFBUF_AringAddWrite(struct sfd_handle *handle, Sint32 buffer,
                           Sint32 samples);
Sint32 SFBUF_AringGetRead(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_pcm_info *out);
Sint32 SFBUF_AringAddRead(struct sfd_handle *handle, Sint32 buffer,
                          Sint32 samples);
Sint32 SFBUF_VfrmGetWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_frame_info **out);
Sint32 SFBUF_VfrmAddWrite(struct sfd_handle *handle, Sint32 buffer,
                          struct sfd_frame_info *frame);
void SFBUF_TakeBuf(struct sfd_handle *handle, Sint32 buffer, char **data,
                   Sint32 *size);
void SFBUF_SetTermFlg(struct sfd_handle *handle, Sint32 buffer, Bool ended);
void SFTIM_VbIn(void);
void SFD_VbIn(void);
void SFD_VbOut(void);
Sint32 SFD_ExecServer(void);
void SFTIM_InitHn(struct sfd_handle *handle, struct sfd_timer *timer);
Sint32 SFTIM_GetTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit);
struct sfd_handle *SFD_Create(struct sfd_create_params *params, void *work);
void SFPLY_AddDecPic(struct sfd_handle *handle, Sint32 count,
                     Sint32 coding_type);
void SFPLY_AddSkipPic(struct sfd_handle *handle, Sint32 count,
                      Sint32 coding_type);
Sint32 SFD_Start(struct sfd_handle *handle);
Sint32 SFD_Stop(struct sfd_handle *handle);
Sint32 sfply_ResetHn(struct sfd_handle *handle);
Sint32 SFD_GetWritePtr(struct sfd_handle *handle, struct sfd_ring_info *out);
Sint32 SFD_AddWritePtr(struct sfd_handle *handle, Sint32 bytes, Sint32 written);
Sint32 SFD_TermSupply(struct sfd_handle *handle);
Sint32 SFD_GetFrm(struct sfd_handle *handle, struct sfd_frame_info **out);
Sint32 SFD_RelFrm(struct sfd_handle *handle, struct sfd_frame_info *frame);
void SFTIM_InitTcode(struct sfd_timecode *code);
void SFTIM_UpdateItime(struct sfd_timer *timer, Sint32 time);
Sint32 SFTIM_GetNextItime(const struct sfd_timer *timer, Sint32 time);
void SFTIM_SetStartTime(struct sfd_timer *timer, Sint32 time, Sint32 unit);
Sint32 SFD_GetTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit);
Sint32 SFD_GetTimeAfterSeek(struct sfd_handle *handle, Sint32 *time,
                            Sint32 *unit);
Sint32 SFD_GetNowTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit);
Sint32 SFTIM_GetNowTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit);
Bool SFTIM_ChkRegularTime(const struct sfd_handle *handle, Sint32 *time,
                          Sint32 *unit);
Sint32 SFD_OutUsrFrmSync(struct sfd_handle *handle);
Sint32 SFD_SetUsrIsSkipFn(struct sfd_handle *handle, sfd_stage_fn skip);
Bool SFD_CmpTime(Sint32 first, Sint32 first_unit, Sint32 second,
                 Sint32 second_unit);
Bool SFD_IsDispTime(Sint32 first, Sint32 first_unit, Sint32 second,
                    Sint32 second_unit);
Sint32 SFD_SetUsrTimeFn(struct sfd_handle *handle, sfd_time_fn source);
void SFTIM_SetTimeFn(struct sfd_handle *handle, sfd_time_fn source,
                     Sint32 slot);
void SFTIM_Tc2Time(const struct sfd_timecode *code, Sint32 *time, Sint32 *unit);
void SFTIM_GetTimeOneFrmVideo(const struct sfd_handle *handle, Sint32 *time,
                              Sint32 *unit);
Sint32 SFD_GetPlayFps(struct sfd_handle *handle, Sint32 *fps);
Sint32 SFD_OutDispSync(struct sfd_handle *handle, Sint32 time, Sint32 unit);
Sint32 SFD_IsDrawTime(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                      Bool *due);
Sint32 SFTIM_GetSpeed(const struct sfd_handle *handle);
void SFD_SetMpvPara(const struct sfd_video_params *params);
Sint32 SFD_SetMpvCond(struct sfd_handle *handle, Sint32 id, Sint32 value);
Sint32 SFD_GetNumFrm(struct sfd_handle *handle, Sint32 *count);
void SFD_SetAdxtPara(const struct sfd_audio_params *params);
#endif

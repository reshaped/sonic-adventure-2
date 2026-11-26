/* the Sofdec middleware player's records, 32 of them in mwg_ply_work, and
 * the calls between the player's members */
#ifndef MWSFD_H
#define MWSFD_H
#include <cri_lsc.h>
#include <sfd_player.h>
#include <sfdfx_mw.h>
#include <sg_mwgsc.h>
#include <sg_mwply.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>
#include <sj.h>

#define MWSFD_MAX_PLAYERS 32

struct mwsfd_player {
    MW_PLY_OBJ base;
    Bool used;
    MWE_PLY_STAT state;
    MWS_PLY_CPRM_SFD parameters;
    Bool created;
    struct sfd_handle *decoder;
    struct gsc_stream *file;
    SJ stream, memory, input;
    /* the SFD_AUDIO_OUT stage's ADXRNA, or null. SFD_GetTrHn stores it
     * through a void ** */
    void *audio_transfer;
    MWRNV2 renderer, alpha_renderer;
    Bool frame_loaded, decoding;
    Sint32 display_type;
    Sint32 reserved[2];
    void *audio_decoder;
    Sint32 brightness, brightness_offset;
    Float display_x, display_y, display_z;
    Float display_width, display_height;
    Sint32 stream_size;
    Bool video_enabled;
    Sint32 composition;
    Uint8 *composition_buffer;
    Sint32 composition_size;
    Sint32 draw_frame;
    mwsfd_effect_fn effect;
    void *effect_user;
    Sint32 load_frame;
    Sint8 linked, link_ended, paused;
    Sint8 reserved_flag;
    char render_work[128], alpha_work[128];
    LSC link;
    Sint32 link_time, link_count;
};

struct mwsfd_error {
    void (*report)(void *user, Sint32 code);
    void *user;
    Sint32 code;
};
struct mwsfd_library {
    Sint32 reserved;
    struct mwsfd_player players[MWSFD_MAX_PLAYERS];
    struct mwsfd_error error;
};

extern struct mwsfd_library mwg_ply_work;
extern Bool mwg_sfd_init_flag, mwg_sleep_enable_flag;
/* the rates mwPlySetDispMode sets mwg_vsync_mhz to, in millihertz */
#define MWSFD_NTSC_MILLIHZ 59940
#define MWSFD_PAL_MILLIHZ 50000
extern Sint32 mwg_vsync_mhz;
extern volatile Sint32 mwg_vcnt;
extern volatile Bool *mwg_eor_flag_ptr;
Sint32 mwPlyLock(void);
void mwPlyUnlock(Sint32 mask);
void mwPlySetDispMode(Sint32 mode, Sint32 frame, Sint32 count, Sint32 latency);
Sint32 mwPlySfdInit(struct mwsfd_player *player, Sint32 type, Sint32 time_unit);
void mwSfdDestroy(MWPLY handle);
/* the codes the player's members record with mwSfdSetErrCode. a -1xx code
 * names the GSC call that failed, and a -3xx code the Sofdec call, but for
 * MWSFD_ERR_FILE_TYPE: a file type the player has no stages for */
#define MWSFD_ERR_PARAMETER (-4)
#define MWSFD_ERR_NO_HANDLE (-11)
#define MWSFD_ERR_NULL_HANDLE (-12)
#define MWSFD_ERR_GSC_INIT (-101)
#define MWSFD_ERR_GSC_OPEN (-102)
#define MWSFD_ERR_GSC_START (-103)
#define MWSFD_ERR_INIT (-301)
#define MWSFD_ERR_FILE_TYPE (-302)
#define MWSFD_ERR_ERROR_FN (-303)
#define MWSFD_ERR_CREATE (-305)
#define MWSFD_ERR_DESTROY (-306)
#define MWSFD_ERR_START (-307)
#define MWSFD_ERR_STOP (-308)
#define MWSFD_ERR_TIME (-309)
#define MWSFD_ERR_PAUSE (-310)
#define MWSFD_ERR_STANDBY (-311)
Sint32 mwSfdSetErrCode(Sint32 code);
void mwSfdExecDecServer(void);
void mwSfdExecServer(void *user);
void mwSfdVsync(void *user);
void mwSfdStartFrame(void *user);

/* the SH-4's cache line, in bytes */
#define MWSFD_CACHE_LINE 32
/* copy bytes from src to dest, or nothing for bytes below 1. where dest and
 * src both start a cache line, the store queues copy bytes / MWSFD_CACHE_LINE
 * whole lines. the rest stays uncopied. that copy needs FPSCR.SZ 0. it drops
 * each line of dest from the operand cache without writing it back. it leaves
 * QACR0 and QACR1 holding bits 28 to 26 of dest. memcpy copies every byte of
 * any other dest and src */
void mwPlySh4CpyMem(void *dest, const void *src, Sint32 bytes);
void mwPlySfdCalcWorkStmBuf(Sint32 type, Sint32 bps, Sint32 *stream,
                            Sint32 *system, Sint32 *picture, Sint32 *sound,
                            Sint32 *adx_ring, Sint32 *adx_work);
Sint32 mwPlyCalcWorkCompo(MWS_PLY_CPRM_SFD *params);
Sint32 mwPlyCalcWorkSfd(MWS_PLY_CPRM_SFD *params);
Sint32 mwPlySfdFinish(struct mwsfd_player *player);
struct sfd_handle *mwPlySfdCreate(struct mwsfd_player *player, Sint32 type,
                                  Sint32 bps, Sint32 width, Sint32 height,
                                  Sint32 frames, Sint8 *work, Sint32 size);
void mwPlySfdDestroy(struct sfd_handle *handle);
void mwPlySfdSetCond(struct mwsfd_player *player, MWS_PLY_CPRM_SFD *params,
                     Sint32 vsyncs, Sint32 latency);
Sint32 mwPlySfdStandby(struct mwsfd_player *player);
Sint32 mwPlySfdStart(struct mwsfd_player *player);
Sint32 mwPlySfdStop(struct mwsfd_player *player);
void mwPlySfdGetWritePtr(struct mwsfd_player *player, char **data, Sint32 *size,
                         Sint32 *written);
void mwPlySfdAddWritePtr(struct mwsfd_player *player, Sint32 size,
                         Sint32 written);
Bool mwPlySfdIsEndDec(struct mwsfd_player *player);
SJ mwPlySfdCreateSj(void);
void mwPlySfdSjCopyToSfd(struct mwsfd_player *player);
#endif

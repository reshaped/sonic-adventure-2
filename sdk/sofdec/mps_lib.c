/* the MPEG program stream parser's library: its work, its handles and their
 * errors */
#include <mps.h>
#include <sg_xpt.h>
#include <sofdec.h>
#include <string.h>

enum handle_state { HANDLE_FREE = 1, HANDLE_USED };

struct mps_library *MPSLIB_libwork;
/* the handle MPSLIB_CheckHn checked last */
/* @hack type: mpslib_hn_last volatile, where a plain pointer would do */
static struct mps_handle *volatile mpslib_hn_last;

static Sint32 mpslib_InitLibWork(Sint32 count, void *work);
static void mpslib_InitErr(struct mps_error *error);
static void mpslib_SetErrSub(struct mps_error *error, Sint32 code);
static void mpslib_SetErrFnSub(struct mps_error *error, mps_error_fn report,
                               void *user);
static struct mps_handle *mpslib_SearchFreeHn(void);
static struct mps_handle *mpslib_InitHn(struct mps_handle *handle);
static void mpslib_ClearPackHd(struct mps_pack *header);
static void mpslib_ClearSysHd(struct mps_system *header);
static void mpslib_ClearPketHd(struct mps_packet *header);

Sint32 MPS_Init(Sint32 count, void *work)
{
    Sint32 result;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = mpslib_InitLibWork(count, work)) != 0) {
        return result;
    }
    MPSDEC_Init();
    MPSGET_Init();
    return 0;
}

static Sint32 mpslib_InitLibWork(Sint32 count, void *work)
{
    struct mps_handle *handles;
    Sint32 i;

    MPSLIB_libwork = work;
    UTY_MemsetDword((Uint32 *)MPSLIB_libwork, 0,
                    (sizeof(struct mps_library) +
                     (sizeof(struct mps_handle) * (count - 1))) /
                        sizeof(Uint32));
    strncpy(MPSLIB_libwork->version, "MPS Version 1.31 1999-11-26",
            sizeof(MPSLIB_libwork->version));
    strncpy(MPSLIB_libwork->copyright, "Copyright (c) 1997,1999 CRI",
            sizeof(MPSLIB_libwork->copyright));
    mpslib_InitErr(&MPSLIB_libwork->error);
    MPSLIB_libwork->count = count;
    handles = MPSLIB_libwork->handles;
    for (i = 0; i < count; i++) {
        handles[i].state = HANDLE_FREE;
    }
    return 0;
}

void MPS_Finish(void)
{
    struct mps_handle *handles;
    Sint32 count;
    Sint32 i;

    count = MPSLIB_libwork->count;
    handles = MPSLIB_libwork->handles;
    for (i = 0; i < count; i++) {
        if (handles[i].state == HANDLE_FREE) {
            continue;
        }
        MPS_Destroy(&handles[i]);
    }
    MPSDEC_Finish();
    MPSGET_Finish();
}

static void mpslib_InitErr(struct mps_error *error)
{
    MPSLIB_InitErrInf(error);
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void MPSLIB_InitErrInf(struct mps_error *error)
{
    error->report = NULL;
    error->user = NULL;
    error->code = 0;
}

Sint32 MPSLIB_SetErr(struct mps_handle *handle, Sint32 code)
{
    if (handle == NULL) {
        mpslib_SetErrSub(&MPSLIB_libwork->error, code);
    } else {
        mpslib_SetErrSub(&handle->error, code);
    }
    return code;
}

static void mpslib_SetErrSub(struct mps_error *error, Sint32 code)
{
    mps_error_fn report;

    error->code = code;
    if (code != 0) {
        report = error->report;
        if (report != NULL) {
            report(error->user, code);
        }
    }
}

Sint32 MPS_SetErrFn(struct mps_handle *handle, mps_error_fn report, void *user)
{
    if (handle == NULL) {
        mpslib_SetErrFnSub(&MPSLIB_libwork->error, report, user);
    } else {
        if (MPSLIB_CheckHn(handle) != 0) {
            return MPSLIB_SetErr(NULL, MPS_ERR_SET_CALLBACK);
        }
        mpslib_SetErrFnSub(&handle->error, report, user);
    }
    return 0;
}

static void mpslib_SetErrFnSub(struct mps_error *error, mps_error_fn report,
                               void *user)
{
    error->report = report;
    error->user = user;
}

Sint32 MPS_GetErrInf(struct mps_handle *handle, struct mps_error *out)
{
    if (handle == NULL) {
        *out = MPSLIB_libwork->error;
    } else {
        if (MPSLIB_CheckHn(handle) != 0) {
            return MPSLIB_SetErr(NULL, MPS_ERR_GET_ERROR);
        }
        *out = handle->error;
    }
    return 0;
}

Sint32 MPSLIB_CheckHn(struct mps_handle *handle)
{
    mpslib_hn_last = handle;
    if (handle == NULL || handle->state == HANDLE_FREE) {
        return -1;
    }
    return 0;
}

struct mps_handle *MPS_Create(void)
{
    struct mps_handle *handle;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((handle = mpslib_SearchFreeHn()) == NULL) {
        return NULL;
    }
    /* @hack spelling: the result through handle, where returning the call
     * would do */
    handle = mpslib_InitHn(handle);
    return handle;
}

static struct mps_handle *mpslib_SearchFreeHn(void)
{
    struct mps_handle *handles;
    struct mps_handle *handle;
    Sint32 count;
    Sint32 i;

    count = MPSLIB_libwork->count;
    handles = MPSLIB_libwork->handles;
    for (i = 0; i < count; i++) {
        handle = &handles[i];
        if (handle->state == HANDLE_FREE) {
            return handle;
        }
    }
    return NULL;
}

static struct mps_handle *mpslib_InitHn(struct mps_handle *handle)
{
    UTY_MemsetDword((Uint32 *)handle, 0, UTY_DWORDS(*handle));
    handle->state = HANDLE_USED;
    MPSLIB_InitErrInf(&handle->error);
    handle->length_bytes = MPS_LENGTH_BYTES;
    mpslib_ClearPackHd(&handle->pack);
    mpslib_ClearSysHd(&handle->last_system);
    mpslib_ClearSysHd(&handle->systems[MPS_AUDIO]);
    mpslib_ClearSysHd(&handle->systems[MPS_VIDEO]);
    mpslib_ClearSysHd(&handle->systems[MPS_OTHER]);
    mpslib_ClearPketHd(&handle->packet);
    return handle;
}

static void mpslib_ClearPackHd(struct mps_pack *header)
{
    header->mpeg1 = MPS_UNKNOWN;
    header->clock = MPS_UNKNOWN;
    header->mux_rate = MPS_UNKNOWN;
}

static void mpslib_ClearSysHd(struct mps_system *header)
{
    header->length = MPS_UNKNOWN;
    header->rate_bound = MPS_UNKNOWN;
    header->audio_bound = MPS_UNKNOWN;
    header->video_bound = MPS_UNKNOWN;
    header->fixed_rate = MPS_UNKNOWN;
    header->constrained = MPS_UNKNOWN;
    header->audio_lock = MPS_UNKNOWN;
    header->video_lock = MPS_UNKNOWN;
}

static void mpslib_ClearPketHd(struct mps_packet *header)
{
    header->stream_id = MPS_UNKNOWN;
    header->kind = MPS_UNKNOWN;
    header->channel = MPS_UNKNOWN;
    header->length = MPS_UNKNOWN;
    header->buffer_size = MPS_UNKNOWN;
    header->pts = MPS_UNKNOWN;
    header->dts = MPS_UNKNOWN;
    header->payload_length = MPS_UNKNOWN;
}

Sint32 MPS_Destroy(struct mps_handle *handle)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_DESTROY);
    }
    handle->state = HANDLE_FREE;
    return 0;
}

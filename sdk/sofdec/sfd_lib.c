/* the library's state: SFD_Init and SFD_Finish, and the critical sections the
 * members share. it keeps the error records of the library and each handle */
#include <machine.h>
#include <sfd.h>
#include <sg_xpt.h>
#include <sofdec.h>
#include <string.h>

#define INVALID_ERROR_CALLBACK ((Sint32)0xff000101)
#define INVALID_ERROR_QUERY ((Sint32)0xff000102)

/* the version and build time SFD_GetVersionStr returns. the member exports
 * it, though no header of the SDK declares it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
const char SFLIB_version_str[] =
    "\nCRI SFD  Ver.1.31 Build:Nov 26 1999 20:51:41\n";
struct sfd_library SFLIB_libwork;
/* SFD_GetVersionStr's string, which SFD_Init keeps here */
static const char *sflib_version;
/* SHC lays bss out in the order it first meets the names. this include
 * declares sfd_hn_last, so it follows sflib_version */
#include <sfd_player.h>
/* the handle SFLIB_CheckHn was last given */
struct sfd_handle *sfd_hn_last;

static Sint32 sflib_InitLibWork(struct sfd_init_params *params);
static void sflib_InitErr(struct sfd_error *error);
static void sflib_SetErrSub(struct sfd_error *error, Sint32 code);
static void sflib_SetErrFnSub(struct sfd_error *error,
                              void (*report)(void *, Sint32), void *user);
static void sflib_InitCs(void);
static void sflib_FinishCs(void);
static void sflib_InitSub(void);
static void sflib_FinishSub(void);

const char *SFD_GetVersionStr(void)
{
    return SFLIB_version_str;
}

Sint32 SFD_Init(struct sfd_init_params *params)
{
    Sint32 result;
    sflib_version = SFD_GetVersionStr();
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = sflib_InitLibWork(params)) != 0) {
        return result;
    }
    sflib_InitSub();
    sflib_InitCs();
    return 0;
}

static Sint32 sflib_InitLibWork(struct sfd_init_params *params)
{
    struct sfd_handle *handles;
    Sint32 count;
    Sint32 i;
    Sint32 result;

    UTY_MemsetDword((Uint32 *)&SFLIB_libwork, 0,
                    sizeof(SFLIB_libwork) / sizeof(Uint32));
    strncpy(SFLIB_libwork.version, "SFD Version 1.31 1999-11-26",
            sizeof(SFLIB_libwork.version));
    strncpy(SFLIB_libwork.copyright, "Copyright (c) 1997,1999 CRI",
            sizeof(SFLIB_libwork.copyright));
    UTY_MemcpyDword((Uint32 *)SFLIB_libwork.conditions,
                    (const Uint32 *)SFPLY_cond_dfl, SFD_CONDITIONS);
    SFLIB_libwork.params = *params;
    SFLIB_libwork.server_status = SFD_SERVER_IDLE;
    sflib_InitErr(&SFLIB_libwork.error);
    SFTIM_Init(&SFLIB_libwork.clock, params->time_unit);
    SFBUF_Init(&SFLIB_libwork.buffer_work);
    count = params->count;
    handles = params->handles;
    SFLIB_libwork.handle_count = count;
    SFLIB_libwork.handles = handles;
    UTY_MemsetDword((Uint32 *)handles, 0,
                    count * sizeof(*handles) / sizeof(Uint32));
    for (i = 0; i < count; i++) {
        handles[i].status = SFD_FREE;
    }
    result = SFTRN_Init(&SFLIB_libwork.stages, params->stages);
    if (result != 0) {
        return result;
    }
    return 0;
}

Sint32 SFD_Finish(void)
{
    Sint32 result;
    Sint32 last_error;
    Sint32 count;
    struct sfd_handle *handles;
    Sint32 i;

    last_error = 0;
    count = SFLIB_libwork.handle_count;
    handles = SFLIB_libwork.handles;
    for (i = 0; i < count; i++) {
        if (handles[i].status == SFD_FREE) {
            continue;
        }
        last_error = SFD_Destroy(&handles[i]);
    }
    SFTIM_Finish(&SFLIB_libwork.clock);
    SFBUF_Finish(&SFLIB_libwork.buffer_work);
    result = SFTRN_Finish(&SFLIB_libwork.stages);
    sflib_FinishSub();
    sflib_FinishCs();
    if (result != 0) {
        return result;
    }
    if (last_error != 0) {
        return last_error;
    }
    return 0;
}

static void sflib_InitErr(struct sfd_error *error)
{
    SFLIB_InitErrInf(error);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void SFLIB_InitErrInf(struct sfd_error *error)
{
    error->report = NULL;
    error->user = NULL;
    error->code = 0;
    error->detail = 0;
    error->extra = 0;
}

Sint32 SFLIB_SetErr(struct sfd_handle *handle, Sint32 code)
{
    if (handle == NULL) {
        sflib_SetErrSub(&SFLIB_libwork.error, code);
    } else {
        sflib_SetErrSub(&handle->error, code);
        if (handle->status > 0) {
            handle->status = -handle->status;
        }
    }
    return code;
}

static void sflib_SetErrSub(struct sfd_error *error, Sint32 code)
{
    void (*report)(void *, Sint32);

    error->code = code;
    if (code != 0) {
        report = error->report;
        if (report != NULL) {
            report(error->user, code);
        }
    }
}

Sint32 SFD_SetErrFn(struct sfd_handle *handle, void (*report)(void *, Sint32),
                    void *object)
{
    if (handle == NULL) {
        sflib_SetErrFnSub(&SFLIB_libwork.error, report, object);
    } else {
        if (SFLIB_CheckHn(handle) != 0) {
            return SFLIB_SetErr(NULL, INVALID_ERROR_CALLBACK);
        }
        sflib_SetErrFnSub(&handle->error, report, object);
    }
    return 0;
}

static void sflib_SetErrFnSub(struct sfd_error *error,
                              void (*report)(void *, Sint32), void *user)
{
    error->report = report;
    error->user = user;
}

Sint32 SFD_GetErrInf(struct sfd_handle *handle, struct sfd_error *out)
{
    if (handle == NULL) {
        *out = SFLIB_libwork.error;
    } else {
        if (SFLIB_CheckHn(handle) != 0) {
            return SFLIB_SetErr(NULL, INVALID_ERROR_QUERY);
        }
        *out = handle->error;
    }
    return 0;
}

Sint32 SFLIB_CheckHn(struct sfd_handle *handle)
{
    sfd_hn_last = handle;
    if (handle == NULL) {
        return -1;
    }
    if (handle->status == SFD_FREE) {
        return -1;
    }
    return 0;
}

static void sflib_InitCs(void)
{
}
static void sflib_FinishCs(void)
{
}

void SFLIB_LockCs(Sint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}
void SFLIB_UnlockCs(const Sint32 *mask)
{
    set_imask(*mask);
}

static void sflib_InitSub(void)
{
    SFPLY_Init();
    SFHDS_Init();
}

static void sflib_FinishSub(void)
{
    SFHDS_Finish();
}

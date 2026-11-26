/* a handle's status, information, conditions and stage handles. SFD_SetCond
 * and SFD_GetCond take a NULL handle for the library's conditions. each handle
 * SFD_Create makes starts with those */
#include <sfd.h>
#include <sg_xpt.h>

#define INVALID_STATUS ((Sint32)0xff000111)
#define INVALID_SET_CONDITION ((Sint32)0xff000112)
#define INVALID_GET_CONDITION ((Sint32)0xff000113)
#define INVALID_INFO ((Sint32)0xff000114)
#define INVALID_TRANSFER ((Sint32)0xff000117)

static void sfset_SetCondAll(Sint32 id, Sint32 value);
static Bool sfset_IsCondValid(struct sfd_handle *handle, Sint32 id,
                              Sint32 value);

Sint32 SFD_GetSvrStat(void)
{
    Sint32 status;
    /* @hack spelling: the call's result held in status, where the return
     * could take it directly */
    status = SFPLY_DecideSvrStat();
    return status;
}

Sint32 SFD_GetHnStat(struct sfd_handle *handle)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_STATUS);
    }
    return handle->status;
}

Sint32 SFD_SetCond(struct sfd_handle *handle, Sint32 id, Sint32 value)
{
    if (handle == NULL) {
        sfset_SetCondAll(id, value);
        SFLIB_libwork.conditions[id] = value;
    } else {
        if (SFLIB_CheckHn(handle) != 0) {
            return SFLIB_SetErr(NULL, INVALID_SET_CONDITION);
        }
        SFSET_SetCond(handle, id, value);
    }
    return 0;
}

static void sfset_SetCondAll(Sint32 id, Sint32 value)
{
    Sint32 count;
    struct sfd_handle *handle;
    Sint32 i;

    count = SFLIB_libwork.handle_count;
    handle = SFLIB_libwork.handles;
    /* @bug logic: sets the first handle's condition, count times while its
     * status is above 0, and no other handle's (0x8c54ae8e) */
    for (i = 0; i < count; i++) {
        if (handle->status > 0) {
            SFSET_SetCond(handle, id, value);
        }
    }
}

void SFSET_SetCond(struct sfd_handle *handle, Sint32 id, Sint32 value)
{
    if (sfset_IsCondValid(handle, id, value)) {
        handle->conditions[id] = value;
    }
}

static Bool sfset_IsCondValid(struct sfd_handle *handle, Sint32 id,
                              Sint32 value)
{
    if (id == SFD_COND_AUDIO && value == TRUE &&
        SFTRN_IsSetup(handle, SFD_AUDIO) == 0) {
        return FALSE;
    }
    if (id == SFD_COND_VIDEO && value == TRUE &&
        SFTRN_IsSetup(handle, SFD_VIDEO) == 0) {
        return FALSE;
    }
    return TRUE;
}

Sint32 SFD_GetCond(struct sfd_handle *handle, Sint32 id, Sint32 *value)
{
    if (handle == NULL) {
        *value = SFLIB_libwork.conditions[id];
    } else {
        if (SFLIB_CheckHn(handle) != 0) {
            return SFLIB_SetErr(NULL, INVALID_GET_CONDITION);
        }
        *value = SFSET_GetCond(handle, id);
    }
    return 0;
}

Sint32 SFSET_GetCond(struct sfd_handle *handle, Sint32 id)
{
    return handle->conditions[id];
}

Sint32 SFD_GetMvInf(struct sfd_handle *handle, struct sfd_movie_info *info)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_INFO);
    }
    *info = handle->movie;
    return 0;
}

Sint32 SFD_GetPlyInf(struct sfd_handle *handle, struct sfd_play_info *info)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_INFO);
    }
    *info = handle->play;
    return 0;
}

Sint32 SFD_GetTrHn(struct sfd_handle *handle, Sint32 stage, void **out)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_TRANSFER);
    }
    SFSET_GetTrHn(handle, stage, out);
    return 0;
}

void SFSET_GetTrHn(struct sfd_handle *handle, Sint32 stage, void **out)
{
    *out = handle->stages[stage].work.handle;
}

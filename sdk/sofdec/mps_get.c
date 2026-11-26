/* the parser's copies of the headers a handle read last (mps.h) */
#include <mps.h>
#include <sg_xpt.h>

void MPSGET_Init(void)
{
}

void MPSGET_Finish(void)
{
}

Sint32 MPS_GetPackHd(struct mps_handle *handle, struct mps_pack *out)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_GET_PACK);
    }
    *out = handle->pack;
    return 0;
}

Sint32 MPS_GetSysHd(struct mps_handle *handle, struct mps_system *out,
                    Sint32 index)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_GET_SYSTEM);
    }
    *out = handle->systems[index];
    return 0;
}

Sint32 MPS_GetLastSysHd(struct mps_handle *handle, struct mps_system *out)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_GET_SYSTEM);
    }
    *out = handle->last_system;
    return 0;
}

Sint32 MPS_GetPketHd(struct mps_handle *handle, struct mps_packet *out)
{
    if (MPSLIB_CheckHn(handle) != 0) {
        return MPSLIB_SetErr(NULL, MPS_ERR_GET_PACKET);
    }
    *out = handle->packet;
    return 0;
}

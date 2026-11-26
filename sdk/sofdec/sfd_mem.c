/* the input stage for a stream in memory. the caller writes the stream into
 * the stage's output ring through SFD_GetWritePtr and SFD_AddWritePtr. each
 * server call marks the ring prepared */
#include <sfd.h>
#include <sg_xpt.h>
#include "../sofdec/sfd_stage.h"

#define NO_READ ((Sint32)0xff000501)

/* clang-format off */
static SFD_STAGE_NOOP(SFMEM_Init)
static SFD_STAGE_NOOP(SFMEM_Finish)

static Sint32 SFMEM_ExecServer(struct sfd_handle *handle, Sint32 first,
                               Sint32 second, Sint32 third)
/* clang-format on */
{
    struct sfd_transfer *stage;
    Sint32 output;
    stage = &handle->stages[SFD_INPUT];
    output = stage->outputs[SFD_OUTPUT];
    SFBUF_SetPrepFlg(handle, output, TRUE);
    return 0;
}

/* clang-format off */
static SFD_STAGE_NOOP(SFMEM_Create)
static SFD_STAGE_NOOP(SFMEM_Destroy)
static SFD_STAGE_NOOP(SFMEM_Standby)
static SFD_STAGE_NOOP(SFMEM_Start)
static SFD_STAGE_NOOP(SFMEM_Stop)
static SFD_STAGE_NOOP(SFMEM_Pause)

static Sint32 SFMEM_GetWrite(struct sfd_handle *handle, Sint32 info,
                             Sint32 second, Sint32 third)
/* clang-format on */
{
    struct sfd_transfer *stage;
    Sint32 output;
    stage = &handle->stages[SFD_INPUT];
    output = stage->outputs[SFD_OUTPUT];
    return SFBUF_RingGetWrite(handle, output, (struct sfd_ring_info *)info);
}

static Sint32 SFMEM_AddWrite(struct sfd_handle *handle, Sint32 bytes,
                             Sint32 written, Sint32 third)
{
    struct sfd_transfer *stage;
    Sint32 output;
    stage = &handle->stages[SFD_INPUT];
    output = stage->outputs[SFD_OUTPUT];
    return SFBUF_RingAddWrite(handle, output, bytes, written);
}

SFD_STAGE_REJECT(SFMEM_GetRead, NO_READ)
SFD_STAGE_REJECT(SFMEM_AddRead, NO_READ)
/* clang-format off */
static SFD_STAGE_NOOP(SFMEM_Seek)

const struct sfd_stage_ops SFD_tr_in_mem = {
    {SFMEM_Init, SFMEM_Finish, SFMEM_ExecServer, SFMEM_Create, SFMEM_Destroy,
     SFMEM_Standby, SFMEM_Start, SFMEM_Stop, SFMEM_Pause, SFMEM_GetWrite,
     SFMEM_AddWrite, SFMEM_GetRead, SFMEM_AddRead, SFMEM_Seek}};
/* clang-format on */

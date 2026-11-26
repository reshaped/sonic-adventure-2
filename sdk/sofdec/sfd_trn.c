/* a handle's stages, which SFTRN_InitHn connects through the handle's
 * buffers. SFTRN_CallTrSetup runs an operation on every stage, and
 * SFTRN_CallTrtTrif on one. a stage the setup leaves out has no function
 * table, and its operations do nothing and return 0 */
#include <sfd.h>
#include <sg_xpt.h>

#define INVALID_STAGES ((Sint32)0xff000302)

static Sint32 sftrn_CallTrEntry(const struct sfd_stage_list *list,
                                Sint32 operation);
static void sftrn_InitTrData(struct sfd_transfer *stage,
                             const struct sfd_stage_ops *ops);
static Sint32 sftrn_BuildAll(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages);
static void sftrn_BuildSystem(struct sfd_handle *handle,
                              const struct sfd_stage_ops *const *stages);
static void sftrn_BuildVideo(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages);
static void sftrn_BuildAudio(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages);
static void sftrn_BuildUsr(struct sfd_handle *handle,
                           const struct sfd_stage_ops *const *stages);
static void sftrn_ConnTrnBuf0(struct sfd_handle *handle, Sint32 stage,
                              Sint32 buffer);
static void sftrn_ConnTrnBufV(struct sfd_handle *handle, Sint32 stage,
                              Sint32 buffer);
static void sftrn_ConnTrnBufA(struct sfd_handle *handle, Sint32 stage,
                              Sint32 buffer);
static void sftrn_ConnTrnBufU(struct sfd_handle *handle, Sint32 stage,
                              Sint32 buffer);
static void sftrn_ConnTrnBuf(struct sfd_handle *handle, Sint32 stage,
                             Sint32 output, Sint32 buffer);
static void sftrn_ConnBufTrn(struct sfd_handle *handle, Sint32 buffer,
                             Sint32 stage);

Sint32 SFTRN_Init(struct sfd_stage_list *list,
                  const struct sfd_stage_list *source)
{
    Sint32 result;
    *list = *source;
    /* @hack spelling: the call's result held in result, where the return
     * could take it directly */
    result = sftrn_CallTrEntry(source, SFD_OP_INIT);
    return result;
}

Sint32 SFTRN_Finish(struct sfd_stage_list *list)
{
    Sint32 result;
    /* @hack spelling: the call's result held in result, where the return
     * could take it directly */
    result = sftrn_CallTrEntry(list, SFD_OP_FINISH);
    return result;
}

static Sint32 sftrn_CallTrEntry(const struct sfd_stage_list *list,
                                Sint32 operation)
{
    Sint32 result = 0;
    Sint32 i;
    const struct sfd_stage_ops *const *entries;
    const sfd_stage_fn *methods;

    entries = list->entries;
    for (i = 0; i < SFD_TRANSFER_LIST; i++) {
        if (entries[i] == NULL) {
            break;
        }
        methods = entries[i]->apply;
        result = methods[operation](NULL, 0, 0, 0);
        if (result != 0) {
            break;
        }
    }
    return result;
}

Sint32 SFTRN_InitHn(struct sfd_handle *handle, struct sfd_transfer *stages,
                    struct sfd_create_params *params, void *work)
{
    Sint32 i;
    struct sfd_transfer *first;
    const struct sfd_stage_ops *const *entries;
    const struct sfd_setup *setup;
    /* @hack spelling: first, a copy of stages, where stages would do */
    first = stages;
    setup = params->setup;
    entries = setup->entries;
    for (i = 0; i < SFD_STAGES; i++) {
        sftrn_InitTrData(&first[i], entries[i]);
    }
    if (sftrn_BuildAll(handle, entries) != 0) {
        return SFLIB_SetErr(handle, INVALID_STAGES);
    }
    return 0;
}

static void sftrn_InitTrData(struct sfd_transfer *stage,
                             const struct sfd_stage_ops *ops)
{
    Sint32 i;
    stage->ended = FALSE;
    stage->prepared = FALSE;
    stage->ops = ops;
    stage->input = SFD_NO_BUFFER;
    for (i = 0; i < SFD_SYSTEM_OUTPUTS; i++) {
        stage->outputs[i] = SFD_NO_BUFFER;
    }
}

static Sint32 sftrn_BuildAll(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages)
{
    if (stages[SFD_SYSTEM] != NULL) {
        sftrn_ConnTrnBuf0(handle, SFD_INPUT, SFD_SYSTEM_BYTES);
        sftrn_BuildSystem(handle, stages);
    } else if (stages[SFD_VIDEO] != NULL) {
        sftrn_ConnTrnBuf0(handle, SFD_INPUT, SFD_VIDEO_BYTES);
        sftrn_BuildVideo(handle, stages);
        SFSET_SetCond(handle, SFD_COND_AUDIO, FALSE);
    } else if (stages[SFD_AUDIO] != NULL) {
        sftrn_ConnTrnBuf0(handle, SFD_INPUT, SFD_AUDIO_BYTES);
        sftrn_BuildAudio(handle, stages);
        SFSET_SetCond(handle, SFD_COND_VIDEO, FALSE);
    } else if (stages[SFD_USER] != NULL) {
        sftrn_ConnTrnBuf0(handle, SFD_INPUT, SFD_USER_DATA);
        sftrn_BuildUsr(handle, stages);
        SFSET_SetCond(handle, SFD_COND_AUDIO, FALSE);
        SFSET_SetCond(handle, SFD_COND_VIDEO, FALSE);
    } else {
        return -1;
    }
    return 0;
}

static void sftrn_BuildSystem(struct sfd_handle *handle,
                              const struct sfd_stage_ops *const *stages)
{
    sftrn_ConnBufTrn(handle, SFD_SYSTEM_BYTES, SFD_SYSTEM);
    if (stages[SFD_VIDEO] != NULL) {
        sftrn_ConnTrnBufV(handle, SFD_SYSTEM, SFD_VIDEO_BYTES);
        sftrn_BuildVideo(handle, stages);
    }
    if (stages[SFD_AUDIO] != NULL) {
        sftrn_ConnTrnBufA(handle, SFD_SYSTEM, SFD_AUDIO_BYTES);
        sftrn_BuildAudio(handle, stages);
    }
    if (stages[SFD_USER] != NULL) {
        sftrn_ConnTrnBufU(handle, SFD_SYSTEM, SFD_USER_DATA);
        sftrn_BuildUsr(handle, stages);
    }
}

static void sftrn_BuildVideo(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages)
{
    sftrn_ConnBufTrn(handle, SFD_VIDEO_BYTES, SFD_VIDEO);
    sftrn_ConnTrnBuf0(handle, SFD_VIDEO, SFD_VIDEO_FRAMES);
    if (stages[SFD_VIDEO_POST] != NULL) {
        sftrn_ConnBufTrn(handle, SFD_VIDEO_FRAMES, SFD_VIDEO_POST);
        sftrn_ConnTrnBuf0(handle, SFD_VIDEO_POST, SFD_POST_FRAMES);
        sftrn_ConnBufTrn(handle, SFD_POST_FRAMES, SFD_VIDEO_OUT);
    } else {
        sftrn_ConnBufTrn(handle, SFD_VIDEO_FRAMES, SFD_VIDEO_OUT);
    }
}

static void sftrn_BuildAudio(struct sfd_handle *handle,
                             const struct sfd_stage_ops *const *stages)
{
    sftrn_ConnBufTrn(handle, SFD_AUDIO_BYTES, SFD_AUDIO);
    sftrn_ConnTrnBuf0(handle, SFD_AUDIO, SFD_AUDIO_SAMPLES);
    if (stages[SFD_AUDIO_POST] != NULL) {
        sftrn_ConnBufTrn(handle, SFD_AUDIO_SAMPLES, SFD_AUDIO_POST);
        sftrn_ConnTrnBuf0(handle, SFD_AUDIO_POST, SFD_POST_SAMPLES);
        sftrn_ConnBufTrn(handle, SFD_POST_SAMPLES, SFD_AUDIO_OUT);
    } else {
        sftrn_ConnBufTrn(handle, SFD_AUDIO_SAMPLES, SFD_AUDIO_OUT);
    }
}

static void sftrn_BuildUsr(struct sfd_handle *handle,
                           const struct sfd_stage_ops *const *stages)
{
    sftrn_ConnBufTrn(handle, SFD_USER_DATA, SFD_USER);
}

/* define name, which has stage write buffer through output slot */
#define CONNECT_OUTPUT(name, slot)                                             \
    static void name(struct sfd_handle *handle, Sint32 stage, Sint32 buffer)   \
    {                                                                          \
        sftrn_ConnTrnBuf(handle, stage, slot, buffer);                         \
        /* @hack redundant: a return at the end of a void function */          \
        return;                                                                \
    }

CONNECT_OUTPUT(sftrn_ConnTrnBuf0, SFD_OUTPUT)
CONNECT_OUTPUT(sftrn_ConnTrnBufV, SFD_SYSTEM_VIDEO)
CONNECT_OUTPUT(sftrn_ConnTrnBufA, SFD_SYSTEM_AUDIO)
CONNECT_OUTPUT(sftrn_ConnTrnBufU, SFD_SYSTEM_PRIVATE)

static void sftrn_ConnTrnBuf(struct sfd_handle *handle, Sint32 stage,
                             Sint32 output, Sint32 buffer)
{
    struct sfd_transfer *stages;
    struct sfd_transfer *writer;
    struct sfd_buffer *target;
    /* @hack spelling: the writer and its buffer reached through stepped
     * pointers, where indexing the stages and buffers would do */
    stages = &handle->stages[0];
    writer = &stages[stage];
    writer->outputs[output] = buffer;
    target = handle->buffers.records;
    target += buffer;
    target->writer = stage;
}

static void sftrn_ConnBufTrn(struct sfd_handle *handle, Sint32 buffer,
                             Sint32 stage)
{
    struct sfd_buffer *source;
    struct sfd_transfer *reader;
    /* @hack spelling: the buffer and its reader reached through stepped
     * pointers, where indexing the buffers and stages would do */
    source = handle->buffers.records;
    source += buffer;
    source->reader = stage;
    reader = handle->stages;
    reader += stage;
    reader->input = buffer;
}

Sint32 SFTRN_CallTrSetup(struct sfd_handle *handle, Sint32 operation)
{
    Sint32 result = 0;
    Sint32 i;
    struct sfd_transfer *first;
    struct sfd_transfer *stage;
    const struct sfd_stage_ops *ops;
    const sfd_stage_fn *methods;

    /* @hack spelling: first, a copy of the stages' address, where stage
     * could take it directly */
    first = &handle->stages[0];
    stage = first;
    for (i = 0; i < SFD_STAGES; i++, stage++) {
        ops = stage->ops;
        if (ops == NULL) {
            continue;
        }
        methods = ops->apply;
        result = methods[operation](handle, 0, 0, 0);
        if (result != 0) {
            break;
        }
    }
    return result;
}

Sint32 SFTRN_CallTrtTrif(struct sfd_handle *handle, Sint32 stage,
                         Sint32 operation, Sint32 first, Sint32 second)
{
    const struct sfd_stage_ops *ops;
    const sfd_stage_fn *methods;
    Sint32 result;

    struct sfd_transfer(*stages)[SFD_STAGES];
    stages = &handle->stages;
    /* @hack type: the stage's address formed in bytes from a pointer to the
     * stages, where indexing handle->stages would do */
    ops = ((struct sfd_transfer *)((char *)&(*stages)[0] +
                                   (stage * sizeof(struct sfd_transfer))))
              ->ops;
    if (ops == NULL) {
        return 0;
    }
    methods = ops->apply;
    /* @hack spelling: the call's result held in result, where the return
     * could take it directly */
    result = methods[operation](handle, first, second, 0);
    return result;
}
/* @hack spelling: transfer stepped on to stage, where indexing the stages by
 * stage would do */
#define STEP_TO_STAGE(transfer, handle, stage)                                 \
    do {                                                                       \
        (transfer) = (handle)->stages;                                         \
        (transfer) += (stage);                                                 \
    } while (0)
void SFTRN_SetPrepFlg(struct sfd_handle *handle, Sint32 stage, Bool prepared)
{
    struct sfd_transfer *transfer;
    STEP_TO_STAGE(transfer, handle, stage);
    transfer->prepared = prepared;
}
Bool SFTRN_GetPrepFlg(struct sfd_handle *handle, Sint32 stage)
{
    struct sfd_transfer *transfer;
    STEP_TO_STAGE(transfer, handle, stage);
    return transfer->prepared;
}
void SFTRN_SetTermFlg(struct sfd_handle *handle, Sint32 stage, Bool ended)
{
    struct sfd_transfer *transfer;
    STEP_TO_STAGE(transfer, handle, stage);
    transfer->ended = ended;
}
Bool SFTRN_GetTermFlg(struct sfd_handle *handle, Sint32 stage)
{
    struct sfd_transfer *transfer;
    STEP_TO_STAGE(transfer, handle, stage);
    return transfer->ended;
}
Bool SFTRN_IsSetup(struct sfd_handle *handle, Sint32 stage)
{
    struct sfd_transfer *transfer;
    STEP_TO_STAGE(transfer, handle, stage);
    /* @hack spelling: a test of ops that returns FALSE or TRUE, where
     * returning whether ops is set would do */
    if (transfer->ops == NULL) {
        return FALSE;
    }
    return TRUE;
}

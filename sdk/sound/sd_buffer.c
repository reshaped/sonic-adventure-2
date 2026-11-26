#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the size of each of the stream's mute buffers */
#define MUTE_BYTES 0x1000

union sd_module_refs sdgModuleRef;
struct sd_pstm_port_ext sdgPstmPortExtRef[SD_PSTM_PORT_NUM];
Uint8 sdgGlobalWrk[sizeof(struct sd_work) + SD_BUFFER_ALIGN];
struct sd_memblk sdgMemBlk[SDD_MEMBLK_HANDLE_NUM];
struct sd_memblk_queue_entry sdgMemBlkQueue[SDD_MEMBLK_QUEUE_NUM];
Uint8 sdgHostCmdPrimBuf[(SD_BATCH_MAX * sizeof(struct sd_driver_cmd)) +
                        SD_BUFFER_ALIGN];
struct sd_host_cmd_entry sdgHostCmd2ndBuf[SD_HOST_CMD_QUEUE_NUM];
/* the member exports these two buffers, though no header of the SDK
 * declares them. by their names, each holds silence for a stream, one in
 * linear PCM and one in ADPCM */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint8 sdgPstmLinearMute[MUTE_BYTES];
Uint8 sdgPstmAdpcmMute[MUTE_BYTES];
/* NOLINTEND(misc-use-internal-linkage) */

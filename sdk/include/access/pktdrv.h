/* the LAN adapter's packet driver, which the data link sends and receives
 * frames through */
#ifndef ACCESS_PKTDRV_H
#define ACCESS_PKTDRV_H
#include <access/at_types.h>
#include <sg_xpt.h>
Sint32 pd_ftp_init(Sint32 interface_number);
Sint32 pd_ftp_open(void);
Sint32 pd_ftp_close(void);
Sint32 pd_ftp_terminate(void);
Sint32 pd_ftp_getaddr(Uint8 *address);
Sint32 pd_ftp_send(Uint8 *frame, Sint32 length);
Sint32 PKTDRV_Init(void);
Sint32 PKTDRV_CanExit(void);
Sint32 PKTDRV_Exit(void);
Sint32 PKTDRV_GetIfnum(void);
Sint32 PKTDRV_IfInit(Sint32 interface_number);
Sint32 PKTDRV_IfUp(Sint32 interface_number);
Sint32 PKTDRV_IfDown(Sint32 interface_number);
/* buffs's buffers must hold ETHERNET_FRAME_BYTES or fewer in all */
Sint32 PKTDRV_Send(AT_MaxBuffs *buffs, Sint32 interface_number);
Sint32 PKTDRV_Recv(Sint32 length, Sint32 interface_number);
#endif

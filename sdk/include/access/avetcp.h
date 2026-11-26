/* ACCESS's TCP calls, which make their requests through AT_apiCall. a send or a
 * receive passes no callback, so it waits until it is done */
#ifndef ACCESS_AVETCP_H
#define ACCESS_AVETCP_H
#include <access/at_api.h>
#include <sg_xpt.h>
int AvetcpInitialize(void);
Sint16 AvetcpTerminate(void);
Sint16 AvetcpOpen(Uint32 address, Uint16 remote_port, Uint16 local_port);
Sint16 AvetcpClose(Sint16 handle);
Sint16 AvetcpAbort(Sint16 handle);
Sint16 AvetcpStat(Sint16 handle, AT_stat_r *status);
Sint16 AvetcpSend(Sint16 handle, Uint8 *buffer, Sint16 count);
Sint16 AvetcpReceive(Sint16 handle, Uint8 *buffer, Sint16 count);
#endif

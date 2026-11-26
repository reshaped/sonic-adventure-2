/* the PPP library's debug messages, which go out of the SCIF at 57600 bps */
#ifndef NETWORK_NET_DEBUG_H
#define NETWORK_NET_DEBUG_H
/* dbDebug's bits, by category. DumpPacket dumps only where dbDebug holds
 * both */
#define DB_DEBUG_PPP 4
#define DB_DEBUG_DUMP 8
extern int dbDebug;
void dbInit(void);
#define DB_MESSAGE_BYTES 8192
/* write format's message where dbDebug holds category, each \n as \r\n,
 * with interrupts masked. in the SHC build the message must fit in
 * DB_MESSAGE_BYTES with its NUL */
void dbPrint(int category, const char *format, ...);
#endif

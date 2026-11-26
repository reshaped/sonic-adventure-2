/* reads and writes of the broadband adapter's registers across the G2 bus,
 * for its NexGen driver */
#ifndef NETWORK_G2_IO_H
#define NETWORK_G2_IO_H
#include <sg_xpt.h>

Uint8 ETD_G2Read8(Uint32 address);
Uint16 ETD_G2Read16(Uint32 address);
Uint32 ETD_G2Read32(Uint32 address);
void ETD_G2Write8(Uint32 address, Uint8 value);
void ETD_G2Write16(Uint32 address, Uint16 value);
void ETD_G2Write32(Uint32 address, Uint32 value);
#endif

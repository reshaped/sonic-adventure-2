#include <sg_xpt.h>
#include "ppp_packet.h"

Uint8 *PP_put32(Uint8 *destination, Uint32 value)
{
    *destination++ = (Uint8)(value >> 24);
    *destination++ = (Uint8)(value >> 16);
    *destination++ = (Uint8)(value >> 8);
    *destination++ = (Uint8)value;
    return destination;
}
Uint8 *PP_put16(Uint8 *destination, Uint16 value)
{
    *destination++ = (Uint8)(value >> 8);
    *destination++ = (Uint8)value;
    return destination;
}
Uint16 PP_get16(const Uint8 *source)
{
    Uint16 value = *source++;
    value <<= 8;
    value |= *source;
    return value;
}
Uint32 PP_get32(const Uint8 *source)
{
    Uint32 value = *source++;
    value <<= 8;
    value |= *source++;
    value <<= 8;
    value |= *source++;
    value <<= 8;
    value |= *source;
    return value;
}

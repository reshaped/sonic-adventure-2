#include <access/in.h>

/* the socket library's identification, which names its byte order */
char ByteOrderStr[] =
    "$CMS#ACS%CPR02012 IN SOCKET BYTE ODER IS 386BSD(8086) TYPE";

/* htonl's and ntohl's body. it stores value's bytes in result most
 * significant first, through unsigned char, which may alias result */
#define NETWORK_LONG(value)                                                    \
    unsigned int result;                                                       \
    unsigned char *bytes = (unsigned char *)&result;                           \
    bytes[0] = (unsigned char)(((value) & 0xff000000UL) >> 24);                \
    bytes[1] = (unsigned char)(((value) & 0x00ff0000UL) >> 16);                \
    bytes[2] = (unsigned char)(((value) & 0x0000ff00UL) >> 8);                 \
    bytes[3] = (unsigned char)(value);                                         \
    return result
/* @hack spelling: word, assigned inside the first byte's expression, where
 * value itself would do */
#define NETWORK_SHORT(value)                                                   \
    unsigned short result;                                                     \
    unsigned char *bytes = (unsigned char *)&result;                           \
    int word;                                                                  \
    bytes[0] = (unsigned char)(((word = (value)) & 0xff00) >> 8);              \
    bytes[1] = (unsigned char)word;                                            \
    return result
unsigned long htonl(unsigned long host)
{
    NETWORK_LONG(host);
}
unsigned long ntohl(unsigned long network)
{
    NETWORK_LONG(network);
}
unsigned short htons(unsigned short host)
{
    NETWORK_SHORT(host);
}
unsigned short ntohs(unsigned short network)
{
    NETWORK_SHORT(network);
}

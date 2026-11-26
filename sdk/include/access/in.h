/* conversions of 16- and 32-bit values between host and network byte order, and
 * the socket library's identifying string */
#ifndef ACCESS_IN_H
#define ACCESS_IN_H
extern char ByteOrderStr[];
unsigned long htonl(unsigned long host);
unsigned long ntohl(unsigned long network);
unsigned short htons(unsigned short host);
unsigned short ntohs(unsigned short network);
#endif

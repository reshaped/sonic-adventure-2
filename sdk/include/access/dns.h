/* ACCESS's resolver. ADNS_GetTicket opens a lookup of host and returns its
 * ticket, or -1. ADNS_LookUp returns eWouldBlock while the answer is due. on
 * eOK it writes the host's four address bytes. on a failure it releases the
 * ticket itself. otherwise ADNS_ReleaseTicket releases it */
#ifndef ACCESS_DNS_H
#define ACCESS_DNS_H
#include <sg_xpt.h>
enum {
    eOK,
    eGeneric = -1,
    eWouldBlock = -2,
    eMemory = -3,
    eIO = -4,
    eNotFound = -5,
    eTimedOut = -6
};
#define kWave_MaxHostNameLen 1024

void ADNS_Initialize(const Uint8 *suffix, Uint32 primary, Uint32 secondary);
void ADNS_Finalize(void);
Sint32 ADNS_GetTicket(const Uint8 *host);
void ADNS_ReleaseTicket(Sint32 ticket);
Sint32 ADNS_LookUp(Sint32 ticket, Uint8 *address);
void ADNS_GetTicketInfo(Sint32 ticket, Uint8 **host);
Uint32 DNS_GetAddress(const char *host);
void dns_initialize_servers(const Uint8 *suffix, Uint32 first, Uint32 second,
                            Uint32 third, Uint32 fourth);
/* copy ticket index's host name, its NUL included, into hostname, which must
 * hold kWave_MaxHostNameLen bytes. returns eOK, or eNotFound for an unused
 * ticket or an empty name */
int dns_copy_ticket_name(int index, char *hostname);
int dns_copy_ticket_address(int index, Uint8 *address);
#endif

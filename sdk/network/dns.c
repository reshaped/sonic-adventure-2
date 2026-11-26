#include <access/at_api.h>
#include <access/dns.h>
#include <access/vsyncpro.h>
#include <sg_xpt.h>
#include <string.h>
#include "tcp_api.h"

#define DNS_TICKETS 8
#define DNS_SERVERS 4
#define DNS_PACKET_BYTES 2048
#define DNS_PORT_NETWORK_ORDER 0x3500
#define DNS_HEADER_BYTES 12
/* a section's count in the header takes DNS_COUNT_BYTES. the question count
 * and the answer count lie at these offsets */
#define DNS_COUNT_BYTES 2
#define DNS_QUESTIONS_OFFSET 4
#define DNS_ANSWERS_OFFSET 6
/* a question ends in its type and class. an answer adds a TTL and its
 * data's length to them, after its name. the length lies DNS_LENGTH_OFFSET
 * bytes in */
#define DNS_TYPE_CLASS_BYTES 4
#define DNS_TTL_BYTES 4
#define DNS_RECORD_BYTES 10
#define DNS_LENGTH_OFFSET 8
#define DNS_ADDRESS_BYTES 4
/* the longest name and the longest label a query carries */
#define DNS_NAME_CHARACTERS 253
#define DNS_LABEL_BYTES 63
/* the top bit of a 16-bit handle, which each of API_UDP_Open's negative errors
 * sets */
#define HANDLE_ERROR 0x8000
/* a header's flags. DNS_RESPONSE marks a reply, and DNS_OPCODE holds the
 * query's kind. DNS_RECURSION_DESIRED asks the server to recurse, and DNS_RCODE
 * holds a reply's error */
#define DNS_RESPONSE 0x8000
#define DNS_OPCODE 0x7800
#define DNS_RECURSION_DESIRED 0x0100
#define DNS_RCODE 0x000f
/* the type and class of an Internet address record */
#define DNS_TYPE_A 1
#define DNS_CLASS_IN 1
/* a length byte with both these bits set starts a pointer of
 * DNS_POINTER_BYTES, which ends a compressed name */
#define DNS_POINTER 0xc0
#define DNS_POINTER_BYTES 2

/* PUT16 stores value at cursor as two bytes, most significant first. GET16
 * reads two such bytes into value. each steps cursor past them */
#define PUT16(cursor, value)                                                   \
    (*(cursor)++ = ((value) & 0xff00) >> 8, *(cursor)++ = (value) & 0xff)
#define GET16(value, cursor)                                                   \
    ((value) = *(cursor)++ << 8, (value) |= *(cursor)++)

enum { DNS_IDLE, DNS_WAITING, DNS_RECEIVED };
/* a lookup. a reply must come from remote_address and remote_port, each where
 * it isn't 0. received counts the reply's bytes in packet. deadline is the
 * dns_time the query's wait ends at, or 0 before the first query. retry is an
 * index of retry_delay, and server an index of servers */
struct dns_ticket {
    Uint16 active, handle;
    Sint32 state;
    Uint32 remote_address;
    Uint16 remote_port;
    Uint8 packet[DNS_PACKET_BYTES];
    Sint32 received, deadline;
    char hostname[kWave_MaxHostNameLen];
    Sint32 query_id, retry, server;
    Bool appended_suffix;
    AT_NINT32 address;
};
struct dns_servers {
    Uint32 address[DNS_SERVERS];
};

#pragma section ATA
static char build_version[44];
/* the library's version and build time */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
char *ntgDnsBuild = build_version;
/* the empty list dns_initialize_servers starts from */
static struct dns_servers no_servers = {{0}};
/* defined here, past no_servers, whose bytes the image holds first */
static char build_version[44] = "\nntDns Ver 1.10 Build:Jan 26 2001 16:55:00\n";
#pragma section

/* the ms a query waits for its reply, indexed by retry: DNS_WAITS waits, then
 * the same again. with one server, retry steps through the first DNS_WAITS.
 * with any other count, retry steps 0, 3, 1, 4, 2, 5 while the query
 * alternates between servers[0] and servers[1], so each wait is tried at
 * both */
#define DNS_WAITS 3
static int retry_delay[2 * DNS_WAITS] = {5000, 15000, 30000,
                                         5000, 15000, 30000};
static char default_domain[kWave_MaxHostNameLen];
static struct dns_ticket tickets[DNS_TICKETS];
static Uint8 outgoing[DNS_PACKET_BYTES];
static Uint32 servers[DNS_SERVERS];
static Sint32 server_failures, server_count, preferred_server;

static int dns_time(void)
{
    return (int)((unsigned int)vsGetTickCount() * VS_TICK_MILLISECONDS);
}
static struct dns_ticket *find_ticket(Uint16 handle)
{
    int i;
    for (i = 0; i < DNS_TICKETS; i++) {
        if (tickets[i].active && tickets[i].handle == handle) {
            break;
        }
    }
    if (i == DNS_TICKETS) {
        return NULL;
    }
    return &tickets[i];
}
static int reserve_ticket(void)
{
    int i;
    for (i = 0; i < DNS_TICKETS; i++) {
        if (!tickets[i].active) {
            break;
        }
    }
    if (i == DNS_TICKETS) {
        return -1;
    }
    tickets[i].active = 1;
    return i;
}
static void release_ticket(int index)
{
    tickets[index].active = 0;
}
#ifdef NONMATCHING
/* this side takes status as the Sint16 AT_udp_receiver passes, the plain form
 * the hack tag below gives, so receive_packet is a receiver as it stands */
static Sint16 receive_packet(Sint16 status, AT_UDPPRM *packet)
#else
/* @hack type: status as an int, where AT_udp_receiver's Sint16 would do.
 * udp_rcv_call passes AT_UDP_GET_BUFFER or AT_UDP_COPIED, which a Sint16
 * holds alike */
static Sint16 receive_packet(int status, AT_UDPPRM *packet)
#endif
{
    struct dns_ticket *ticket = find_ticket(packet->handle);
    if (!ticket) {
        return 1;
    }
#ifdef NONMATCHING
    /* this side refuses a negative length */
    if (packet->len < 0) {
        return 1;
    }
#endif
    if (status == AT_UDP_GET_BUFFER) {
        if (ticket->state != DNS_WAITING ||
            (ticket->remote_address &&
             ticket->remote_address != packet->srcaddr.u32) ||
            (ticket->remote_port &&
             ticket->remote_port != packet->srcport.u16)) {
            return 1;
        } else {
            packet->usrbuff = ticket->packet;
            if ((unsigned int)packet->len > sizeof(ticket->packet)) {
                packet->len = sizeof(ticket->packet);
                return 0;
            }
        }
    } else {
        ticket->state = DNS_RECEIVED;
        ticket->received = packet->len;
    }
    return 0;
}
static int open_ticket(Uint32 address, Uint16 remote_port, Uint16 local_port)
{
    int index = reserve_ticket();
    Sint32 handle;
    AT_uopenparam request;
    struct dns_ticket *ticket;
    if (index == -1) {
        return -1;
    }
    memset(&request, 0, sizeof(request));
    request.youraddr.u32 = address;
    request.yourport.u16 = remote_port;
    request.myport.u16 = local_port;
#ifdef NONMATCHING
    /* this side stores receive_packet as it is, an AT_udp_receiver on this
     * side. a call through another function type is undefined in C */
    request.receiver = receive_packet;
#else
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    request.receiver = (AT_udp_receiver)receive_packet;
#endif
    handle = API_UDP_Open(&request);
    /* @hack spelling: the test of HANDLE_ERROR, where handle < 0 would do */
    if (handle & HANDLE_ERROR) {
        release_ticket(index);
        return -1;
    }
    ticket = &tickets[index];
    ticket->handle = handle;
    ticket->state = DNS_IDLE;
    ticket->remote_address = address;
    ticket->remote_port = remote_port;
    ticket->deadline = 0;
    ticket->query_id = 0;
    ticket->retry = 0;
    ticket->server = preferred_server;
    return index;
}
static void close_ticket(int index)
{
    AT_nhparam request;
    /* API_TCP_Close takes a UDP handle too, and closes it by its class */
    request.nh = tickets[index].handle;
    API_TCP_Close(&request);
    release_ticket(index);
}
static int send_packet(int index, Uint32 address, Uint16 port, Uint8 *data,
                       Sint16 length)
{
    struct dns_ticket *ticket = &tickets[index];
    AT_usendparam request;
    Sint32 result;
    request.nh = ticket->handle;
    request.youraddr.u32 = address;
    request.yourport.u16 = port;
    request.bufnum = 1;
    request.buffers[0].len = length;
    request.buffers[0].buf = data;
    result = API_UDP_Send(&request);
    return result == 0;
}
/* write the label at *input to *output as its length and its bytes, and step
 * both past it. returns 1 where a dot ends the label, and 0 at the string's
 * end. a dot with no label before it is written as the label "." */
static int write_label(Uint8 **output, Uint8 **input)
{
    Uint8 *to = *output + 1;
    Uint8 *from = *input;
    int length = 0;
    int character;
    int more;
    for (;;) {
        character = *from;
        if (!character) {
            more = 0;
            break;
        }
        if (character == '.') {
            from++;
            more = 1;
            if (length) {
                break;
            }
            *to++ = '.';
            length = 1;
            break;
        }
        from++;
        *to++ = character;
        length++;
    }
    if (length) {
        **output = length;
        *input = from;
        *output = to;
    }
    return more;
}
static Uint8 *skip_name(Uint8 *name)
{
    while (*name) {
        if ((*name & DNS_POINTER) == DNS_POINTER) {
            return name + DNS_POINTER_BYTES;
        }
        name += *name + 1;
    }
    return name + 1;
}
/* send ticket index's query to server, and give it timeout ms for its reply.
 * a host with no dot goes with default_domain after it. returns 1 where the
 * query went */
static int send_query(int index, Uint32 server, int timeout)
{
    struct dns_ticket *ticket = &tickets[index];
    Uint8 *to = outgoing;
    Uint8 *from;
    int suffix;
    memset(outgoing, 0, sizeof(outgoing));
    /* one question, and the memset's 0 in the other three counts */
    PUT16(to, ticket->query_id);
    PUT16(to, DNS_RECURSION_DESIRED);
    PUT16(to, 1);
    to += 3 * DNS_COUNT_BYTES;
    suffix = 1;
    from = (Uint8 *)ticket->hostname;
    while (write_label(&to, &from)) {
        suffix = 0;
    }
    if (suffix) {
        from = (Uint8 *)default_domain;
        suffix = 0;
        while (write_label(&to, &from)) {
            suffix = 1;
        }
    }
    ticket->appended_suffix = suffix;
    *to++ = 0;
    PUT16(to, DNS_TYPE_A);
    PUT16(to, DNS_CLASS_IN);
    memset(ticket->packet, 0, sizeof(ticket->packet));
    ticket->state = DNS_WAITING;
    ticket->deadline = (Sint32)((Uint32)dns_time() + timeout);
    return send_packet(index, server, DNS_PORT_NETWORK_ORDER, outgoing,
                       to - outgoing);
}
#ifdef NONMATCHING
/* whether a query can carry host: 1 to DNS_NAME_CHARACTERS characters, whose
 * labels each hold 1 to DNS_LABEL_BYTES. a dot may end the name */
static int valid_host(const Uint8 *host)
{
    unsigned int total = 0;
    unsigned int label = 0;
    if (!host || !*host) {
        return 0;
    }
    while (*host) {
        if (++total > DNS_NAME_CHARACTERS) {
            return 0;
        }
        if (*host++ == '.') {
            if (!label) {
                return 0;
            }
            label = 0;
        } else if (++label > DNS_LABEL_BYTES) {
            return 0;
        }
    }
    return 1;
}
#endif
void ADNS_Initialize(const Uint8 *suffix, Uint32 primary, Uint32 secondary)
{
    int i;
    for (i = 0; i < DNS_TICKETS; i++) {
        tickets[i].active = 0;
    }
    default_domain[0] = 0;
#ifdef NONMATCHING
    /* this side copies only a suffix a query can carry, which default_domain
     * holds */
    if (valid_host(suffix)) {
#else
    if (suffix) {
#endif
        strcpy(default_domain, (const char *)suffix);
    }
    server_count = 0;
    if (primary) {
        servers[0] = primary;
        server_count++;
        if (secondary) {
            servers[1] = secondary;
            server_count++;
        }
    } else if (secondary) {
        servers[0] = secondary;
        server_count++;
    }
    preferred_server = 0;
}
void dns_initialize_servers(const Uint8 *suffix, Uint32 first, Uint32 second,
                            Uint32 third, Uint32 fourth)
{
    struct dns_servers candidates;
    Uint32 *list = candidates.address;
    int count;
    candidates = no_servers;
    count = 0;
    if (first) {
        list[count++] = first;
    }
    if (second) {
        list[count++] = second;
    }
    if (third) {
        list[count++] = third;
    }
    if (fourth) {
        list[count++] = fourth;
    }
    ADNS_Initialize(suffix, list[0], list[1]);
    if (count > 3) {
        count--;
        servers[count] = list[count];
        server_count++;
    }
    if (count > 2) {
        count--;
        servers[count] = list[count];
        server_count++;
    }
    server_failures = 0;
}
/* after a lookup times out at both servers, swap in the spares the list
 * holds. servers[2] trades places with servers[server], where the lookup
 * started, and servers[3] with servers[1 - server] */
static void rotate_servers(int server)
{
    Uint32 saved;
    if (server_failures > 0) {
        if (server_count < 3) {
            server_failures = 0;
        }
        if (server_count > 2) {
            server_failures = 0;
            saved = servers[2];
            servers[2] = servers[server];
            servers[server] = saved;
        }
        if (server_count > 3) {
            saved = servers[3];
            servers[3] = servers[1 - server];
            servers[1 - server] = saved;
        }
    }
}
void ADNS_Finalize(void)
{
}
Sint32 ADNS_GetTicket(const Uint8 *host)
{
    int index;
    struct dns_ticket *ticket;
#ifdef NONMATCHING
    /* this side refuses a host a query can't carry, which ticket->hostname may
     * not hold, and a lookup with no server */
    if (!valid_host(host) || !server_count) {
        return eGeneric;
    }
#endif
    index = open_ticket(0, DNS_PORT_NETWORK_ORDER, 0);
    if (index == -1) {
        return -1;
    }
    ticket = &tickets[index];
    strcpy(ticket->hostname, (const char *)host);
    memset(ticket->address.b, 0, sizeof(ticket->address.b));
    return index;
}
void ADNS_ReleaseTicket(Sint32 ticket)
{
#ifdef NONMATCHING
    /* this side releases only an active ticket of the table, where the matching
     * side takes any but -1 */
    if ((Uint32)ticket < DNS_TICKETS && tickets[ticket].active) {
#else
    if (ticket != -1) {
#endif
        close_ticket(ticket);
    }
}

#ifdef NONMATCHING
/* the byte past the name at name, or NULL where the name runs past end. a
 * pointer ends the name, as in skip_name, which follows none */
static const Uint8 *checked_name(const Uint8 *name, const Uint8 *end)
{
    while (name < end) {
        Uint8 length = *name++;
        if (!length) {
            return name;
        }
        if ((length & DNS_POINTER) == DNS_POINTER) {
            return name < end ? name + 1 : NULL;
        }
        if (length > DNS_LABEL_BYTES || end - name < length) {
            return NULL;
        }
        name += length;
    }
    return NULL;
}
/* whether ADNS_LookUp can parse the reply without reading past it */
static int valid_reply(const struct dns_ticket *ticket)
{
    const Uint8 *at = ticket->packet;
    const Uint8 *end;
    unsigned int answers;
    unsigned int length;
    if (ticket->received < DNS_HEADER_BYTES ||
        ticket->received > DNS_PACKET_BYTES) {
        return 0;
    }
    end = at + ticket->received;
    if (at[DNS_QUESTIONS_OFFSET] || at[DNS_QUESTIONS_OFFSET + 1] != 1) {
        return 0;
    }
    answers = ((unsigned int)at[DNS_ANSWERS_OFFSET] << 8) |
              at[DNS_ANSWERS_OFFSET + 1];
    at = checked_name(at + DNS_HEADER_BYTES, end);
    if (!at || end - at < DNS_TYPE_CLASS_BYTES) {
        return 0;
    }
    at += DNS_TYPE_CLASS_BYTES;
    while (answers--) {
        at = checked_name(at, end);
        if (!at || end - at < DNS_RECORD_BYTES) {
            return 0;
        }
        length = ((unsigned int)at[DNS_LENGTH_OFFSET] << 8) |
                 at[DNS_LENGTH_OFFSET + 1];
        if (end - at - DNS_RECORD_BYTES < (int)length ||
            (at[0] == 0 && at[1] == DNS_TYPE_A && at[2] == 0 &&
             at[3] == DNS_CLASS_IN && length != DNS_ADDRESS_BYTES)) {
            return 0;
        }
        at += DNS_RECORD_BYTES + length;
    }
    return 1;
}
#endif
Sint32 ADNS_LookUp(Sint32 ticket, Uint8 *address)
{
    struct dns_ticket *work;
    Uint8 *read;
    Uint8 *write;
    unsigned int id;
    unsigned int flags;
    unsigned int type;
    unsigned int class_id;
    unsigned int length;
    int answers;
    int i;
    long host_length;
#ifdef NONMATCHING
    /* this side refuses a ticket outside the table or not active, and a missing
     * address. the matching side refuses only a negative ticket */
    if ((Uint32)ticket >= DNS_TICKETS || !tickets[ticket].active || !address) {
#else
    if (ticket < 0) {
#endif
        return eGeneric;
    }
    work = &tickets[ticket];
    if (work->state != DNS_RECEIVED) {
#ifdef NONMATCHING
        /* this side waits on a ticket with no deadline, and compares times by
         * their difference, which holds across dns_time's wrap */
        if (!work->deadline ||
            (Sint32)((Uint32)dns_time() - (Uint32)work->deadline) > 0) {
#else
        if (work->deadline < dns_time()) {
#endif
            if (work->retry > 0) {
                if (server_count == 1) {
                    if (work->retry > DNS_WAITS - 1) {
                        ADNS_ReleaseTicket(ticket);
                        return eTimedOut;
                    }
                } else {
                    work->server = 1 - work->server;
                    if (work->retry > (2 * DNS_WAITS) - 1) {
                        ADNS_ReleaseTicket(ticket);
                        server_failures++;
                        rotate_servers(work->server);
                        return eTimedOut;
                    }
                }
            }
            if (!send_query(ticket, servers[work->server],
                            retry_delay[work->retry])) {
                ADNS_ReleaseTicket(ticket);
                return eGeneric;
            }
            if (server_count == 1) {
                work->retry++;
            } else if (work->retry < DNS_WAITS) {
                work->retry += DNS_WAITS;
            } else if (work->retry < (2 * DNS_WAITS) - 1) {
                work->retry -= DNS_WAITS - 1;
            } else {
                work->retry++;
            }
        } else {
            return eWouldBlock;
        }
        /* @hack redundant: the empty test of state */
        if (work->state == DNS_RECEIVED) {
        }
        return eWouldBlock;
    }
#ifdef NONMATCHING
    /* this side goes on waiting where valid_reply finds that the parse below
     * would read past the reply */
    if (!valid_reply(work)) {
        work->state = DNS_WAITING;
        return eWouldBlock;
    }
#endif
    read = work->packet;
    GET16(id, read);
    GET16(flags, read);
    if (id != (unsigned int)work->query_id ||
        (flags & (DNS_RESPONSE | DNS_OPCODE)) != DNS_RESPONSE) {
        memset(work->packet, 0, sizeof(work->packet));
        work->state = DNS_WAITING;
        return eWouldBlock;
    }
    if (flags & DNS_RCODE) {
        ADNS_ReleaseTicket(ticket);
        return eGeneric;
    }
    read += DNS_COUNT_BYTES;
    GET16(answers, read);
    if (!answers) {
        if (work->appended_suffix) {
            host_length = strlen(work->hostname);
            work->hostname[host_length++] = '.';
            work->hostname[host_length] = 0;
            work->deadline = 0;
            work->retry = 0;
            work->state = DNS_IDLE;
            if (send_query(ticket, servers[work->server],
                           retry_delay[work->retry])) {
                return eWouldBlock;
            }
        }
        ADNS_ReleaseTicket(ticket);
        return eGeneric;
    }
    /* the authority and additional counts, then the question */
    read += 2 * DNS_COUNT_BYTES;
    read = skip_name(read);
    read += DNS_TYPE_CLASS_BYTES;
    for (i = 0; i < answers; i++) {
        read = skip_name(read);
        GET16(type, read);
        GET16(class_id, read);
        read += DNS_TTL_BYTES;
        GET16(length, read);
        if (type == DNS_TYPE_A && class_id == DNS_CLASS_IN) {
            for (write = work->address.b;
                 write < work->address.b + sizeof(work->address.b); write++) {
                *write = *read;
                *address++ = *read++;
            }
            preferred_server = work->server;
            server_failures = 0;
            return eOK;
        }
        read += length;
    }
    ADNS_ReleaseTicket(ticket);
    return eGeneric;
}
void ADNS_GetTicketInfo(Sint32 ticket, Uint8 **host)
{
    struct dns_ticket *work;
#ifdef NONMATCHING
    /* this side refuses a missing host, and gives NULL for a ticket outside the
     * table or not active */
    if (!host) {
        return;
    }
    if ((Uint32)ticket >= DNS_TICKETS || !tickets[ticket].active) {
        *host = NULL;
        return;
    }
#endif
    work = &tickets[ticket];
    *host = (Uint8 *)work->hostname;
}
int dns_copy_ticket_name(int index, char *hostname)
{
    int result = eNotFound;
    unsigned long length;
    int i;
    struct dns_ticket *ticket;
    char *from;
#ifdef NONMATCHING
    /* this side refuses an index outside the table, where the matching side
     * takes any but -1 */
    if ((unsigned int)index < DNS_TICKETS && hostname) {
#else
    if (index != -1 && hostname) {
#endif
        ticket = &tickets[index];
        if (ticket->active == 1) {
            if ((int)(length = strlen(ticket->hostname)) > 0) {
                i = 0;
                from = ticket->hostname;
                for (; i <= (int)length; i++) {
                    *hostname++ = *from++;
                }
                result = eOK;
            }
        }
    }
    return result;
}
int dns_copy_ticket_address(int index, Uint8 *address)
{
    int result = eNotFound;
    struct dns_ticket *ticket;
    Uint8 *from;
#ifdef NONMATCHING
    /* this side refuses an index outside the table, where the matching side
     * takes any but -1 */
    if ((unsigned int)index < DNS_TICKETS && address) {
#else
    if (index != -1 && address) {
#endif
        ticket = &tickets[index];
        if (ticket->active == 1 && ticket->address.u32) {
            from = ticket->address.b;
            do {
                *address++ = *from++;
            } while (from < ticket->address.b + sizeof(ticket->address.b));
            result = eOK;
        }
    }
    return result;
}
Uint32 DNS_GetAddress(const char *host)
{
    Uint32 address;
    Uint8 bytes[sizeof(address)];
    int index = ADNS_GetTicket((const Uint8 *)host);
    int result;
    if (index == -1) {
        return (Uint32)-1;
    }
    while ((result = ADNS_LookUp(index, bytes)) == eWouldBlock) {
#ifdef NONMATCHING
        /* this side waits a frame between polls, while the vertical blank's
         * network handler takes the reply */
        vsWaitVSync(1);
#endif
    }
    if (result == eOK) {
        memcpy(&address, bytes, sizeof(address));
    } else {
        address = 0;
    }
    ADNS_ReleaseTicket(index);
    return address;
}

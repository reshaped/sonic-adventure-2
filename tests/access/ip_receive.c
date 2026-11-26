/* Exercise IP_Recv's frees and the reassembly list's links on the
 * non-matching side. */
#include <assert.h>
#include <string.h>
#include "ip.c"

#define LISTS 3
#define SOURCE 0x0a000001
#define PAYLOAD_OFFSET (ETHERNET_HEADER_BYTES + IP_HEADER_BYTES)
#define FRAGMENT_BYTES 8
/* a protocol IP_Recv has no case for: GRE's */
#define OTHER_PROTOCOL 47

Uint8 max_icb_entry;
struct net_icb *icb_table;
volatile Sint32 icmp_timer_1sec;

static AT_MaxBuffs lists[LISTS];
static int lists_taken[LISTS];
/* the lists OS_AllocBuffs lends before it fails, and its failures */
static int lists_left = LISTS;
static int lists_refused;
/* the pool buffers MP_Alloc has lent and OS_FreeMempool hasn't taken back */
static int pool_taken;
/* the calls of MP_Alloc that fail before one lends */
static int pool_failures;
static Uint8 frame[64];
static struct net_iface interface;

AT_MaxBuffs *OS_AllocBuffs(void)
{
    int index;
    if (!lists_left) {
        lists_refused++;
        return NULL;
    }
    for (index = 0; index < LISTS; index++) {
        if (!lists_taken[index]) {
            lists_left--;
            lists_taken[index] = 1;
            return &lists[index];
        }
    }
    return NULL;
}

void OS_FreeBuffs(AT_MaxBuffs *buffers)
{
    assert(lists_taken[buffers - lists]);
    lists_taken[buffers - lists] = 0;
    lists_left++;
}

static int lists_in_use(void)
{
    int count = 0;
    int index;
    for (index = 0; index < LISTS; index++) {
        count += lists_taken[index];
    }
    return count;
}

Sint32 MP_Alloc(AT_Vptr *pointer, Sint32 size, Sint32 kind)
{
    (void)size;
    (void)kind;
    if (pool_failures) {
        pool_failures--;
        return -1;
    }
    pointer->type = AT_PTR_TYPE_POOL;
    pointer->buff = 0;
    pool_taken++;
    return 0;
}

Sint32 OS_FreeMempool(AT_Vptr *pointer)
{
    assert(pointer->type == AT_PTR_TYPE_POOL);
    pool_taken--;
    return 0;
}

void OS_Vmemcpy(AT_Vptr *destination, AT_Vptr *source, Uint16 size)
{
    (void)destination;
    (void)source;
    (void)size;
}

void OS_SetVptrLocal(AT_Vptr *pointer, void *address)
{
    (void)address;
    pointer->type = AT_PTR_TYPE_LOCAL;
    pointer->buff = 0;
}

Uint16 OS_Htons(Uint16 value)
{
    return (Uint16)((value << 8) | (value >> 8));
}

Uint32 OS_Htonl(Uint32 value)
{
    return ((value & 0xff) << 24) | ((value & 0xff00) << 8) |
           ((value >> 8) & 0xff00) | (value >> 24);
}

Sint32 DL_MatchMyIpaddr(Uint32 ip)
{
    (void)ip;
    return 0;
}

struct net_iface *DL_GetIfEntry(Sint32 index)
{
    (void)index;
    return &interface;
}

Sint32 UDP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_size, Uint32 src,
                Uint32 dst)
{
    (void)packet;
    (void)ip_header_size;
    (void)src;
    (void)dst;
    return UDP_NO_SOCKET;
}

Sint32 TCP_Recv(AT_MaxBuffs *packet, Uint16 ip_header_size, Uint32 src,
                Uint32 dst)
{
    (void)packet;
    (void)ip_header_size;
    (void)src;
    (void)dst;
    return 0;
}

Sint32 ICMP_Recv(AT_MaxBuffs *buffers, Uint16 offset, Uint32 source)
{
    (void)buffers;
    (void)offset;
    (void)source;
    return 0;
}

Sint32 ICMP_Send(Uint8 type, Uint8 code, Uint32 destination,
                 AT_MaxBuffs *buffers)
{
    (void)type;
    (void)code;
    (void)destination;
    (void)buffers;
    return 0;
}

Sint32 API_IpRecv(AT_MaxBuffs *packet, Uint32 source, AT_udp_receiver receiver,
                  Sint32 socket)
{
    (void)packet;
    (void)source;
    (void)receiver;
    (void)socket;
    assert(0); /* No raw socket is open. */
    return 0;
}

static int reassemblies_in_use(void)
{
    int count = 0;
    int index;
    for (index = 0; index < REASSEMBLIES; index++) {
        count += fragpktarray[index].used;
    }
    return count;
}

static int fragments_in_use(void)
{
    int count = 0;
    int index;
    for (index = 0; index < FRAGMENTS; index++) {
        count += fragmentarray[index].used;
    }
    return count;
}

/* take the fragment of datagram ident at offset, in FRAGMENT_UNIT_BYTES */
static AT_MaxBuffs *take(Uint16 ident, Uint8 ttl, Uint16 offset, Uint8 more)
{
    return receive_fragment(frame, PAYLOAD_OFFSET, FRAGMENT_BYTES, 0, ttl,
                            ident, SOURCE, offset, more);
}

static struct ip_reassembly *find(Uint16 ident)
{
    struct ip_reassembly *reassembly;
    for (reassembly = reassembly_head; reassembly;
         reassembly = reassembly->next) {
        if (reassembly->ident == ident) {
            return reassembly;
        }
    }
    return NULL;
}

/* a fragment again at the last fragment's offset is freed, not lost, whether
 * the last is the head or behind it. one again at the head's offset is freed
 * once */
static void check_duplicate(void)
{
    assert(!take(1, 30, 0, 1));
    assert(!take(1, 30, 0, 1));
    assert(fragments_in_use() == 1 && pool_taken == 1);
    assert(!take(1, 30, 2, 1));
    assert(!take(1, 30, 2, 1));
    assert(!take(1, 30, 0, 1));
    assert(fragments_in_use() == 2 && pool_taken == 2);
}

/* a datagram completed behind the head is unlinked and released, and the
 * list keeps the others */
static void check_complete_behind_head(void)
{
    AT_MaxBuffs *buffers = take(2, 30, 0, 0);
    assert(buffers && buffers->bufnum == 1);
    free_reassembled_buffers(buffers);
    OS_FreeBuffs(buffers);
    assert(reassembly_head == find(1) && !reassembly_head->next);
    assert(reassembly_tail == reassembly_head);
    assert(reassemblies_in_use() == 1);
}

/* a reassembly behind the head that expires is unlinked alone */
static void check_expiry_behind_head(void)
{
    struct ip_reassembly *kept;
    assert(!take(3, 30, 0, 1));
    assert(!take(4, 0, 0, 1));
    kept = find(3);
    IP_Timer();
    assert(reassembly_head == find(1) && reassembly_head->next == kept);
    assert(!kept->next && reassembly_tail == kept);
    assert(reassemblies_in_use() == 2 && fragments_in_use() == 3);
}

/* a pool that fails once drops the other reassemblies, and the one kept ends
 * the list */
static void check_discard(void)
{
    struct ip_reassembly *head = reassembly_head;
    pool_failures = 1;
    assert(!take(1, 30, 1, 1));
    assert(reassembly_head == head && !head->next);
    assert(reassembly_tail == head);
    assert(reassemblies_in_use() == 1);
    assert(fragments_in_use() == 3 && pool_taken == 3);
}

/* write into frame, after its link header, a datagram of FRAGMENT_BYTES of
 * protocol's data. fragment is the header's flags and offset. the result is
 * the frame's size */
static Sint32 write_datagram(Uint8 protocol, Uint16 ident, Uint16 fragment)
{
    struct ip_header header;
    Uint16 sum;
    memset(&header, 0, sizeof(header));
    header.version_ihl =
        (IP_VERSION << IP_VERSION_SHIFT) | (IP_HEADER_BYTES / IP_WORD_BYTES);
    header.length = OS_Htons(IP_HEADER_BYTES + FRAGMENT_BYTES);
    header.ident = OS_Htons(ident);
    header.fragment = OS_Htons(fragment);
    header.ttl = 64;
    header.protocol = protocol;
    sum = IP_Chksum(&header, IP_HEADER_BYTES / CHECKSUM_WORD_BYTES);
    ((Uint8 *)&header.checksum)[0] = (Uint8)(sum >> 8);
    ((Uint8 *)&header.checksum)[1] = (Uint8)sum;
    memset(frame, 0, sizeof(frame));
    memcpy(frame + ETHERNET_HEADER_BYTES, &header, sizeof(header));
    return PAYLOAD_OFFSET + FRAGMENT_BYTES;
}

/* hand IP_Recv a frame of the datagram write_datagram writes */
static Sint32 receive(Uint8 protocol, Uint16 ident, Uint16 fragment)
{
    return IP_Recv(frame, write_datagram(protocol, ident, fragment),
                   ETHERNET_HEADER_BYTES, 0);
}

/* a datagram whose ICMP reply finds no list frees its own, and a reassembled
 * one its pool buffers too */
static void check_no_reply_list(void)
{
    int pool_before = pool_taken;
    lists_left = 1;
    assert(receive(IPPROTO_UDP, 0, 0) == -1);
    assert(lists_refused == 1 && !lists_in_use());
    lists_left = 1;
    assert(receive(OTHER_PROTOCOL, 0, 0) == -1);
    assert(lists_refused == 2 && !lists_in_use());
    lists_left = 1;
    assert(receive(IPPROTO_UDP, 5, IP_MF) == 0);
    assert(pool_taken == pool_before + 1);
    assert(receive(IPPROTO_UDP, 5, 1) == -1);
    assert(lists_refused == 3 && !lists_in_use());
    assert(pool_taken == pool_before);
    assert(receive(OTHER_PROTOCOL, 6, IP_MF) == 0);
    assert(receive(OTHER_PROTOCOL, 6, 1) == -1);
    assert(lists_refused == 4 && !lists_in_use());
    assert(pool_taken == pool_before && reassemblies_in_use() == 1);
    lists_left = LISTS;
}

int main(void)
{
    check_duplicate();
    check_complete_behind_head();
    check_expiry_behind_head();
    check_discard();
    check_no_reply_list();
    return 0;
}

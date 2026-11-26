/* Exercise ICMP_Recv's mask reply on the non-matching side: a reply too short
 * to hold a mask is dropped, and a whole one is recorded. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "icmp.c"

#define MASK_REPLY_ID 0x1234
#define MASK_REPLY_SEQ 1
#define SUBNET_MASK 0xffffff00
/* the reply's offset in frame, past its link and IP headers */
#define REPLY_OFFSET (ETHERNET_HEADER_BYTES + IP_HEADER_BYTES)

/* the buffers OS_SetVptrLocal has named. a vptr's buff holds a buffer's index
 * here times LOCAL_SPAN, plus the offset into that buffer */
#define LOCALS 4
#define LOCAL_SPAN 0x10000
static Uint8 *locals[LOCALS];
static Uint8 frame[64];

void OS_SetVptrLocal(AT_Vptr *pointer, void *address)
{
    int index;
    for (index = 0; index < LOCALS && locals[index] != address; index++) {
        if (!locals[index]) {
            locals[index] = address;
            break;
        }
    }
    assert(index < LOCALS);
    pointer->type = AT_PTR_TYPE_LOCAL;
    pointer->buff = index * LOCAL_SPAN;
}

static Uint8 *local_address(AT_Vptr *pointer)
{
    assert(pointer->type == AT_PTR_TYPE_LOCAL);
    return locals[pointer->buff / LOCAL_SPAN] + pointer->buff % LOCAL_SPAN;
}

void OS_IncVptr(AT_Vptr *pointer, Sint32 size)
{
    pointer->buff += size;
}

void OS_Vmemcpy(AT_Vptr *destination, AT_Vptr *source, Uint16 size)
{
    memcpy(local_address(destination), local_address(source), size);
}

/* every message's checksum passes */
Uint16 UDP_Chksum(struct tcp_pseudo *pseudo, AT_MaxBuffs *buffers,
                  Sint32 reverse)
{
    (void)pseudo;
    (void)buffers;
    (void)reverse;
    return 0;
}

AT_MaxBuffs *OS_AllocBuffs(void)
{
    abort(); /* No echo request comes. */
}

void OS_FreeBuffs(AT_MaxBuffs *buffers)
{
    (void)buffers;
    abort(); /* No echo request comes. */
}

Sint32 IP_Send(AT_MaxBuffs *buffers, Uint32 destination, Uint8 raw, Uint8 ttl,
               Uint8 service, Uint8 dont_fragment, Uint16 options_size,
               void *options, Uint8 protocol, Sint32 flags)
{
    (void)buffers;
    (void)destination;
    (void)raw;
    (void)ttl;
    (void)service;
    (void)dont_fragment;
    (void)options_size;
    (void)options;
    (void)protocol;
    (void)flags;
    abort(); /* No echo request comes. */
}

Sint32 UDP_ComeIcmpUnreach(Uint16 port, Uint32 address, Uint8 code)
{
    (void)port;
    (void)address;
    (void)code;
    abort(); /* No unreachable error comes. */
}

int IP_ComeIcmpRedirect(Uint32 destination, Uint32 gateway, Uint16 flags,
                        Uint32 source)
{
    (void)destination;
    (void)gateway;
    (void)flags;
    (void)source;
    abort(); /* No redirect comes. */
}

struct net_iface *DL_GetIfEntry(Sint32 index)
{
    (void)index;
    abort(); /* No redirect comes. */
}

Uint32 OS_Htonl(Uint32 value)
{
    (void)value;
    abort(); /* No redirect comes. */
}

/* hand ICMP_Recv a frame holding the first length bytes of a mask reply */
static Sint32 receive(Sint32 length)
{
    struct icmp_header reply;
    AT_MaxBuffs buffers;
    memset(&reply, 0, sizeof(reply));
    reply.type = ICMP_MASKREPLY;
    reply.un.echo.id = MASK_REPLY_ID;
    reply.un.echo.seq = MASK_REPLY_SEQ;
    reply.mask = SUBNET_MASK;
    memset(frame, 0, sizeof(frame));
    memcpy(frame + REPLY_OFFSET, &reply, length);
    buffers.bufnum = 1;
    buffers.total_len = buffers.buffers[0].len = REPLY_OFFSET + length;
    OS_SetVptrLocal(&buffers.buffers[0].vptr, frame);
    return ICMP_Recv(&buffers, REPLY_OFFSET, 0);
}

int main(void)
{
    assert(receive(ICMP_HEADER_BYTES) == -1);
    assert(mask_reply.id == 0);
    assert(receive(ICMP_MASK_BYTES) == 0);
    assert(mask_reply.id == MASK_REPLY_ID && mask_reply.seq == MASK_REPLY_SEQ);
    assert(mask_reply.mask == SUBNET_MASK);
    return 0;
}

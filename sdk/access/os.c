/* the system's side of ACCESS. OS_AllocBuffs lends one of BUFFER_SLOTS buffer
 * lists, and OS_Vmemcpy and OS_ChksumBuffs read the bytes an AT_Vptr names.
 * the TCP/IP daemon, its critical section and its clock are the vsync
 * library's */
#include <access/at_types.h>
#include <access/tcp_private.h>
#include <access/vsyncpro.h>
#include <sg_xpt.h>
#include <string.h>

#define BUFFER_SLOTS 5
/* @hack type: the type read as a Sint8, where the Uint8 field would do */
#define VPTR_TYPE(pointer) ((Sint8)(pointer)->type)

struct buffer_slot {
    AT_MaxBuffs buffers;
    Uint8 in_use;
};

volatile Sint32 TcpTaskCount = 0;
volatile Sint32 TcpTaskTerminateReq = 0;

static struct buffer_slot alloc_max_buffs[BUFFER_SLOTS];
static Sint32 tcpipSavedLevel;

Sint32 OS_Init(void)
{
    Sint32 index;
    for (index = 0; index < BUFFER_SLOTS; index++) {
        alloc_max_buffs[index].in_use = 0;
    }
    return 0;
}

AT_MaxBuffs *OS_AllocBuffs(void)
{
    Sint32 index;
    for (index = 0; index < BUFFER_SLOTS; index++) {
        if (!alloc_max_buffs[index].in_use) {
            alloc_max_buffs[index].in_use = 1;
            return &alloc_max_buffs[index].buffers;
        }
    }
    return NULL;
}

void OS_FreeBuffs(AT_MaxBuffs *buffs)
{
    struct buffer_slot *slot;
    Sint32 index;
    for (index = 0; index < BUFFER_SLOTS; index++) {
        slot = &alloc_max_buffs[index];
        if (&slot->buffers == buffs) {
            alloc_max_buffs[index].in_use = 0;
            return;
        }
    }
}

Uint32 OS_Htonl(Uint32 value)
{
    return ((value & 0xff) << 24) | ((value & 0xff00) << 8) |
           ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
}

Uint16 OS_Htons(Uint16 value)
{
    return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
}

#ifdef NONMATCHING
/* the address pointer names, or NULL for AT_PTR_TYPE_NONE */
static Uint8 *buffer_address(const AT_Vptr *pointer)
{
    Uint32 value = pointer->buff;
    if (pointer->type == AT_PTR_TYPE_POOL) {
        const struct memory_pool *pool =
            &mcb_table[(value & MP_PAGE_MASK) >> MP_PAGE_SHIFT];
        const struct pool_fragment *fragment =
            &pool->fragments[(value & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT];
        return pool->base + fragment->offset + (value & MP_OFFSET_MASK);
    }
    if (pointer->type == AT_PTR_TYPE_LOCAL ||
        pointer->type == AT_PTR_TYPE_EXTERNAL) {
        return (Uint8 *)value;
    }
    return NULL;
}
#else
/* @hack type: mcb_table under a second name, which symbols.txt places at
 * mcb_table, as an array of one pointer, where mcb_table would do */
extern struct memory_pool *os_pool_table[1];
#endif

void OS_Vmemcpy(AT_Vptr *destination, AT_Vptr *source, Uint16 size)
{
#ifdef NONMATCHING
    /* this side finds each address by buffer_address, which reads mcb_table by
     * its own name. os_pool_table is a name only symbols.txt places. it copies
     * nothing for a pointer of AT_PTR_TYPE_NONE, as the matching side's
     * switches do */
    void *target = buffer_address(destination);
    void *origin = buffer_address(source);
    if (target && origin && size != 0) {
        memcpy(target, origin, size);
    }
#else
    Uint32 source_address;
    Uint32 destination_address;

    switch (VPTR_TYPE(source)) {
    case AT_PTR_TYPE_LOCAL:
        /* @hack redundant: the assignment of AT_PTR_TYPE_POOL to
         * destination's type, which never runs */
        if (0) {
            destination->type = AT_PTR_TYPE_POOL;
        }
        switch (VPTR_TYPE(destination)) {
        case AT_PTR_TYPE_LOCAL:
            memcpy((void *)destination->buff, (void *)source->buff, size);
            break;
        case AT_PTR_TYPE_POOL: {
            Uint32 encoded;
            Sint32 pool_index;
            Uint32 fragment_index;

            encoded = destination->buff;
            pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
            fragment_index = (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
            destination_address = (Uint32)(*os_pool_table)[pool_index].base;
            destination_address +=
                (*os_pool_table)[pool_index].fragments[fragment_index].offset +
                (encoded & MP_OFFSET_MASK);
        }
            memcpy((void *)destination_address, (void *)source->buff, size);
            break;
        case AT_PTR_TYPE_EXTERNAL:
            memcpy((void *)destination->buff, (void *)source->buff, size);
            break;
        }
        break;
    case AT_PTR_TYPE_POOL:
        /* @hack redundant: the assignment of AT_PTR_TYPE_EXTERNAL to
         * destination's type, which never runs */
        if (0) {
            destination->type = AT_PTR_TYPE_EXTERNAL;
        }
        switch (VPTR_TYPE(destination)) {
        case AT_PTR_TYPE_LOCAL: {
            Uint32 encoded;
            Sint32 pool_index;
            Uint32 fragment_index;

            encoded = source->buff;
            pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
            fragment_index = (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
            /* @hack redundant: the sum assigned to fragment_index, which
             * never runs */
            if (0) {
                fragment_index = fragment_index + encoded;
            }
            source_address = (Uint32)(*os_pool_table)[pool_index].base;
            source_address +=
                (*os_pool_table)[pool_index].fragments[fragment_index].offset +
                (encoded & MP_OFFSET_MASK);
        }
            memcpy((void *)destination->buff, (void *)source_address, size);
            break;
        case AT_PTR_TYPE_POOL: {
            Uint32 encoded;
            Sint32 pool_index;
            Uint32 fragment_index;

            encoded = source->buff;
            /* @hack redundant: the mask assigned to encoded, which never
             * runs */
            if (0) {
                encoded = encoded & MP_PAGE_MASK;
            }
            pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
            fragment_index = (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
            source_address = (Uint32)(*os_pool_table)[pool_index].base;
            source_address +=
                (*os_pool_table)[pool_index].fragments[fragment_index].offset +
                (encoded & MP_OFFSET_MASK);
        }
            {
                Uint32 encoded;
                Sint32 pool_index;
                Uint32 fragment_index;

                encoded = destination->buff;
                pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
                fragment_index =
                    (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
                /* @hack redundant: the product assigned to fragment_index,
                 * which never runs */
                if (0) {
                    fragment_index = encoded * sizeof(struct memory_pool);
                }
                destination_address = (Uint32)(*os_pool_table)[pool_index].base;
                destination_address += (*os_pool_table)[pool_index]
                                           .fragments[fragment_index]
                                           .offset +
                                       (encoded & MP_OFFSET_MASK);
            }
            memcpy((void *)destination_address, (void *)source_address, size);
            break;
        case AT_PTR_TYPE_EXTERNAL: {
            Uint32 encoded;
            Sint32 pool_index;
            Uint32 fragment_index;

            encoded = source->buff;
            pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
            fragment_index = (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
            source_address = (Uint32)(*os_pool_table)[pool_index].base;
            /* @hack spelling: the fragment's record as a sum, where
             * fragments[fragment_index] would do */
            source_address +=
                (fragment_index + (*os_pool_table)[pool_index].fragments)
                    ->offset +
                (encoded & MP_OFFSET_MASK);
        }
            memcpy((void *)destination->buff, (void *)source_address, size);
            break;
        }
        break;
    case AT_PTR_TYPE_EXTERNAL:
        switch (VPTR_TYPE(destination)) {
        case AT_PTR_TYPE_LOCAL:
            memcpy((void *)destination->buff, (void *)source->buff, size);
            break;
        case AT_PTR_TYPE_POOL: {
            Uint32 encoded;
            Sint32 pool_index;
            Uint32 fragment_index;

            encoded = destination->buff;
            pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
            fragment_index = (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
            /* @hack redundant: the 0 assigned to encoded, which never runs */
            if (0) {
                encoded = 0;
            }
            destination_address = (Uint32)(*os_pool_table)[pool_index].base;
            destination_address +=
                (*os_pool_table)[pool_index].fragments[fragment_index].offset +
                (encoded & MP_OFFSET_MASK);
        }
            memcpy((void *)destination_address, (void *)source->buff, size);
            break;
        case AT_PTR_TYPE_EXTERNAL:
            memcpy((void *)destination->buff, (void *)source->buff, size);
            break;
        }
        break;
    }
#endif
}

void OS_SetVptrLocal(AT_Vptr *pointer, void *address)
{
    pointer->buff = (Uint32)address;
    pointer->type = AT_PTR_TYPE_LOCAL;
}

void OS_SetVptrExternal(AT_Vptr *pointer, void *address)
{
    pointer->buff = (Uint32)address;
    pointer->type = AT_PTR_TYPE_EXTERNAL;
}

void *OS_GetVptrLocal(AT_Vptr *pointer)
{
    return (void *)pointer->buff;
}

void OS_IncVptr(AT_Vptr *pointer, Sint32 size)
{
    Uint8 *cursor;
    switch (VPTR_TYPE(pointer)) {
    case AT_PTR_TYPE_LOCAL:
        cursor = (Uint8 *)pointer->buff;
        cursor += size;
        pointer->buff = (Uint32)cursor;
        return;
    case AT_PTR_TYPE_POOL:
        pointer->buff += size;
        return;
    case AT_PTR_TYPE_EXTERNAL:
        cursor = (Uint8 *)pointer->buff;
        cursor += size;
        pointer->buff = (Uint32)cursor;
        return;
    default:
        return;
    }
}

Uint32 OS_ChksumBuffs(Uint32 seed, AT_MaxBuffs *buffs, Sint32 reverse)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in both walks below: it skips a buffer
     * that holds no bytes, so an odd byte pairs with the next buffer's first.
     * one loop walks the list either way, and reads mcb_table by its own name
     * through buffer_address */
    Uint32 total = seed;
    int index;
    int pending = 0;
    for (index = 0; index < buffs->bufnum; ++index) {
        const AT_VptrArray *segment =
            &buffs->buffers[reverse == 1 ? buffs->bufnum - index - 1 : index];
        int length = segment->len;
        Uint8 *data;
        if (length <= 0) {
            continue;
        }
        data = segment->vptr.type == AT_PTR_TYPE_POOL
                   ? buffer_address(&segment->vptr)
                   : (Uint8 *)segment->vptr.buff;
        if (pending) {
            total += *data++;
            --length;
        }
        sum_cul(&total, data, (Sint16)(length / 2));
        pending = length & 1;
        if (pending) {
            total += (Uint32)data[length - 1] << 8;
        }
    }
    return total;
#else
    Uint32 total = seed;

    Sint32 has_high_byte = 0;
    Sint32 count = buffs->bufnum;
    Uint16 high_byte = 0;
    Sint32 index;
    Sint16 length;
    Uint8 *data;
    AT_Vptr *pointer;

    if (reverse == 1) {
        while (count) {
            count--;
            pointer = &buffs->buffers[count].vptr;
            if (VPTR_TYPE(pointer) == AT_PTR_TYPE_LOCAL) {
                data = (Uint8 *)pointer->buff;
            } else if (VPTR_TYPE(pointer) == AT_PTR_TYPE_POOL) {
                Uint32 encoded;
                Sint32 pool_index;
                Uint32 fragment_index;
                Uint32 address;
                Uint32 pool_address;
                /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
                Uint32 unused;

                encoded = pointer->buff;
                fragment_index =
                    (encoded & MP_FRAGMENT_MASK) >> MP_FRAGMENT_SHIFT;
                pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
                /* @hack spelling: the page's record reached through
                 * pool_address, which first holds a record's size, where
                 * &(*os_pool_table)[pool_index] would do */
                pool_address = sizeof(struct memory_pool);

                address =
                    (Uint32)((struct memory_pool *)(pool_address =
                                                        (pool_index *
                                                         pool_address) +
                                                        (Uint32)*os_pool_table))
                        ->base;
                /* @hack redundant: the reads of alloc_max_buffs into unused,
                 * which nothing reads */
                unused = pool_index ? alloc_max_buffs->buffers.bufnum : 0;
                unused = alloc_max_buffs->buffers.bufnum || count;
                address += ((struct memory_pool *)pool_address)
                               ->fragments[fragment_index]
                               .offset +
                           (encoded & MP_OFFSET_MASK);
                data = (Uint8 *)address;
            } else {
                data = (Uint8 *)buffs->buffers[count].vptr.buff;
            }
            length = buffs->buffers[count].len;
            /* @bug memory: reads two bytes outside an empty buffer that
             * follows one of odd length. the load of its first byte reads
             * past its end. its length then falls to -1, and the load of its
             * last byte reads the byte before its start (0x0cad8a10,
             * 0x0cad8a2e) */
            if (has_high_byte) {
                high_byte |= *data;
                data++;
                total += high_byte;
                length--;
            }
            if (length & 1) {
                high_byte = (data[length - 1] << 8) & 0xff00;
                has_high_byte = 1;
                length--;
            } else {
                has_high_byte = 0;
                high_byte = 0;
            }
            length >>= 1;
            sum_cul(&total, data, length);
        }
        if (has_high_byte) {
            total += high_byte;
        }
    } else {
        for (index = 0; index < count; index++) {
            pointer = &buffs->buffers[index].vptr;
            if (VPTR_TYPE(pointer) == AT_PTR_TYPE_LOCAL) {
                data = (Uint8 *)pointer->buff;
            } else if (VPTR_TYPE(pointer) == AT_PTR_TYPE_POOL) {
                Uint32 encoded;
                Sint32 pool_index;
                Uint32 fragment_index;
                Uint32 address;
                Uint32 pool_address;
                /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
                Uint32 unused;

                encoded = pointer->buff;
                /* @hack spelling: the fragment's index masked here and
                 * shifted below, where one statement would do */
                fragment_index = encoded & MP_FRAGMENT_MASK;
                pool_index = (encoded & MP_PAGE_MASK) >> MP_PAGE_SHIFT;
                /* @hack spelling: the page's record reached through
                 * pool_address, which first holds a record's size, where
                 * &(*os_pool_table)[pool_index] would do */
                pool_address = sizeof(struct memory_pool);
                pool_address = (Uint32) & (*os_pool_table)->base +
                                              (pool_index * pool_address);
                /* @hack redundant: the read of alloc_max_buffs into unused,
                 * which nothing reads */
                unused = pool_index ? alloc_max_buffs->buffers.bufnum : 0;
                address = (Uint32)((struct memory_pool *)pool_address)->base;
                fragment_index >>= MP_FRAGMENT_SHIFT;
                address += ((struct memory_pool *)pool_address)
                               ->fragments[fragment_index]
                               .offset +
                           (encoded & MP_OFFSET_MASK);
                data = (Uint8 *)address;
            } else {
                data = (Uint8 *)buffs->buffers[index].vptr.buff;
            }
            length = buffs->buffers[index].len;
            /* @bug memory: reads two bytes outside an empty buffer that
             * follows one of odd length. the load of its first byte reads
             * past its end. its length then falls to -1, and the load of its
             * last byte reads the byte before its start (0x0cad8ace,
             * 0x0cad8aea) */
            if (has_high_byte) {
                high_byte |= *data;
                data++;
                total += high_byte;
                length--;
            }
            if (length & 1) {
                high_byte = (data[length - 1] << 8) & 0xff00;
                has_high_byte = 1;
                length--;
            } else {
                has_high_byte = 0;
                high_byte = 0;
            }
            length >>= 1;
            sum_cul(&total, data, length);
        }
        if (has_high_byte) {
            total += high_byte;
        }
    }
    return total;
#endif
}

Sint32 OS_FreeMempool(AT_Vptr *pointer)
{
    if (pointer && VPTR_TYPE(pointer) == AT_PTR_TYPE_POOL) {
        if (MP_Free(pointer) == 0) {
            return 0;
        }
    }
    return -1;
}

void sum_cul(Uint32 *seed, void *address, Sint16 count)
{
    Uint8 *data = address;
    Uint32 value = *seed;
    Sint32 index;
    for (index = 0; index < count; index++) {
        value += *data++ << 8;
        value += *data++;
    }
    *seed = value;
}

void OS_Create(void)
{
    vsStartTcpipDaemon();
    while (TcpTaskCount != 1) {
        OS_Yield();
    }
}

void OS_Destroy(void)
{
    TcpTaskTerminateReq = 1;
    while (TcpTaskCount) {
        OS_Yield();
    }
    TcpTaskTerminateReq = 0;
}

void OS_Yield(void)
{
    vsWaitVSync(1);
}

void OS_CriticalLock(void)
{
    tcpipSavedLevel = vsEnterCriticalSection();
}

void OS_CriticalUnLock(void)
{
    vsLeaveCriticalSection(tcpipSavedLevel);
}

void OS_Wait(Sint32 delay)
{
    Sint32 elapsed;
    for (elapsed = 0; elapsed < delay; elapsed += VS_TICK_MILLISECONDS) {
        vsPauseTcpipDaemon();
    }
}

void OS_RcvTskWakeup(void)
{
}

void OS_Pause(void)
{
    vsPauseTcpipDaemon();
}

Sint32 TIMER_GetCounter(void)
{
    return vsGetTickCount() * VS_TICK_MILLISECONDS;
}

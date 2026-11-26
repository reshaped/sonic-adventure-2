/* the bytes of an event file that lie past a pointer into it. GCC's build
 * checks the file's pointers by them */
#ifndef EVENT_FILE_SPAN_H
#define EVENT_FILE_SPAN_H
#include <sg_xpt.h>
#include <stdint.h>

/* the bytes from field to the end of the file at bank, bytes long. it
 * returns 0 where field lies outside the file. bank and field may each be a P0
 * or a P1 address. the disc's files hold P0 pointers, and the loaders read the
 * same RAM through P1. bit 31 alone tells the two apart, so clearing it takes
 * each address to P0. an address in any other segment lies outside the file */
static Uint32 event_file_remaining(const void *bank, Uint32 bytes,
                                   const void *field)
{
    uintptr_t base = (uintptr_t)bank;
    uintptr_t at = (uintptr_t)field;

    base &= ~(uintptr_t)0x80000000U;
    at &= ~(uintptr_t)0x80000000U;
    if (at < base || at - base >= bytes) {
        return 0;
    }
    return bytes - (Uint32)(at - base);
}

#endif

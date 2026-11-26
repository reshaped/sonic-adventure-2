/* the GCC build's checks that an event file's records lie inside it */
#ifndef EVENT_FILE_H
#define EVENT_FILE_H

#include <ninja.h>
#include <stdint.h>

#include "event/file_span.h"

/* whether count records of stride bytes, from records, lie inside the file at
 * base, whose size is bytes. it is FALSE for null records, and for records off
 * a 4-byte boundary. it is FALSE for a count or a stride of 0, and for bytes
 * of 0 or less */
static Bool event_file_records(const void *base, Sint32 bytes,
                               const void *records, Uint32 count, Uint32 stride)
{
    return bytes > 0 && records != NULL &&
           (uintptr_t)records % sizeof(Uint32) == 0 && count > 0 &&
           stride > 0 &&
           count <= event_file_remaining(base, (Uint32)bytes, records) / stride;
}

/* whether the texture list and its names lie inside the file at base, as
 * event_file_records tells */
static Bool event_file_texlist(const void *base, Sint32 bytes,
                               const NJS_TEXLIST *list)
{
    return event_file_records(base, bytes, list, 1, sizeof(*list)) &&
           event_file_records(base, bytes, list->textures, list->nbTexture,
                              sizeof(*list->textures));
}

#endif

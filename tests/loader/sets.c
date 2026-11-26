/* Exercise the SET loader with a reusable GD-ROM work area. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "object/set.h"
#include "system/areas.h"
#include "system/loader.h"

static struct set_head scratch[128];
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA ((Uint8 *)scratch)
#define WORK_AREA_END (WORK_AREA + sizeof(scratch))
#include "object/set.c"

struct fixture {
    struct set_head head;
    struct set_entry entries[2];
};

static struct fixture files[2];
static Sint32 sizes[2];
static int refused = -1;
static Bool store_full;
static int live;

static int file_index(const char *name)
{
    assert(strcmp(name, "s") == 0 || strcmp(name, "u") == 0);
    return strcmp(name, "u") == 0;
}

Sint32 loader_file_size(const char *name)
{
    return sizes[file_index(name)];
}

Sint32 loader_read_file_checked(const char *name, void *destination,
                                Uint32 capacity)
{
    int index = file_index(name);
    Sint32 bytes = sizes[index];

    assert(capacity == sizeof(scratch));
    if (index == refused) {
        memset(destination, 0xa5, 32);
        return -1;
    }
    if (bytes < 0 || (Uint32)bytes > capacity) {
        return -1;
    }
    assert((Uint32)bytes <= sizeof(files[index]));
    memcpy(destination, &files[index], bytes);
    return bytes;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *buffer;

    if (store_full) {
        return NULL;
    }
    buffer = calloc(count, size);
    assert(buffer != NULL);
    live++;
    return buffer;
}

void syFree(void *buffer)
{
    assert(live > 0 && buffer != NULL);
    live--;
    free(buffer);
}

void njMemCopy32(void *dest, const void *src, Sint32 blocks)
{
    assert(blocks > 0);
    memcpy(dest, src, blocks * 32U);
}

static void check_fixed_buffer(void)
{
    struct fixture destination;
    struct fixture previous;

    memset(&destination, 0x5a, sizeof(destination));
    previous = destination;
    refused = 0;
    assert(!set_read_into("s", &destination.head, sizeof(destination)));
    assert(memcmp(&destination, &previous, sizeof(destination)) == 0);
    refused = -1;
    assert(!set_read_into("s", &destination.head, sizeof(destination.head)));
    assert(memcmp(&destination, &previous, sizeof(destination)) == 0);
    assert(set_read_into("s", &destination.head, sizeof(destination)));
    assert(memcmp(&destination, &files[0], sizes[0]) == 0);
    assert(memcmp(&destination.entries[1], &previous.entries[1],
                  sizeof(struct set_entry)) == 0);
}

int main(void)
{
    struct set_head *head;
    struct set_entry *entries;
    struct set_entry empty = {0};

    assert(sizeof(struct set_head) == 32 && sizeof(struct set_entry) == 32);
    files[0].head.count = 1;
    files[0].head.unk_04[3] = 0x52;
    files[0].entries[0].id = 19;
    files[1].head.count = 2;
    files[1].entries[0].id = 41;
    files[1].entries[1].id = 47;
    sizes[0] = 64;
    sizes[1] = 96;
    check_fixed_buffer();

    head = set_read("s", "u", 6);
    assert(head != NULL && head->count == 3 && head->unk_04[3] == 0x52);
    entries = (struct set_entry *)(head + 1);
    assert(entries[0].id == 19 && entries[1].id == 41 && entries[2].id == 47);
    assert(memcmp(&entries[4], &empty, sizeof(empty)) == 0);
    syFree(head);

    store_full = TRUE;
    assert(set_read("s", "u", 0) == NULL && live == 0);
    store_full = FALSE;
    sizes[1] = -1;
    assert(set_read("s", "u", 0) == NULL && live == 0);
    sizes[1] = 96;
    refused = 0;
    assert(set_read("s", "u", 0) == NULL && live == 0);
    refused = 1;
    assert(set_read("s", "u", 0) == NULL && live == 0);
    refused = -1;

    files[1].head.count = 1;
    assert(set_read("s", "u", 0) == NULL && live == 0);
    files[1].head.count = 0;
    sizes[1] = 32;
    head = set_read("s", "u", 0);
    assert(head != NULL && head->count == 1);
    syFree(head);

    sizes[0] = 63;
    assert(set_read("s", NULL, 0) == NULL && live == 0);
    sizes[0] = 64;
    head = set_read("s", NULL, 512);
    assert(head != NULL && head->count == 1);
    entries = (struct set_entry *)(head + 1);
    assert(entries[0].id == 19);
    assert(memcmp(&entries[510], &empty, sizeof(empty)) == 0);
    syFree(head);
    return live;
}

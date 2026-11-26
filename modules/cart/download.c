/* the kart pack a download course plays: its course record, set file, layout
 * and model archive. cart_download_load reads it from the download file
 * download_chosen names */
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/course.h"
#include "save/save.h"
#include "system/areas.h"
#include "system/prs.h"

#ifdef NONMATCHING
/* memcpy, the pack's format and the tables of the race, the stage and the
 * track. this side checks a pack by them before it takes it */
#include <string.h>

#include "cart/download_format_impl.h"
#include "cart/race.h"
#include "cart/stage.h"
#include "cart/track.h"
#endif

/* a kart pack's parts. the matching load decodes the pack over this record.
 * each pointer then holds its part's offset in the pack, until the load adds
 * the pack's address */
struct cart_download {
    struct cart_course *course;
    void *set;
    Sint32 set_size;
    Sint8 *layout;
    Sint32 layout_size;
    struct cart_archive *model;
};

#ifdef NONMATCHING
enum pack_part { COURSE, SET, LAYOUT, MODEL, PARTS };

static union {
    Uint32 alignment;
    Uint8 bytes[CART_PACK_CAPACITY];
} buffer;
static struct cart_download current;
static struct cart_download *pack = &current;
static Uint32 model_size;

static Bool valid_set(const Uint8 *set, Uint32 size)
{
    Uint32 count, i;
    Uint32 kind;
    if (size < 32 || size > 32768 || size % 32) {
        return FALSE;
    }
    count = cart_word(set);
    if (count > (size - 32) / 32) {
        return FALSE;
    }
    for (i = 0; i < count; ++i) {
        set += 32;
        kind = (set[0] | ((Uint32)set[1] << 8)) & SET_KIND;
        if (kind >= (Uint32)cart_objects.count) {
            return FALSE;
        }
    }
    return TRUE;
}

static Bool valid_layout(const Uint8 *pieces, Uint32 size)
{
    Uint32 gates = 0;
    Uint32 i, kind;
    if (size > CART_SECTIONS) {
        return FALSE;
    }
    for (i = 0; i < size && pieces[i] != 0xff; ++i) {
        kind = pieces[i];
        if (kind >= CART_PIECES) {
            return FALSE;
        }
        if (cart_piece_gates[kind].gates) {
            if ((Uint32)cart_piece_gates[kind].count > CART_GATES - gates) {
                return FALSE;
            }
            gates += cart_piece_gates[kind].count;
        }
    }
    /* a CPU kart first heads for the gate after its slot in cart_racers. a
     * layout thus needs more gates than racers */
    return gates > CART_RACERS;
}

/* a pack opens with seven words. the first is the course record's offset. the
 * set file, the layout and the model archive then each give an offset and a
 * size. the course record has no size of its own. it ends in a music name of
 * up to MUSIC_BYTES */
static Bool read_parts(const Uint8 *data, Uint32 size, struct cart_region *part)
{
    enum { HEADER_BYTES = 28, MUSIC_OFFSET = 68, MUSIC_BYTES = 64 };
    if (size < HEADER_BYTES) {
        return FALSE;
    }
    part[COURSE].offset = cart_word(data);
    part[COURSE].size = MUSIC_OFFSET + MUSIC_BYTES;
    part[SET].offset = cart_word(data + 4);
    part[SET].size = cart_word(data + 8);
    part[LAYOUT].offset = cart_word(data + 12);
    part[LAYOUT].size = cart_word(data + 16);
    part[MODEL].offset = cart_word(data + 20);
    part[MODEL].size = cart_word(data + 24);
    return cart_regions(part, PARTS, HEADER_BYTES, size) &&
           !((part[COURSE].offset | part[MODEL].offset) & 3) &&
           part[SET].size && part[LAYOUT].size && part[MODEL].size &&
           cart_string(data + part[COURSE].offset + MUSIC_OFFSET,
                       MUSIC_BYTES) &&
           valid_set(data + part[SET].offset, part[SET].size) &&
           valid_layout(data + part[LAYOUT].offset, part[LAYOUT].size);
}

int cart_download_load(void)
{
    const void *compressed;
    Uint32 stored, decoded;
    Uint8 *candidate = WORK_AREA_FILE;
    struct cart_region part[PARTS];

    compressed = save_download_load_sized(
        &download_chosen, (struct download_file *)WORK_AREA, &stored);
    if (!compressed ||
        !prs_decompress_checked(compressed, stored, candidate,
                                sizeof(buffer.bytes), NULL, &decoded) ||
        !read_parts(candidate, decoded, part) ||
        !cart_replace_download_model(candidate + part[MODEL].offset,
                                     part[MODEL].size)) {
        return FALSE;
    }
    memcpy(buffer.bytes, candidate, decoded);
    pack->course = (struct cart_course *)(buffer.bytes + part[COURSE].offset);
    pack->set = buffer.bytes + part[SET].offset;
    pack->set_size = part[SET].size;
    pack->layout = (Sint8 *)(buffer.bytes + part[LAYOUT].offset);
    pack->layout_size = part[LAYOUT].size;
    pack->model = (struct cart_archive *)(buffer.bytes + part[MODEL].offset);
    model_size = part[MODEL].size;
    return TRUE;
}

Uint32 cart_download_model_size(void)
{
    return model_size;
}

void cart_download_copy_set(void *to, Uint32 capacity)
{
    if (to && pack->set && (Uint32)pack->set_size <= capacity) {
        memcpy(to, pack->set, pack->set_size);
    }
}

void cart_download_copy_layout(void *to, Uint32 capacity)
{
    if (to && pack->layout && (Uint32)pack->layout_size <= capacity) {
        memcpy(to, pack->layout, pack->layout_size);
    }
}
#else
static Uint8 buffer[0x19000];
static struct cart_download *pack = (struct cart_download *)buffer;

int cart_download_load(void)
{
    void *packed;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((packed = save_download_load(
             &download_chosen, (struct download_file *)WORK_AREA)) != NULL) {
        prs_decompress(packed, buffer);
        pack->course =
            (struct cart_course *)((Uint32)pack->course + (Uint32)buffer);
        pack->set = (void *)((Uint32)pack->set + (Uint32)buffer);
        pack->layout = (Sint8 *)((Uint32)pack->layout + (Uint32)buffer);
        pack->model =
            (struct cart_archive *)((Uint32)pack->model + (Uint32)buffer);
        return TRUE;
    }
    return FALSE;
}
void cart_download_copy_set(void *to)
{
    njMemCopy(to, pack->set, pack->set_size);
}
void cart_download_copy_layout(void *to)
{
    njMemCopy(to, pack->layout, pack->layout_size);
}

#endif
struct cart_archive *cart_download_model(void)
{
    return pack->model;
}
struct cart_course *cart_download_course(void)
{
    return pack->course;
}

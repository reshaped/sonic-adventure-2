/* the tile accelerator's setup: each bank's layout and region array */
#include <kamui.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

/* the video chip's registers, as the offsets kmiWriteRegister takes */
#define TA_TILE_CLIP 0x13c
#define TA_ALLOC_CONTROL 0x140
/* TA_TILE_CLIP takes the last tile's x from bit 0, and its y from bit
 * TILE_CLIP_Y_SHIFT */
#define TILE_CLIP_Y_SHIFT 16
/* KMI_SOFT_RESET's bit that holds the tile accelerator in reset */
#define RESET_TA 1
/* KMI_TA_LIST_INIT's value that starts the lists afresh */
#define INIT_LISTS 0x80000000UL

/* OPB_CODE masks an object pointer block's size code, two bits that index
 * opb_words */
#define OPB_CODE 3UL
/* TA_ALLOC_CONTROL takes each list's size code at bit ALLOC_SHIFT(list), list
 * a ListType. on core 6, opb_downward's bit lies at
 * ALLOC_DOWNWARD_SHIFT_CORE6, past the punch-through list's code. on core 5
 * it lies at ALLOC_DOWNWARD_SHIFT_CORE5, and the punch-through list gets no
 * code */
#define ALLOC_SHIFT(list) (4 * (list))
#define ALLOC_DOWNWARD_SHIFT_CORE6 20
#define ALLOC_DOWNWARD_SHIFT_CORE5 16

/* the options word kmiSetTAEnvironment takes. KMI_OPTION_BY_COLUMNS orders
 * the tiles KMI_BY_COLUMNS, and OPTION_OPB_DOWNWARD sets opb_downward. each
 * list's size code lies at OPTION_OPB_SHIFT(list) */
#define OPTION_OPB_DOWNWARD_SHIFT 16
#define OPTION_OPB_DOWNWARD (1UL << OPTION_OPB_DOWNWARD_SHIFT)
#define OPTION_OPB_SHIFT(list) (17 + (2 * (list)))
#define OPTION_OPB(options, list)                                              \
    (((options) & (OPB_CODE << OPTION_OPB_SHIFT(list))) >>                     \
     OPTION_OPB_SHIFT(list))

/* REGION_X and REGION_Y place a tile's x and y in its entry's control word */
#define REGION_X(x) (((x) & 63) << 2)
#define REGION_Y(y) (((y) & 63) << 8)
/* the control word's bit for the array's last entry */
#define REGION_LAST_SHIFT 31
#define REGION_LAST (1UL << REGION_LAST_SHIFT)
/* the control word's bit that leaves the tile's depths uncleared */
#define REGION_KEEP_DEPTH 0x40000000UL
/* the control word's bit that draws translucent polygons in the order sent */
#define REGION_PRESORT_SHIFT 29
#define REGION_PRESORT (1UL << REGION_PRESORT_SHIFT)
/* the control word's bit that leaves the tile unwritten to the frame buffer */
#define REGION_NO_FLUSH 0x10000000UL
/* a list word's bit for a list the tile doesn't draw */
#define LIST_EMPTY 0x80000000UL

/* a tile's entry in the region array: its control word, then a word for each
 * list. a list's word is where its object pointer blocks for the tile start,
 * in ListType order */
struct pvr_region {
    KMDWORD control;
    KMDWORD opaque;
    KMDWORD opaque_modifier;
    KMDWORD translucent;
    KMDWORD translucent_modifier;
    KMDWORD punch_through;
};

/* the dummy translucent object: an object list of DUMMY_LIST_WORDS words,
 * then the one triangle it names. under KMI_FLUSH_ENTRY, each tile's second
 * entry draws the dummy as its translucent list. the triangle covers the
 * screen, but DUMMY_ISP's depth compare never passes, so it draws nothing */
#define DUMMY_LIST_WORDS 2
struct dummy_vertex {
    KMFLOAT x, y, z;
    KMDWORD color;
};
struct dummy_triangle {
    KMDWORD isp, tsp, texture;
    struct dummy_vertex vertex[3];
};
struct dummy_object {
    KMDWORD list[DUMMY_LIST_WORDS];
    struct dummy_triangle triangle;
};
/* texture memory's 32-bit view, through the uncached area */
#define TEXTURE_MEMORY 0xa5000000UL
/* the object list's entry for one triangle whose vertices each hold a word
 * past x, y and z. its low bits take the triangle's address, in words from
 * the bank's start */
#define OL_ONE_TRIANGLE 0x80200000
/* the object list's end */
#define OL_END 0xf0000000
/* the triangle's ISP word: Gouraud shading, and the depth compare mode that
 * never passes */
#define DUMMY_ISP 0x00800000
/* the triangle's TSP word: blend factors one and one, no fog, and the
 * vertices' alpha */
#define DUMMY_TSP 0x24900000

/* a bank's ISP parameters take ISP_QUARTERS quarters of its buffer. the
 * object lists' end beside them rounds down to a multiple of OL_LOW_ALIGN
 * bytes. their end below the region array rounds down to one of
 * OL_HIGH_ALIGN. isp_limit stops ISP_LIMIT_SLACK bytes short of the ISP
 * parameters' end */
#define ISP_QUARTERS 3
#define OL_LOW_ALIGN 32
#define OL_HIGH_ALIGN 128
#define ISP_LIMIT_SLACK 256

/* the words of an object pointer block, by its size code */
static KMDWORD opb_words[4] = {0, 8, 16, 32};
/* the member exports this flag, though no header of the SDK declares it.
 * while it is set, each tile's punch-through list takes the translucent
 * list's object pointer blocks */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
KMBOOLEAN gkmCopyTransToPunchthrough = FALSE;

/* the bytes of the first object pointer blocks: for each tile, one block of
 * each list's size */
static KMDWORD object_list_bytes(KMDWORD tiles)
{
    KMDWORD bytes;
    bytes = (opb_words[kmiDeviceExtension.opaque_opb] +
             opb_words[kmiDeviceExtension.opaque_modifier_opb] +
             opb_words[kmiDeviceExtension.translucent_opb] +
             opb_words[kmiDeviceExtension.translucent_modifier_opb] +
             opb_words[kmiDeviceExtension.punch_through_opb]) *
            tiles * sizeof(KMDWORD);
    return bytes;
}

/* point the tile accelerator at bank's buffers and clip it to width by height
 * tiles. load each list's block size, and start the lists afresh */
static void load_ta_registers(KMDWORD width, KMDWORD height, KMDWORD bank)
{
    KMDWORD allocation;
    kmiWriteRegister(KMI_TA_OL_BASE, kmiDeviceExtension.ta_bank[bank].ol_base);
    kmiWriteRegister(KMI_TA_OL_LIMIT,
                     kmiDeviceExtension.ta_bank[bank].ol_limit);
    kmiWriteRegister(KMI_TA_ISP_BASE,
                     kmiDeviceExtension.ta_bank[bank].isp_base);
    kmiWriteRegister(KMI_TA_ISP_LIMIT,
                     kmiDeviceExtension.ta_bank[bank].isp_limit);
    kmiWriteRegister(TA_TILE_CLIP,
                     ((height - 1) << TILE_CLIP_Y_SHIFT) + width - 1);
    allocation = ((kmiDeviceExtension.opaque_opb & OPB_CODE)
                  << ALLOC_SHIFT(KM_OPAQUE_POLYGON)) |
                 ((kmiDeviceExtension.opaque_modifier_opb & OPB_CODE)
                  << ALLOC_SHIFT(KM_OPAQUE_MODIFIER)) |
                 ((kmiDeviceExtension.translucent_opb & OPB_CODE)
                  << ALLOC_SHIFT(KM_TRANS_POLYGON)) |
                 ((kmiDeviceExtension.translucent_modifier_opb & OPB_CODE)
                  << ALLOC_SHIFT(KM_TRANS_MODIFIER));
    if (kmiDeviceExtension.core == 6) {
        allocation |= (kmiDeviceExtension.opb_downward & 1)
                      << ALLOC_DOWNWARD_SHIFT_CORE6;
        allocation |= (kmiDeviceExtension.punch_through_opb & OPB_CODE)
                      << ALLOC_SHIFT(KM_PUNCHTHROUGH_POLYGON);
    } else {
        allocation |= (kmiDeviceExtension.opb_downward & 1)
                      << ALLOC_DOWNWARD_SHIFT_CORE5;
    }
    kmiWriteRegister(TA_ALLOC_CONTROL, allocation);
    kmiWriteRegister(KMI_TA_LIST_INIT, INIT_LISTS);
}

/* write the dummy translucent object at address, a byte offset in texture
 * memory, and return address. bank_start is the offset of the bank's start */
static KMDWORD kmiCreateDummyTranslucentObject(KMDWORD address,
                                               KMDWORD bank_start)
{
    KMDWORD *list;
    list = (KMDWORD *)(address | TEXTURE_MEMORY);
    list[0] = (((address - bank_start) / sizeof(KMDWORD)) + DUMMY_LIST_WORDS) |
              OL_ONE_TRIANGLE;
    list[1] = OL_END;
    /* @hack type: list stepped past the object list and cast to the triangle
     * at each store, where a struct dummy_triangle pointer would do */
    list += DUMMY_LIST_WORDS;
    ((struct dummy_triangle *)list)->isp = DUMMY_ISP;
    ((struct dummy_triangle *)list)->tsp = DUMMY_TSP;
    ((struct dummy_triangle *)list)->texture = 0;
    ((struct dummy_triangle *)list)->vertex[0].x = 0.0F;
    ((struct dummy_triangle *)list)->vertex[0].y = 0.0F;
    ((struct dummy_triangle *)list)->vertex[0].z = 1.0e-10F;
    ((struct dummy_triangle *)list)->vertex[0].color = 0x80ff0000;
    ((struct dummy_triangle *)list)->vertex[1].x = 0.0F;
    ((struct dummy_triangle *)list)->vertex[1].y = 1280.0F;
    ((struct dummy_triangle *)list)->vertex[1].z = 1.0e-10F;
    ((struct dummy_triangle *)list)->vertex[1].color = 0x8000ff00;
    ((struct dummy_triangle *)list)->vertex[2].x = 2560.0F;
    ((struct dummy_triangle *)list)->vertex[2].y = 0.0F;
    ((struct dummy_triangle *)list)->vertex[2].z = 1.0e-10F;
    ((struct dummy_triangle *)list)->vertex[2].color = 0x800000ff;
    return address;
}

#define CLEAR_LISTS(record)                                                    \
    (record).opaque = LIST_EMPTY;                                              \
    (record).opaque_modifier = LIST_EMPTY;                                     \
    (record).translucent = LIST_EMPTY;                                         \
    (record).translucent_modifier = LIST_EMPTY;                                \
    (record).punch_through = LIST_EMPTY

/* write the entry that draws nothing at address, ahead of the first tile's,
 * on a chip with KMI_EMPTY_FIRST_ENTRY */
static void kmiCreateDummyRegionforAvoidCacheBug(KMBYTE *address)
{
    struct pvr_region empty;
    empty.control = REGION_NO_FLUSH;
    CLEAR_LISTS(empty);
    kmiCopyToPVRMemory(address, &empty, sizeof(empty));
}

void kmiCreateRegionArray(KMDWORD width, KMDWORD height, KMDWORD slot,
                          KMINT32 unused, KMINT32 bank, KMDWORD order)
{
    struct pvr_region first;
    struct pvr_region second;
    struct pvr_region third;
    struct pvr_region fourth;
    KMDWORD count;
    KMDWORD array_bytes;
    KMBYTE *base;
    KMBYTE *dest;
    KMDWORD dummy_list;
    KMBOOLEAN opaque_dummy;
    KMDWORD presort;
    KMDWORD i;
    KMINT32 x;
    KMINT32 y;
    KMDWORD last;
    KMDWORD sort_bit;
    KMDWORD opaque_start;
    KMDWORD opaque_modifier_start;
    KMDWORD translucent_start;
    KMDWORD translucent_modifier_start;
    KMDWORD punch_start;
    KMDWORD opaque_stride;
    KMDWORD opaque_modifier_stride;
    KMDWORD translucent_stride;
    KMDWORD translucent_modifier_stride;
    KMDWORD punch_stride;
    KMDWORD empty_opaque;
    KMDWORD empty_opaque_modifier;
    KMDWORD empty_translucent;
    KMDWORD empty_translucent_modifier;
    KMDWORD empty_punch;
    KMDWORD tile;
    KMDWORD clipped;

#ifdef NONMATCHING
    /* GCC's side starts dummy_list empty. the tile loop reads it only under
     * the two tests that guard its creation. it reads them again from the
     * volatile kmiDeviceExtension, and they can pass there where they failed
     * here. dummy_list would then be unset */
    dummy_list = LIST_EMPTY;
#endif
    opaque_dummy = 0;
    /* @hack spelling: an if that assigns in each branch, where presort =
     * kmiDeviceExtension.presort == 1 would do */
    if (kmiDeviceExtension.presort == 1) {
        presort = 1;
    } else {
        presort = 0;
    }
    if (!kmiDeviceExtension.ta_busy) {
        load_ta_registers(width, height, bank);
    }
    count = width * height;
    array_bytes = count * sizeof(struct pvr_region);
    base = (KMBYTE *)kmiDeviceExtension.ta_bank[bank].region_base +
           (slot * array_bytes);
    /* @hack redundant: if (1) around the dummy's creation */
    if (1) {
        if ((kmiDeviceExtension.capabilities & KMI_FLUSH_ENTRY) &&
            kmiDeviceExtension.dummy_translucent) {
            dummy_list = kmiCreateDummyTranslucentObject(
                kmiDeviceExtension.ta_bank[bank].region_base +
                    (array_bytes * 4),
                kmiDeviceExtension.ta_bank[bank].start);
            opaque_dummy = !kmiDeviceExtension.opaque_opb &&
                           kmiDeviceExtension.translucent_opb && presort;
        }
    }
    if (kmiDeviceExtension.capabilities & KMI_EMPTY_FIRST_ENTRY) {
        kmiCreateDummyRegionforAvoidCacheBug(base);
        base += sizeof(struct pvr_region);
    }
/* @hack spelling: an if that assigns in each branch, where flag =
 * !kmiDeviceExtension.field ? LIST_EMPTY : 0 would do */
#define EMPTY_FLAG(field, flag)                                                \
    if (!kmiDeviceExtension.field) {                                           \
        (flag) = LIST_EMPTY;                                                   \
    } else {                                                                   \
        (flag) = 0;                                                            \
    }
    EMPTY_FLAG(opaque_opb, empty_opaque)
    EMPTY_FLAG(opaque_modifier_opb, empty_opaque_modifier)
    EMPTY_FLAG(translucent_opb, empty_translucent)
    EMPTY_FLAG(translucent_modifier_opb, empty_translucent_modifier)
    EMPTY_FLAG(punch_through_opb, empty_punch)
#undef EMPTY_FLAG
    /* @hack redundant: five writes to the first entry that no path runs */
    if (0) {
        first.opaque = 33;
        first.opaque_modifier = 33;
        first.translucent = 33;
        first.translucent_modifier = 33;
        first.punch_through = 33;
    }
#define OPB_BYTES(field) (opb_words[kmiDeviceExtension.field] * sizeof(KMDWORD))
    opaque_stride = OPB_BYTES(opaque_opb);
    opaque_modifier_stride = OPB_BYTES(opaque_modifier_opb);
    translucent_stride = OPB_BYTES(translucent_opb);
    translucent_modifier_stride = OPB_BYTES(translucent_modifier_opb);
    punch_stride = OPB_BYTES(punch_through_opb);
    opaque_start = kmiDeviceExtension.ta_bank[bank].ol_base;
#define NEXT_LIST(start, field)                                                \
    ((start) + (opb_words[kmiDeviceExtension.field] * count * sizeof(KMDWORD)))
    opaque_modifier_start = NEXT_LIST(opaque_start, opaque_opb);
    translucent_start = NEXT_LIST(opaque_modifier_start, opaque_modifier_opb);
    translucent_modifier_start = NEXT_LIST(translucent_start, translucent_opb);
    punch_start =
        NEXT_LIST(translucent_modifier_start, translucent_modifier_opb);
    i = 0;
    last = count - 1;
    /* @hack redundant: presort masked to its low bit, which it already is */
    presort = presort & 1;
    sort_bit = presort << REGION_PRESORT_SHIFT;
    /* @hack redundant: an empty do-while that runs once, before the tile
     * loop */
    do {
    } while (0);
    for (; i < count; i++) {
        clipped = 0;
        switch (order) {
        case KMI_BY_COLUMNS:
            x = i / kmiDeviceExtension.tiles_y;
            y = i % kmiDeviceExtension.tiles_y;
            break;
        case KMI_BY_ROWS:
            x = i % kmiDeviceExtension.tiles_x;
            y = i / kmiDeviceExtension.tiles_x;
            break;
#ifdef NONMATCHING
        /* this side returns for an order neither case takes, for which the
         * matching side reads x and y unset */
        default:
            return;
#endif
        }
        if (kmiDeviceExtension.global_clip == 1 &&
            (kmiDeviceExtension.global_clip_width < (KMUINT32)x ||
             kmiDeviceExtension.global_clip_height < (KMUINT32)y)) {
            clipped = LIST_EMPTY;
        }
        tile = (y * kmiDeviceExtension.tiles_x) + x;
        first.control = ((KMDWORD)(i == last) << REGION_LAST_SHIFT) |
                        REGION_Y(y) | REGION_X(x) | sort_bit;
#define REGION_LIST(start, stride, empty)                                      \
    ((((stride) * tile) + (start)) | (empty) | clipped)
        first.opaque = REGION_LIST(opaque_start, opaque_stride, empty_opaque);
        first.opaque_modifier =
            REGION_LIST(opaque_modifier_start, opaque_modifier_stride,
                        empty_opaque_modifier);
        first.translucent = REGION_LIST(translucent_start, translucent_stride,
                                        empty_translucent);
        first.translucent_modifier =
            REGION_LIST(translucent_modifier_start, translucent_modifier_stride,
                        empty_translucent_modifier);
        if (gkmCopyTransToPunchthrough) {
            first.punch_through = first.translucent;
        } else {
            first.punch_through =
                REGION_LIST(punch_start, punch_stride, empty_punch);
        }
        if (kmiDeviceExtension.another_modifier) {
            if (kmiDeviceExtension.another_modifier_list == 1) {
                first.translucent_modifier = first.opaque_modifier;
            } else {
                first.opaque_modifier = first.translucent_modifier;
            }
        }
        if (kmiDeviceExtension.capabilities & KMI_FLUSH_ENTRY) {
            if (kmiDeviceExtension.dummy_translucent) {
                /* the first entry draws the tile's opaque and punch-through
                 * lists. under opaque_dummy, where the opaque list has no
                 * blocks, it draws the dummy as that list. the second entry
                 * draws the dummy as its translucent list. the third draws
                 * the tile's translucent lists. the fourth draws nothing, and
                 * writes the tile out */
                fourth.control = third.control = second.control = first.control;
                first.control &= ~REGION_LAST;
                second.control &= ~REGION_LAST;
                third.control &= ~REGION_LAST;
                first.control &= ~REGION_KEEP_DEPTH;
                second.control |= REGION_KEEP_DEPTH;
                third.control |= REGION_KEEP_DEPTH;
                fourth.control |= REGION_KEEP_DEPTH;
                first.control |= REGION_NO_FLUSH;
                second.control |= REGION_NO_FLUSH;
                third.control |= REGION_NO_FLUSH;
                fourth.control &= ~REGION_NO_FLUSH;
                second.control &= ~REGION_PRESORT;
                if (opaque_dummy) {
                    first.opaque = dummy_list;
                }
                second.opaque_modifier = second.opaque = LIST_EMPTY;
                second.translucent = dummy_list;
                second.translucent_modifier = LIST_EMPTY;
                second.punch_through = LIST_EMPTY;
                third.opaque = LIST_EMPTY;
                third.opaque_modifier = LIST_EMPTY;
                third.translucent = first.translucent;
                third.translucent_modifier = first.translucent_modifier;
                third.punch_through = LIST_EMPTY;
                first.translucent = LIST_EMPTY;
                first.translucent_modifier = LIST_EMPTY;
                CLEAR_LISTS(fourth);
                dest = (i * sizeof(struct pvr_region) * 4) + base;
                kmiCopyToPVRMemory(dest, &first, sizeof(struct pvr_region));
                kmiCopyToPVRMemory(dest + sizeof(struct pvr_region), &second,
                                   sizeof(struct pvr_region));
                kmiCopyToPVRMemory(dest + (sizeof(struct pvr_region) * 2),
                                   &third, sizeof(struct pvr_region));
                kmiCopyToPVRMemory(dest + (sizeof(struct pvr_region) * 3),
                                   &fourth, sizeof(struct pvr_region));
            } else {
                /* the first entry draws the tile's lists. the second draws
                 * nothing, and writes the tile out */
                second.control = first.control;
                first.control &= ~REGION_LAST;
                first.control &= ~REGION_KEEP_DEPTH;
                first.control |= REGION_NO_FLUSH;
                second.control |= REGION_KEEP_DEPTH;
                second.control &= ~REGION_NO_FLUSH;
                CLEAR_LISTS(second);
                dest = (i * sizeof(struct pvr_region) * 2) + base;
                kmiCopyToPVRMemory(dest, &first, sizeof(struct pvr_region));
                kmiCopyToPVRMemory(dest + sizeof(struct pvr_region), &second,
                                   sizeof(struct pvr_region));
            }
        } else {
            dest = (i * sizeof(struct pvr_region)) + base;
            kmiCopyToPVRMemory(dest, &first, sizeof(struct pvr_region));
        }
    }
}

void kmiInitializeTA(void)
{
    kmiWriteRegister(KMI_SOFT_RESET, RESET_TA);
    kmiWriteRegister(KMI_SOFT_RESET, 0);
    kmiInitializeDMA();
    kmiDeviceExtension.unk_44c = 0x800000;
    kmiDeviceExtension.unk_454 = 0x600000;
    kmiDeviceExtension.unk_458 = kmiDeviceExtension.unk_454;
    kmiDeviceExtension.ta_bank[0].start = kmiDeviceExtension.unk_44c >> 1;
    kmiDeviceExtension.ta_bank[1].start = 0;
    kmiDeviceExtension.opb_downward = 1;
}

/* lay out both banks for options and write their region arrays. of the
 * call's parameters, it reads only options */
void kmiSetTAEnvironment(KMUINT32 region, KMUINT32 objects, KMUINT32 isp,
                         KMUINT32 tsp, KMUINT32 options, KMUINT32 buffer)
{
    KMDWORD order;
    KMDWORD list_bytes;
    KMINT32 i;
    kmiDeviceExtension.ta_options = options;
    if (options & KMI_OPTION_BY_COLUMNS) {
        order = KMI_BY_COLUMNS;
    } else {
        order = KMI_BY_ROWS;
    }
    kmiDeviceExtension.opb_downward =
        (options & OPTION_OPB_DOWNWARD) >> OPTION_OPB_DOWNWARD_SHIFT;
    kmiDeviceExtension.opaque_opb = OPTION_OPB(options, KM_OPAQUE_POLYGON);
    kmiDeviceExtension.opaque_modifier_opb =
        OPTION_OPB(options, KM_OPAQUE_MODIFIER);
    kmiDeviceExtension.translucent_opb = OPTION_OPB(options, KM_TRANS_POLYGON);
    kmiDeviceExtension.translucent_modifier_opb =
        OPTION_OPB(options, KM_TRANS_MODIFIER);
    kmiDeviceExtension.punch_through_opb =
        OPTION_OPB(options, KM_PUNCHTHROUGH_POLYGON);
    kmiDeviceExtension.isp_bytes =
        (kmiDeviceExtension.ta_buffer_size / 4) * ISP_QUARTERS;
    list_bytes = object_list_bytes(kmiDeviceExtension.tiles_x *
                                   kmiDeviceExtension.tiles_y);
    /* @hack redundant: an empty do-while that runs once, before the bank
     * loop */
    do {
    } while (0);
    /* a bank holds the ISP parameters, its first isp_bytes, then the object
     * lists. its last bytes hold the region array, after any empty first
     * entry. under KMI_FLUSH_ENTRY, while dummy_translucent is set, the dummy
     * follows the array. the first object pointer blocks start past the ISP
     * parameters. with opb_downward, they end just below the region array
     * instead. the tile accelerator takes further blocks from next_opb_init
     * toward ol_limit */
    for (i = 0; i < 2; i++) {
#define BANK kmiDeviceExtension.ta_bank[i]
#define REGION_BYTES                                                           \
    (kmiDeviceExtension.tiles_x * kmiDeviceExtension.tiles_y *                 \
     sizeof(struct pvr_region))
        BANK.region_base = BANK.start + kmiDeviceExtension.ta_buffer_size;
        if (kmiDeviceExtension.capabilities & KMI_FLUSH_ENTRY) {
            if (kmiDeviceExtension.dummy_translucent) {
                BANK.region_base -= sizeof(struct dummy_object);
                BANK.region_base -= REGION_BYTES * 4;
            } else {
                BANK.region_base -= REGION_BYTES * 2;
            }
        } else {
            BANK.region_base -= REGION_BYTES;
        }
        if (kmiDeviceExtension.capabilities & KMI_EMPTY_FIRST_ENTRY) {
            BANK.region_base -= sizeof(struct pvr_region);
        }
        if (!kmiDeviceExtension.opb_downward) {
            BANK.ol_base = (BANK.start + kmiDeviceExtension.isp_bytes) &
                           ~(OL_LOW_ALIGN - 1);
            BANK.ol_limit =
                (BANK.region_base - list_bytes) & ~(OL_HIGH_ALIGN - 1);
            BANK.next_opb_init = BANK.ol_base + list_bytes;
        } else {
            BANK.ol_base =
                (BANK.region_base - list_bytes) & ~(OL_HIGH_ALIGN - 1);
            BANK.ol_limit = (BANK.start + kmiDeviceExtension.isp_bytes) &
                            ~(OL_LOW_ALIGN - 1);
            BANK.next_opb_init = BANK.ol_base;
        }
        BANK.isp_base = BANK.start;
        BANK.isp_limit =
            BANK.start + kmiDeviceExtension.isp_bytes - ISP_LIMIT_SLACK;
#undef REGION_BYTES
#undef BANK
    }
    kmiCreateRegionArray(kmiDeviceExtension.tiles_x, kmiDeviceExtension.tiles_y,
                         0, 1, 0, order);
    kmiCreateRegionArray(kmiDeviceExtension.tiles_x, kmiDeviceExtension.tiles_y,
                         0, 1, 1, order);
}

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kmiResetTA(void);
void kmiFastTA(void);
void kmiFastTADirect(void);
void kmiVertexTransferTADirect(void);

void kmiResetTA(void)
{
    kmiWriteRegister(KMI_SOFT_RESET, RESET_TA);
    kmiWriteRegister(KMI_SOFT_RESET, 0);
}
void kmiFastTA(void)
{
}
void kmiFastTADirect(void)
{
}
void kmiVertexTransferTADirect(void)
{
}
/* NOLINTEND(misc-use-internal-linkage) */

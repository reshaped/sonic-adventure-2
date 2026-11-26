/* Kamui's DMA transfers to the tile accelerator's address space, on the CPU's
 * channel 2. requests wait in two rings of SLOTS slots, and one transfer runs
 * at a time. the interrupt at a transfer's end calls kmiExecuteDMAChain to
 * start the next */
#include <kamui.h>
#include <kamui_internal.h>
#include <machine.h>

/* channel 2's source, destination, transfer count and control registers, and
 * the DMA operation register */
#define SAR2 (*(volatile KMDWORD *)0xffa00020)
#define DAR2 (*(volatile KMDWORD *)0xffa00024)
#define DMATCR2 (*(volatile KMDWORD *)0xffa00028)
#define CHCR2 (*(volatile KMDWORD *)0xffa0002c)
#define DMAOR (*(volatile KMDWORD *)0xffa00040)
/* the store queues' address control registers, which take an address's area
 * shifted right by QACR_AREA_SHIFT */
#define QACR0 (*(volatile KMDWORD *)0xff000038)
#define QACR1 (*(volatile KMDWORD *)0xff00003c)
#define QACR_AREA_SHIFT 24
/* channel 2's registers on the system bus: the target address, the length in
 * bytes, and the start */
#define DMA_TARGET (*(volatile KMDWORD *)0xa05f6800)
#define DMA_LENGTH (*(volatile KMDWORD *)0xa05f6804)
#define DMA_START (*(volatile KMDWORD *)0xa05f6808)
/* the width of channel 2's path to texture memory */
#define TEXTURE_BUS (*(volatile KMDWORD *)0xa05f6884)
#define TEXTURE_BUS_64BIT 0
#define SOFT_RESET (*(volatile KMDWORD *)0xa05f8008)
#define RESET_TA 1

/* CHCR2's bits */
#define CHCR_ENABLE 0x1
#define CHCR_32_BYTES 0x40 /* 32 bytes a transfer unit */
#define CHCR_BURST 0x80 /* burst mode, where 0 is cycle steal */
#define CHCR_TO_BUS 0x200 /* memory to the bus device that requests it */
#define CHCR_SOURCE_UP 0x1000 /* the source address increments */
#define CONTROL_CYCLE_STEAL (CHCR_TO_BUS | CHCR_32_BYTES | CHCR_ENABLE)
#define CONTROL_BURST (CONTROL_CYCLE_STEAL | CHCR_BURST)

/* DMAOR's bits */
#define DMAOR_ENABLE 0x1
#define DMAOR_CH2_FIRST 0x200 /* channel 2 first, then 0, 1 and 3 */
#define DMAOR_ON_DEMAND 0x8000

/* the interrupt mask that holds off every interrupt below level 15 */
#define IMASK_BELOW_15 14

/* the tile accelerator's polygon input, as a DMA target */
#define TA_INPUT 0x10000000UL
/* the bytes in a transfer unit. DMATCR2 counts units */
#define TRANSFER_BYTES 32

/* SLOTS is a power of 2: a slot index wraps by SLOTS - 1 */
#define RINGS 2
#define SLOTS 8
/* a slot's state: its request's flags, with WAITING while the request waits
 * and RUNNING while it runs. a slot whose state holds neither is free */
#define WAITING 1
#define RUNNING 2
#define IN_USE (WAITING | RUNNING)
/* a request's flag: send in cycle steal mode rather than burst */
#define CYCLE_STEAL 4

struct ta_transfer {
    KMI_DMA_HANDLE handle;
    KMDWORD source;
    KMDWORD unk_08;
    KMDWORD target;
    KMDWORD unk_10;
    KMDWORD length;
    /* the interrupt at a transfer's end clears IN_USE here while
     * kmiWaitForFinishDMA polls it */
#ifdef NONMATCHING
    volatile KMDWORD state;
#else
    /* @hack type: state not volatile, where volatile would do */
    KMDWORD state;
#endif
    KMDWORD unk_1c;
};

static struct ta_transfer rings[RINGS][SLOTS];
static KMDWORD fill_slot[RINGS];
static KMI_DMA_HANDLE next_handle;
static KMDWORD issue_slot[RINGS];
/* the slot started last, which may since have finished. no queued transfer
 * starts while it is IN_USE */
static KMDWORD active_slot;
static KMDWORD active_ring;

/* NOLINTBEGIN(misc-use-internal-linkage) */
KMUINT32 kmiCh2DMAtoTA(const KMDWORD *source, KMDWORD length);
/* NOLINTEND(misc-use-internal-linkage) */

/* empty both rings, and set active_ring to ring and active_slot to its last
 * slot */
#define RESET_RINGS(ring)                                                      \
    {                                                                          \
        KMDWORD i;                                                             \
        fill_slot[0] = 0;                                                      \
        fill_slot[1] = 0;                                                      \
        issue_slot[0] = 0;                                                     \
        issue_slot[1] = 0;                                                     \
        next_handle = 0;                                                       \
        active_slot = SLOTS - 1;                                               \
        active_ring = (ring);                                                  \
        for (i = 0; i < SLOTS; i++) {                                          \
            rings[0][i].state = 0;                                             \
            rings[1][i].state = KMI_DMA_SECOND_RING;                           \
        }                                                                      \
    }

void kmiInitializeDMA(void)
{
    /* CHCR2's transfer end bit clears only on a write that follows a read */
    (void)CHCR2;
    SAR2 = 0;
    DAR2 = 0;
    DMATCR2 = 0;
    CHCR2 = (CONTROL_BURST | CHCR_SOURCE_UP) & ~CHCR_ENABLE;
    DMAOR = DMAOR_ON_DEMAND | DMAOR_CH2_FIRST | DMAOR_ENABLE;
    TEXTURE_BUS = TEXTURE_BUS_64BIT;
    QACR0 = TA_INPUT >> QACR_AREA_SHIFT;
    QACR1 = TA_INPUT >> QACR_AREA_SHIFT;
    RESET_RINGS(RINGS - 1);
}

KMINT32 kmiDMAHazardReset(void)
{
    KMDWORD control;

    DMA_START = 0;
    SOFT_RESET = RESET_TA;
    SOFT_RESET = 0;
    while (DMA_START != 0) {
    }
    control = CHCR2;
    CHCR2 = control & ~CHCR_ENABLE;
    SOFT_RESET = RESET_TA;
    SOFT_RESET = 0;
    /* @bug memory: rings[active_ring][active_slot] lies past both rings, and
     * its state is kmiSystemConfig.buffer, at 0x8c29ca3c. active_ring becomes
     * SLOTS - 1, 7, not RINGS - 1 (the store at 0x8c14b462). until a transfer
     * starts, kmiDMAtoTARequest tests that word's IN_USE bits (0x8c14b5f6), and
     * kmiExecuteDMAChain clears them (0x8c14b734) */
#ifdef NONMATCHING
    /* this side repairs the tag above: active_ring becomes RINGS - 1, as
     * kmiInitializeDMA sets it, so rings[active_ring][active_slot] is the
     * second ring's last slot */
    RESET_RINGS(RINGS - 1);
#else
    RESET_RINGS(SLOTS - 1);
#endif
    return 0;
}

/* start the request in ring's slot, whose length is count bytes */
#define START_TRANSFER(ring, slot, count)                                      \
    {                                                                          \
        KMDWORD control;                                                       \
        rings[ring][slot].state =                                              \
            (rings[ring][slot].state & ~IN_USE) | RUNNING;                     \
        SAR2 = rings[ring][slot].source;                                       \
        DAR2 = (rings[ring][slot].target - TA_INPUT) * 2;                      \
        DMATCR2 = (count) / TRANSFER_BYTES;                                    \
        control = CONTROL_CYCLE_STEAL;                                         \
        if (!(rings[ring][slot].state & CYCLE_STEAL)) {                        \
            control = CONTROL_BURST;                                           \
        }                                                                      \
        if (rings[ring][slot].state & KMI_DMA_SOURCE_UP) {                     \
            control |= CHCR_SOURCE_UP;                                         \
        } else {                                                               \
            /* @hack redundant: an empty else */                               \
        }                                                                      \
        CHCR2 = control;                                                       \
        DMA_TARGET = rings[ring][slot].target;                                 \
        DMA_LENGTH = rings[ring][slot].length;                                 \
        DMA_START = 1;                                                         \
        active_slot = slot;                                                    \
        active_ring = ring;                                                    \
        issue_slot[ring] = (issue_slot[ring] + 1) & (SLOTS - 1);               \
    }

KMUINT32 kmiDMAtoTARequest(KMDWORD target, const KMDWORD *source, KMUINT32 size,
                           KMDWORD flags, KMI_DMA_HANDLE **handle)
{
    KMINT32 mask;
    KMDWORD ring;
    KMDWORD slot;
    KMDWORD address;
    KMUINT32 result;

    mask = get_imask();
    set_imask(IMASK_BELOW_15);
    if (size == 0) {
        set_imask(mask);
        return KMI_DMA_NO_BYTES;
    }
    result = KMI_DMA_QUEUE_FULL;
    ring = (flags & KMI_DMA_SECOND_RING) / KMI_DMA_SECOND_RING;
    address = (KMDWORD)source;
    slot = fill_slot[ring];
    if (!(rings[ring][slot].state & IN_USE)) {
        if (address & (TRANSFER_BYTES - 1)) {
            address += TRANSFER_BYTES - 1;
            address &= ~(TRANSFER_BYTES - 1);
        }
        if (size & (TRANSFER_BYTES - 1)) {
            size += TRANSFER_BYTES - 1;
            size &= ~(TRANSFER_BYTES - 1);
        }
        rings[ring][slot].handle = next_handle;
        rings[ring][slot].source = address;
        rings[ring][slot].target = target;
        rings[ring][slot].length = size;
        rings[ring][slot].state = flags;
        rings[ring][slot].state |= WAITING;
        *handle = &rings[ring][slot].handle;
        result = KMI_DMA_QUEUED;
        fill_slot[ring] = (fill_slot[ring] + 1) & (SLOTS - 1);
        next_handle++;
    }
    if (result == KMI_DMA_QUEUE_FULL) {
        set_imask(mask);
        return result;
    }
    if (!(rings[active_ring][active_slot].state & IN_USE)) {
        if ((rings[ring][slot].state & IN_USE) == WAITING) {
            START_TRANSFER(ring, slot, size);
        }
    }
    set_imask(mask);
    return result;
}

KMINT32 kmiExecuteDMAChain(void)
{
    KMDWORD ring;
    KMDWORD slot;
    KMBOOLEAN started;

    /* @hack spelling: the state assigned its own AND with ~IN_USE, where &=
     * would do */
    rings[active_ring][active_slot].state =
        rings[active_ring][active_slot].state & ~IN_USE;
    started = 0;
    for (ring = 0; ring < RINGS; ring++) {
        slot = issue_slot[ring];
        if ((rings[ring][slot].state & IN_USE) == WAITING) {
            START_TRANSFER(ring, slot, rings[ring][slot].length);
            started = 1;
            break;
        }
    }
    return started;
}

KMINT32 kmiQueryFinishDMA(KMI_DMA_HANDLE handle)
{
    KMDWORD ring;
    KMDWORD slot;

    for (ring = 0; ring < RINGS; ring++) {
        for (slot = 0; slot < SLOTS; slot++) {
            if (rings[ring][slot].handle == handle) {
                if (rings[ring][slot].state & IN_USE) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

KMINT32 kmiWaitForFinishDMA(KMI_DMA_HANDLE handle)
{
    while (kmiQueryFinishDMA(handle) != 1) {
    }
    return 1;
}

KMUINT32 kmiCh2DMAtoTA(const KMDWORD *source, KMDWORD length)
{
    length = (length + (TRANSFER_BYTES - 1)) & ~(TRANSFER_BYTES - 1);
    SAR2 = (KMDWORD)source;
    DAR2 = 0;
    DMATCR2 = length / TRANSFER_BYTES;
    CHCR2 = CONTROL_BURST | CHCR_SOURCE_UP;
    if (length != 0) {
        DMA_TARGET = TA_INPUT;
        DMA_LENGTH = length;
        DMA_START = 1;
        return 1;
    }
    return 0;
}

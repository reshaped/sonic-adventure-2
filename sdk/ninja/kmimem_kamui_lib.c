/* Kamui's access to the video chip's registers and video memory, and the
 * installation of its interrupt handler */
#include <kamui.h>
#include <kamui_internal.h>
#include <machine.h>
#include <sg_xpt.h>

#define PVR_REGISTERS 0xa05f8000UL
/* video memory's uncached 32-bit and 64-bit areas */
#define VIEW_32BIT 0xa5000000UL
#define VIEW_64BIT 0xa4000000UL
/* video memory through the tile accelerator, as a DMA target */
#define FRAME_DMA_PATH 0x11000000UL
#define CACHE_LINE 32
#define DMA_FRAME_FLAGS (KMI_DMA_SECOND_RING | KMI_DMA_SOURCE_UP)

#define NORMAL_STATUS (*(volatile KMDWORD *)0xa05f6900)
#define ERROR_PENDING (*(volatile KMDWORD *)0xa05f6908)
/* the masks of the system bus's interrupts that raise IRQ6, and the events
 * kmiRegisterPhIntHandler enables in them. the handler goes in the word at
 * VBR + IRQ6_VECTOR */
#define IRQ6_NORMAL (*(volatile KMDWORD *)0xa05f6930)
#define IRQ6_ERROR (*(volatile KMDWORD *)0xa05f6938)
#define IRQ6_VECTOR 0x224
/* the interrupts' bits, in NORMAL_STATUS and IRQ6_NORMAL, and in ERROR_PENDING
 * and IRQ6_ERROR */
#define RENDER_END 4
#define VBLANK 8
#define HBLANK 0x20
#define YUV_END 0x40
#define OPAQUE_END 0x80
#define OPAQUE_MODIFIER_END 0x100
#define TRANS_END 0x200
#define TRANS_MODIFIER_END 0x400
#define DMA_END 0x80000
#define PUNCHTHROUGH_END 0x200000
#define STRIP_OVERRUN 2
#define ISP_OVERFLOW 4
#define OL_OVERFLOW 8
#define IRQ6_NORMAL_EVENTS                                                     \
    (RENDER_END | VBLANK | HBLANK | YUV_END | OPAQUE_END |                     \
     OPAQUE_MODIFIER_END | TRANS_END | TRANS_MODIFIER_END | DMA_END |          \
     PUNCHTHROUGH_END)
#define IRQ6_ERROR_EVENTS (STRIP_OVERRUN | ISP_OVERFLOW | OL_OVERFLOW)

struct register_write {
    KMDWORD reg;
    KMDWORD value;
};

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kmiPurgeMemory(KMDWORD address, KMDWORD length);
void kmiWriteRegisters(KMDWORD count, const struct register_write *writes);
KMINT32 kmiCopyTextureToTexture(void *destination, void *source, KMDWORD size);
/* NOLINTEND(misc-use-internal-linkage) */

void kmiModuleInitialize(void)
{
}

KMINT32 kmiWriteRegister(KMDWORD reg, KMDWORD value)
{
    *(volatile KMDWORD *)(PVR_REGISTERS + reg) = value;
    return TRUE;
}

/* write back the cache lines from address through address + length. the end
 * is inclusive. where address and length are both multiples of CACHE_LINE,
 * the line after the range is written back too */
void kmiPurgeMemory(KMDWORD address, KMDWORD length)
{
    KMDWORD line;
    KMDWORD end = address + length;

    for (line = address; line <= end; line += CACHE_LINE) {
        kmiPurgeCache((void *)line);
    }
}

KMDWORD kmiReadRegister(KMDWORD reg)
{
    return *(volatile KMDWORD *)(PVR_REGISTERS + reg);
}

KMINT32 kmiCopyToPVRMemory(void *destination, void *source, KMDWORD size)
{
    kmiMemCopy4((void *)(VIEW_32BIT + (KMDWORD)destination), source, size);
    return TRUE;
}

KMINT32 kmiCopyFromPVRMemory(void *destination, void *source, KMDWORD size)
{
    kmiMemCopy4(destination, (void *)(VIEW_32BIT + (KMDWORD)source), size);
    return TRUE;
}

KMINT32 kmiCopyToFrameBuffer(KMDWORD offset, KMDWORD *source, KMDWORD size)
{
    KMI_DMA_HANDLE *handle;
    KMDWORD bulk;
    KMDWORD target;
    KMINT32 mask = get_imask();

    /* DMA takes the whole cache lines, where offset and source start lines and
     * interrupts are unmasked. the rest goes a long at a time */
    if (!(offset & (CACHE_LINE - 1)) && !((KMDWORD)source & (CACHE_LINE - 1)) &&
        mask == 0) {
        kmiPurgeMemory((KMDWORD)source, size);
        bulk = size & ~(CACHE_LINE - 1);
        if (bulk != 0) {
            target = offset + FRAME_DMA_PATH;
            while (kmiDMAtoTARequest(target, source, bulk, DMA_FRAME_FLAGS,
                                     &handle) == KMI_DMA_QUEUE_FULL) {
            }
            kmiDeviceExtension.frame_dma = *handle;
        } else {
            kmiDeviceExtension.frame_dma = 0;
        }
        if (size & (CACHE_LINE - 1)) {
            offset += bulk;
            source = (KMDWORD *)((KMDWORD)source + bulk);
            kmiMemCopy4((void *)(VIEW_64BIT + offset), source,
                        size & (CACHE_LINE - 1));
        }
        if (kmiDeviceExtension.frame_dma_wait == 1) {
            kmiWaitForFinishDMA(kmiDeviceExtension.frame_dma);
        }
    } else {
        kmiMemCopy4((void *)(VIEW_64BIT + offset), source, size);
    }
    return TRUE;
}

KMINT32 kmiCopyFromFrameBuffer(KMDWORD *destination, KMDWORD offset,
                               KMDWORD size)
{
    kmiMemCopy4(destination, (void *)(VIEW_64BIT + offset), size);
    kmiPurgeMemory((KMDWORD)destination, size);
    return TRUE;
}

void kmiWriteRegisters(KMDWORD count, const struct register_write *writes)
{
    KMDWORD i;

    for (i = 0; i < count; i++) {
        kmiWriteRegister(writes[i].reg, writes[i].value);
    }
}

KMINT32 kmiCopyTextureToTexture(void *destination, void *source, KMDWORD size)
{
    return TRUE;
}

void kmiRegisterPhIntHandler(KMI_IRQ_HANDLER handler)
{
    KMINT32 mask = get_imask();

    set_imask(IMASK_ALL);
    *(KMI_IRQ_HANDLER *)((KMBYTE *)get_vbr() + IRQ6_VECTOR) = handler;
    IRQ6_NORMAL = 0;
    IRQ6_ERROR = 0;
    /* writing 1 to a status bit clears its interrupt */
    NORMAL_STATUS = 0xffffffff;
    ERROR_PENDING = 0xffffffff;
    IRQ6_NORMAL = IRQ6_NORMAL_EVENTS;
    IRQ6_ERROR = IRQ6_ERROR_EVENTS;
    set_imask(mask);
}

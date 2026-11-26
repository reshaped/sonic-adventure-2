/* the system's hardware: its start, in two steps, and its end */
#include <machine.h>
#include <maple_driver.h>
#include <sg_chain.h>
#include <sg_syg2.h>
#include <sg_syhw.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* an entry of sygHwRegs: a register's address, and the value to store
 * there */
struct register_setting {
    Uint32 address;
    Uint32 value;
};

/* the system bus's registers that sygHwRegs sets, through P2. CH2 is DMA
 * channel 2, and PVR the video chip */
#define SB(offset) (0xa05f0000 + (offset))
#define CH2_DMA_DESTINATION SB(0x6800)
#define CH2_DMA_LENGTH SB(0x6804)
#define CH2_DMA_START SB(0x6808)
#define SORT_DMA_TABLE SB(0x6810)
#define SORT_DMA_LINK_BASE SB(0x6814)
#define SORT_DMA_LINK_WIDTH SB(0x6818)
#define SORT_DMA_LINK_SHIFT SB(0x681c)
#define SORT_DMA_START SB(0x6820)
#define DMA_REQUEST_MASK SB(0x6840)
#define BUS_WAIT_COUNT SB(0x6844)
#define CH2_DMA_PRIORITY SB(0x6848)
#define CH2_DMA_MAX_BURST SB(0x684c)
#define TA_BUS_SELECT0 SB(0x6884)
#define TA_BUS_SELECT1 SB(0x6888)
#define ROOT_BUS_SPLIT SB(0x68a0)
#define ROOT_BUS_SPLIT_ON 0x80000000
#define SB_UNK_68A4 SB(0x68a4)
#define SB_UNK_68AC SB(0x68ac)

/* the interrupt registers. each is a run of words, one for the normal
 * sources and one for the external. INT_STATUS and the three levels have a
 * third, for the errors. INT_STATUS holds the sources pending, and a level's
 * register the sources enabled at that level */
#define INT_STATUS SB(0x6900)
#define INT_LEVEL2 SB(0x6910)
#define INT_LEVEL4 SB(0x6920)
#define INT_LEVEL6 SB(0x6930)
#define INT_PVR_DMA SB(0x6940)
#define INT_G2_DMA SB(0x6950)
#define INT_NORMAL 0x0
#define INT_EXTERNAL 0x4
#define INT_ERROR 0x8
/* INT_ALL is every source of a word. a source's 1, written to INT_STATUS,
 * clears it there */
#define INT_ALL 0xffffffff

#define MAPLE_DMA_TABLE SB(0x6c04)
#define MAPLE_DMA_TRIGGER SB(0x6c10)
#define MAPLE_DMA_ENABLE SB(0x6c14)
#define MAPLE_DMA_START SB(0x6c18)
#define MAPLE_SYSTEM SB(0x6c80)
#define MAPLE_DMA_PROTECT SB(0x6c8c)
#define MAPLE_MSB_SELECT SB(0x6ce8)

#define GD_DMA_START SB(0x7418)

/* the G2 bus's registers. each DMA channel has G2_DMA_SPAN bytes of them,
 * at the offsets below */
#define G2_DMA_SPAN 0x20
#define G2_DMA(channel, reg) SB(0x7800 + (G2_DMA_SPAN * (channel)) + (reg))
#define G2_DMA_DEVICE 0x00
#define G2_DMA_MEMORY 0x04
#define G2_DMA_LENGTH 0x08
#define G2_DMA_DIRECTION 0x0c
#define G2_DMA_MODE 0x10
#define G2_DMA_ENABLE 0x14
#define G2_DMA_START 0x18
#define G2_DMA_SUSPEND 0x1c
#define G2_DS_TIMEOUT SB(0x7890)
#define G2_DS_TIMEOUT_COUNT 27
#define G2_TR_TIMEOUT SB(0x7894)
#define G2_TR_TIMEOUT_COUNT 625
#define G2_MODEM_TIMEOUT SB(0x7898)
#define G2_MODEM_WAIT SB(0x789c)
#define G2_UNK_78A0(n) SB(0x78a0 + (4 * (n)))
#define G2_DMA_PROTECT SB(0x78bc)

#define PVR_DMA_DEVICE SB(0x7c00)
#define PVR_DMA_MEMORY SB(0x7c04)
#define PVR_DMA_LENGTH SB(0x7c08)
#define PVR_DMA_DIRECTION SB(0x7c0c)
#define PVR_DMA_TRIGGER SB(0x7c10)
#define PVR_DMA_ENABLE SB(0x7c14)
#define PVR_DMA_START SB(0x7c18)
#define PVR_DMA_PROTECT SB(0x7c80)

/* the values sygHwRegs leaves in the DMA registers. IDLE_MEMORY is the
 * main memory address it gives every table, link base and memory register */
#define IDLE_MEMORY 0x0cff0000
#define IDLE_CH2_DESTINATION 0x11ff0000
#define IDLE_G2_DEVICE 0x009f0000
#define IDLE_PVR_DEVICE 0x04ff0000
#define IDLE_LENGTH 32
#define IDLE_G2_MODE (SY_G2_DMA_MODE_TRANSFER_CTL | SY_G2_DMA_MODE_SUSPEND)

/* a DMA protection register's word holds the register's key from bit
 * PROTECT_KEY_SHIFT. the first megabyte a transfer may reach starts at bit
 * PROTECT_FIRST_SHIFT, and the last at bit 0. a megabyte's number is bits 20
 * to 26 of its addresses, and MEGABYTE_MAX is the highest */
#define PROTECT_KEY_SHIFT 16
#define PROTECT_FIRST_SHIFT 8
#define PROTECT(key, first, last)                                              \
    (((Uint32)(key) << PROTECT_KEY_SHIFT) | ((first) << PROTECT_FIRST_SHIFT) | \
     (last))
#define MAPLE_KEY 0x6155
#define G2_KEY 0x4659
#define PVR_KEY 0x6702
#define MEGABYTE_MAX 0x7f

/* CCR, the cache control register, and its bit that invalidates the
 * instruction cache */
#define CCR (*(volatile Uint32 *)0xff00001c)
#define CCR_ICI 0x800
/* the operand cache's address array: OC_ENTRIES entries, a line's size
 * apart */
#define OC_ADDRESS_ARRAY 0xf4000000
#define OC_ENTRIES 512
#define LINE_SHIFT 5
#define OC_ENTRY(i)                                                            \
    (*(volatile Uint32 *)(OC_ADDRESS_ARRAY + ((i) << LINE_SHIFT)))

/* the interrupt controller's control register and its priority registers A
 * to C, by their SH-4 names */
#define ICR (*(volatile Uint16 *)0xffd00000)
#define IPRA (*(volatile Uint16 *)0xffd00004)
#define IPRB (*(volatile Uint16 *)0xffd00008)
#define IPRC (*(volatile Uint16 *)0xffd0000c)

/* SCSPTR2, the serial port register of the SCIF. with RTSIO set, the RTS
 * pin outputs RTSDT */
#define SCSPTR2 (*(volatile Uint16 *)0xffe80020)
#define RTSIO 0x80
#define RTSDT 0x40

/* address in P2, which reaches memory uncached */
#define P2(address) ((Uint32)(address) | P2_BASE)

static void hw_disable_caches(void);
static void hw_load_registers(void);

/* the member exports syHwCacheFinish and the data below, though no header
 * of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void syHwCacheFinish(void);

const char sygHwBuild[] = "\nsyHw Ver 1.08 Build:Jul 06 2000 09:42:59\n";
const SY_CACHE_FORM sygCacheForm =
    SYD_CACHE_FORM_IC_ENABLE | SYD_CACHE_FORM_P1_CB | SYD_CACHE_FORM_OC_ENABLE;
/* the mode syHwInit gives syIntInit. syFbrPreInit stores
 * SYD_INT_MODE_INTSTACK here, though the image places it among its constants */
#ifdef NONMATCHING
/* GCC folds a const definition's value into its reads. its build drops the
 * const, so syHwInit reads the mode syFbrPreInit stored */
SY_INT_MODE sygIntMode = SYD_INT_MODE_LOCALSTACK;
#else
const SY_INT_MODE sygIntMode = SYD_INT_MODE_LOCALSTACK;
#endif
/* the stack syHwInit gives syIntInit, null until syFbrPreInit sets it */
void *sygIntStack = NULL;
/* the system bus's starting values, which hw_load_registers stores in
 * order */
const struct register_setting sygHwRegs[] = {
    {CH2_DMA_DESTINATION, IDLE_CH2_DESTINATION},
    {CH2_DMA_LENGTH, IDLE_LENGTH},
    {SORT_DMA_TABLE, IDLE_MEMORY},
    {SORT_DMA_LINK_BASE, IDLE_MEMORY},
    {SORT_DMA_LINK_WIDTH, 0},
    {SORT_DMA_LINK_SHIFT, 0},
    {DMA_REQUEST_MASK, 0},
    {BUS_WAIT_COUNT, 0},
    {CH2_DMA_PRIORITY, 0},
    {CH2_DMA_MAX_BURST, 0},
    {TA_BUS_SELECT0, 0},
    {TA_BUS_SELECT1, 0},
    {ROOT_BUS_SPLIT, ROOT_BUS_SPLIT_ON},
    {SB_UNK_68A4, 0},
    {SB_UNK_68AC, 0},
    {INT_LEVEL2 + INT_NORMAL, 0},
    {INT_LEVEL2 + INT_EXTERNAL, 0},
    {INT_LEVEL2 + INT_ERROR, 0},
    {INT_LEVEL4 + INT_NORMAL, 0},
    {INT_LEVEL4 + INT_EXTERNAL, 0},
    {INT_LEVEL4 + INT_ERROR, 0},
    {INT_LEVEL6 + INT_NORMAL, 0},
    {INT_LEVEL6 + INT_EXTERNAL, 0},
    {INT_LEVEL6 + INT_ERROR, 0},
    {INT_PVR_DMA + INT_NORMAL, 0},
    {INT_PVR_DMA + INT_EXTERNAL, 0},
    {INT_G2_DMA + INT_NORMAL, 0},
    {INT_G2_DMA + INT_EXTERNAL, 0},
    {MAPLE_DMA_TABLE, IDLE_MEMORY},
    {MAPLE_DMA_TRIGGER, 0},
    {MAPLE_DMA_ENABLE, 0},
    {MAPLE_SYSTEM, MPDRV_TIMEOUT(MPDRV_START_TIMEOUT)},
    {MAPLE_DMA_PROTECT, PROTECT(MAPLE_KEY, MEGABYTE_MAX, 0)},
    {MAPLE_MSB_SELECT, 1},
    {G2_DMA(0, G2_DMA_DEVICE), IDLE_G2_DEVICE},
    {G2_DMA(0, G2_DMA_MEMORY), IDLE_MEMORY},
    {G2_DMA(0, G2_DMA_LENGTH), IDLE_LENGTH},
    {G2_DMA(0, G2_DMA_DIRECTION), 0},
    {G2_DMA(0, G2_DMA_MODE), IDLE_G2_MODE},
    {G2_DMA(0, G2_DMA_ENABLE), 0},
    {G2_DMA(0, G2_DMA_SUSPEND), 0},
    {G2_DMA(1, G2_DMA_DEVICE), IDLE_G2_DEVICE},
    {G2_DMA(1, G2_DMA_MEMORY), IDLE_MEMORY},
    {G2_DMA(1, G2_DMA_LENGTH), IDLE_LENGTH},
    {G2_DMA(1, G2_DMA_DIRECTION), 0},
    {G2_DMA(1, G2_DMA_MODE), IDLE_G2_MODE},
    {G2_DMA(1, G2_DMA_ENABLE), 0},
    {G2_DMA(1, G2_DMA_SUSPEND), 0},
    {G2_DMA(2, G2_DMA_DEVICE), IDLE_G2_DEVICE},
    {G2_DMA(2, G2_DMA_MEMORY), IDLE_MEMORY},
    {G2_DMA(2, G2_DMA_LENGTH), IDLE_LENGTH},
    {G2_DMA(2, G2_DMA_DIRECTION), 0},
    {G2_DMA(2, G2_DMA_MODE), IDLE_G2_MODE},
    {G2_DMA(2, G2_DMA_ENABLE), 0},
    {G2_DMA(2, G2_DMA_SUSPEND), 0},
    {G2_DS_TIMEOUT, G2_DS_TIMEOUT_COUNT},
    {G2_TR_TIMEOUT, G2_TR_TIMEOUT_COUNT},
    {G2_MODEM_TIMEOUT, 0},
    {G2_MODEM_WAIT, 1},
    {G2_UNK_78A0(0), 0},
    {G2_UNK_78A0(1), 0},
    {G2_UNK_78A0(2), 0},
    {G2_UNK_78A0(3), 0},
    {G2_UNK_78A0(4), 0},
    {G2_UNK_78A0(5), 0},
    {G2_UNK_78A0(6), 0},
    {G2_DMA_PROTECT, PROTECT(G2_KEY, MEGABYTE_MAX, 0)},
    {PVR_DMA_DEVICE, IDLE_PVR_DEVICE},
    {PVR_DMA_MEMORY, IDLE_MEMORY},
    {PVR_DMA_LENGTH, IDLE_LENGTH},
    {PVR_DMA_DIRECTION, 0},
    {PVR_DMA_TRIGGER, 0},
    {PVR_DMA_ENABLE, 0},
    {PVR_DMA_PROTECT, PROTECT(PVR_KEY, MEGABYTE_MAX, 0)},
    {CH2_DMA_START, 0},
    {SORT_DMA_START, 0},
    {MAPLE_DMA_START, 0},
    {GD_DMA_START, 0},
    {G2_DMA(0, G2_DMA_START), 0},
    {G2_DMA(1, G2_DMA_START), 0},
    {G2_DMA(2, G2_DMA_START), 0},
    {PVR_DMA_START, 0},
    {INT_STATUS + INT_NORMAL, INT_ALL},
    {INT_STATUS + INT_ERROR, INT_ALL},
    /* an address of 0 ends the values. hw_load_registers never reads the
     * six entries after it */
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};
/* NOLINTEND(misc-use-internal-linkage) */

void syHwInit(void)
{
    hw_load_registers();
    syIntInit(sygIntMode, sygIntStack);
    sySqInit();
    syCacheInit(sygCacheForm);
    syTmrInit();
}

void syHwInit2(void)
{
    syChainInit();
    syG2SysInit(NULL);
    syExtChkInit();
}

void syHwFinish(void)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    syExtChkExit();
    sySqFinish();
    syHwCacheFinish();
    set_imask(mask);
}

/* turn both caches off. hw_disable_caches has to run uncached, so this
 * calls it at its P2 address */
void syHwCacheFinish(void)
{
    /* @hack spelling: func and uncached, locals for hw_disable_caches and
     * for its P2 address, where ((void (*)(void))P2(hw_disable_caches))()
     * would do */
    void (*func)(void) = hw_disable_caches;
    void (*uncached)(void) = (void (*)(void))P2(func);

    uncached();
}

static void hw_disable_caches(void)
{
    Sint32 i;

    CCR = 0;
    for (i = 0; i < OC_ENTRIES; i++) {
        OC_ENTRY(i) = 0;
    }
    CCR = CCR_ICI;
    CCR = 0;
}

static void hw_load_registers(void)
{
    const struct register_setting *setting;

    ICR = 0;
    IPRA = 0;
    IPRB = 0;
    IPRC = 0;
    SCSPTR2 = RTSIO | RTSDT;
    /* @hack spelling: the loop's test as an if that breaks, first in its
     * body, where the for's condition would do */
    for (setting = sygHwRegs;; setting++) {
        if (setting->address == 0) {
            break;
        }
        *(volatile Uint32 *)setting->address = setting->value;
    }
}

/* the fiber table and the current fiber */
#include <machine.h>
#include <sg_syfbr.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* sygFbrCurrent while no fiber runs */
#define NO_FIBER ((SYS_FBR_INFO *)-1)
/* the sp syFbrConvertThreadToFiber gives the calling thread's record, which
 * a null sp would leave free. the thread's first switch away saves its stack
 * pointer there */
#define UNSAVED_SP ((void *)-1)
/* the fewest records syFbrInit takes */
#define FEWEST_RECORDS 2

/* syFbrPreInit stores to sygIntMode, so this declaration leaves off the
 * const that syhw.c's definition carries for SHC */
extern SY_INT_MODE sygIntMode;

static Sint32 syfbr_find_free_slot(void);
static void lock_fibers(Sint32 *mask);
static void unlock_fibers(const Sint32 *mask);

/* the member exports these names, the statics aside, though no header of
 * the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const sygFbrBuild = "\nsyFbr Ver 1.13 Build:Jun 11 1999 21:40:19\n";
static Sint32 sygFbrMaxFiber = 0;
SYFBR sygFbrCurrent = NO_FIBER;
void *sygFbrISRstack = NULL;
static SYFBR sygFbrTblPtr;
/* syFbrFinish, as the SDK spells it */
Sint32 syFbrFishin(void);
Sint32 syFbrDeleteFiber(SYFBR fiber);
SYFBR syFbrGetCurrentFiber(void);
void *syFbrGetFiberData(void);
Sint32 syFbrSetIntStack(void *sp);
SYFBR syFbrGetTblPtr(int index);
Sint32 syFbrGetMaxFiber(void);
void *syFbrGetSp(SYFBR fiber);
void *syFbrGetData(SYFBR fiber);
void syFbrSetSp(SYFBR fiber, void *sp);
void syFbrSetData(SYFBR fiber, void *data);
void *syFbrGet_fib_sp(int index);
void syFbrDumpTable(void);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 syFbrInit(void *work, Sint32 n)
{
    if (!work || n < FEWEST_RECORDS) {
        return -1;
    }
    sygFbrTblPtr = work;
    sygFbrMaxFiber = n;
    memset(syFbrGetTblPtr(0), 0, syFbrGetMaxFiber() * sizeof(SYS_FBR_INFO));
    sygFbrCurrent = NO_FIBER;
    return 0;
}

Sint32 syFbrPreInit(Sint32 *stack, int nword)
{
    void *top;

    if (!stack) {
        return -1;
    }
    sygIntMode = SYD_INT_MODE_INTSTACK;
    sygIntStack = top = stack + nword;
    syFbrSetIntStack(top);
    return 0;
}

Sint32 syFbrFishin(void)
{
    return 0;
}

SYFBR syFbrCreateFiber(void *stack, Uint32 size, SyFbrYfunc func, void *arg)
{
    SYFBR fiber = NULL;
    /* @hack type: word, one volatile local for size and then the stack
     * pointer, where size and a plain local would do */
    volatile Sint32 word = size;
    Sint32 mask;
    /* @hack type: data, a volatile copy of arg, where arg would do */
    void *volatile data = arg;
    Sint32 index;

    if (!word || !stack || !func) {
        return NULL;
    }
    word = (Sint32)syFbrCreateFiberMain(stack, word, func, data);
    /* @hack redundant: a store of 0 to mask that never runs */
    if (0) {
        mask = 0;
    }
    lock_fibers(&mask);
    index = syfbr_find_free_slot();
    if (index >= 0) {
        fiber = syFbrGetTblPtr(index);
        syFbrSetSp(fiber, (void *)word);
        syFbrSetData(fiber, data);
    }
    unlock_fibers(&mask);
    return fiber;
}

SYFBR syFbrConvertThreadToFiber(void *data)
{
    SYFBR fiber = NULL;
    Sint32 mask;
    Sint32 index;

    if (sygFbrCurrent != NO_FIBER) {
        return NULL;
    }
    lock_fibers(&mask);
    index = syfbr_find_free_slot();
    if (index >= 0) {
        fiber = syFbrGetTblPtr(index);
        syFbrSetSp(fiber, UNSAVED_SP);
        syFbrSetData(fiber, data);
        sygFbrCurrent = fiber;
    }
    unlock_fibers(&mask);
    return fiber;
}

static Sint32 syfbr_find_free_slot(void)
{
    Sint32 count = syFbrGetMaxFiber();
    Sint32 index;

    for (index = 0; index < count; index++) {
        if (!syFbrGet_fib_sp(index)) {
            return index;
        }
    }
    return -1;
}

Sint32 syFbrDeleteFiber(SYFBR fiber)
{
    Sint32 mask;

    if (fiber && fiber != sygFbrCurrent) {
        lock_fibers(&mask);
        syFbrSetSp(fiber, NULL);
        syFbrSetData(fiber, NULL);
        unlock_fibers(&mask);
        return 0;
    }
    return -1;
}

SYFBR syFbrGetCurrentFiber(void)
{
    return sygFbrCurrent;
}

void *syFbrGetFiberData(void)
{
    return syFbrGetData(sygFbrCurrent);
}

Sint32 syFbrSetIntStack(void *sp)
{
    if (!sp) {
        return -1;
    }
    sygFbrISRstack = sp;
    return 0;
}

SYFBR syFbrGetTblPtr(int index)
{
    return &sygFbrTblPtr[index];
}

Sint32 syFbrGetMaxFiber(void)
{
    return sygFbrMaxFiber;
}

void *syFbrGetSp(SYFBR fiber)
{
    return fiber->sp;
}

void *syFbrGetData(SYFBR fiber)
{
    return fiber->data;
}

void syFbrSetSp(SYFBR fiber, void *sp)
{
    fiber->sp = sp;
}

void syFbrSetData(SYFBR fiber, void *data)
{
    fiber->data = data;
}

void *syFbrGet_fib_sp(int index)
{
    return syFbrGetTblPtr(index)->sp;
}

static void lock_fibers(Sint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}

static void unlock_fibers(const Sint32 *mask)
{
    set_imask(*mask);
}

void syFbrDumpTable(void)
{
}

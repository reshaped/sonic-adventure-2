/* syExtIntMasterHandler serves every device's interrupt from one handler on
 * EXTINT_CHAIN_EVENT's chain */
#include <machine.h>
#include <sg_chain.h>
#include <sg_exint.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

#define EXTINT_FIRST_DEVICE 1
#define EXTINT_DEVICE_STRIDE 0x400U
#define EXTINT_REGISTERS(n) (0xa1000000U + ((n) * EXTINT_DEVICE_STRIDE))
/* the offsets of a device's id word and its interrupt word. a read of the
 * interrupt word gives the interrupt's status, and a write sets its mask */
#define EXTINT_ID 0x10
#define EXTINT_INTERRUPT 0x14
/* the status bit of a device whose interrupt is raised */
#define EXTINT_RAISED 1
/* the masks that enable and disable a device's interrupt */
#define EXTINT_ENABLED 1
#define EXTINT_DISABLED 0
/* the chain event of the devices' interrupts, the external event after
 * SYD_CHAIN_EVENT_IML4_MODEM */
#define EXTINT_CHAIN_EVENT 0x0b60
#define EXTINT_CHAIN_PRIORITY 16

struct extint_handler {
    void (*isr)(void *);
    void *arg;
};

const Uint32 sygExtIntVer = 2;
char *sygExtIntBuild = "\nsyExtInt Ver 2.00 Build:Dec 24 1999 14:16:00\n";

static Uint32 sygExtIntHandlerCount;
static struct extint_handler sygExtIntIsrTbl[SYD_EXTINT_EDEV_MAX_DEVICE];
static SYCHAIN sygExtIntChainHandle;

static void syExtIntMasterHandler(void *arg);

Uint32 syExtIntGetDevId(SYEXTINT dev)
{
    Uint32 id;

    syG2Read((void *)(EXTINT_REGISTERS(dev) + EXTINT_ID), &id,
             SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
             SYE_G2_ADR_MODE_FIX);
    return id;
}

Uint32 syExtIntGetDevIst(SYEXTINT dev)
{
    Uint32 status;

    syG2Read((void *)(EXTINT_REGISTERS(dev) + EXTINT_INTERRUPT), &status,
             SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
             SYE_G2_ADR_MODE_FIX);
    return status;
}

void syExtIntSetDevImr(SYEXTINT dev, Uint32 imr)
{
    syG2Write(&imr, (void *)(EXTINT_REGISTERS(dev) + EXTINT_INTERRUPT),
              SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
              SYE_G2_ADR_MODE_FIX);
}

void syExtIntInit(void)
{
    Uint16 mask;
    Sint32 dev;

    mask = get_imask();
    set_imask(IMASK_ALL);
    for (dev = EXTINT_FIRST_DEVICE; dev < SYD_EXTINT_EDEV_MAX_DEVICE; dev++) {
        sygExtIntIsrTbl[dev].isr = NULL;
        sygExtIntIsrTbl[dev].arg = NULL;
    }
    set_imask(mask);
}

SYCHAIN syExIntAddHandler(SY_EXTINT_EVENT event, void (*usrIsr)(void *),
                          Uint32 priority, void *arg)
{
    return syExtIntAddHandler(event, usrIsr, priority, arg);
}

SYCHAIN syExtIntAddHandler(SY_EXTINT_EVENT event, void (*usrIsr)(void *),
                           Uint32 priority, void *arg)
{
    Uint16 mask;

#ifdef NONMATCHING
    /* this side refuses an event outside the devices' table, and a missing
     * handler */
    if (event < EXTINT_FIRST_DEVICE || event >= SYD_EXTINT_EDEV_MAX_DEVICE ||
        !usrIsr) {
        return 0;
    }
#endif
    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* this side replaces the handler of an event that has one, and counts the
     * event once. the matching side counts it again */
    if (sygExtIntIsrTbl[event].isr) {
        sygExtIntIsrTbl[event].isr = usrIsr;
        sygExtIntIsrTbl[event].arg = arg;
        set_imask(mask);
        return event;
    }
#endif
    sygExtIntIsrTbl[event].isr = usrIsr;
    sygExtIntIsrTbl[event].arg = arg;
    if (sygExtIntHandlerCount == 0) {
        sygExtIntChainHandle =
            syChainAddHandler(EXTINT_CHAIN_EVENT, syExtIntMasterHandler,
                              EXTINT_CHAIN_PRIORITY, NULL);
        /* @bug logic: enables the device's interrupt with
         * syExtIntMasterHandler on no chain, when syChainAddHandler's pool is
         * full. the call at 0x0cadfed2 then returns 0, which the store at
         * 0x0cadfed8 keeps as the handle. the count still rises, so later
         * calls chain nothing while it stays above 0 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it clears the event's handler
         * and fails with 0 */
        if (!sygExtIntChainHandle) {
            sygExtIntIsrTbl[event].isr = NULL;
            sygExtIntIsrTbl[event].arg = NULL;
            set_imask(mask);
            return 0;
        }
#endif
    }
    syExtIntSetDevImr(event, EXTINT_ENABLED);
    sygExtIntHandlerCount++;
    set_imask(mask);
    return event;
}

static void syExtIntMasterHandler(void *arg)
{
    /* @hack redundant: registers, a volatile local that nothing reads,
     * stepped through each device's registers beside dev */
    volatile Uint32 registers;
    Uint32 dev;

    for (registers = EXTINT_REGISTERS(EXTINT_FIRST_DEVICE),
        dev = EXTINT_FIRST_DEVICE;
         dev < SYD_EXTINT_EDEV_MAX_DEVICE;
         dev++, registers += EXTINT_DEVICE_STRIDE) {
        if (sygExtIntIsrTbl[dev].isr) {
            if (syExtIntGetDevIst(dev) & EXTINT_RAISED) {
                sygExtIntIsrTbl[dev].isr(sygExtIntIsrTbl[dev].arg);
            }
        }
    }
}

void syExIntDeleteHandler(SYEXTINT extint)
{
    syExtIntDeleteHandler(extint);
}

void syExtIntDeleteHandler(SYEXTINT extint)
{
    Uint16 mask;

#ifdef NONMATCHING
    /* this side refuses an event outside the devices' table */
    if (extint < EXTINT_FIRST_DEVICE || extint >= SYD_EXTINT_EDEV_MAX_DEVICE) {
        return;
    }
#endif
    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* this side does nothing for an event with no handler. the matching side
     * counts it out again */
    if (!sygExtIntIsrTbl[extint].isr) {
        set_imask(mask);
        return;
    }
#endif
    sygExtIntIsrTbl[extint].isr = NULL;
    sygExtIntIsrTbl[extint].arg = NULL;
    syExtIntSetDevImr(extint, EXTINT_DISABLED);
    sygExtIntHandlerCount--;
    if (sygExtIntHandlerCount == 0) {
        syChainDeleteHandler(sygExtIntChainHandle);
    }
    set_imask(mask);
}

void syExtIntFinish(void)
{
    Uint16 mask;
    Sint32 dev;

    mask = get_imask();
    set_imask(IMASK_ALL);
    for (dev = EXTINT_FIRST_DEVICE; dev < SYD_EXTINT_EDEV_MAX_DEVICE; dev++) {
        sygExtIntIsrTbl[dev].isr = NULL;
        sygExtIntIsrTbl[dev].arg = NULL;
    }
    sygExtIntHandlerCount = 0;
    syChainDeleteHandler(sygExtIntChainHandle);
    set_imask(mask);
}

/* Drive the registered watchdog callback using in-memory vectors. */
#include <assert.h>

#include <sg_chain.h>
#include <shinobi.h>

#include "engine/dc/crash_report.h"

static Uint32 vectors[0x20];
static unsigned int interrupt_mask = 4;
static void (*vblank)(void *);
static SYCHAIN next_handle;
static int registrations;
static uint32_t scanout;
static uint32_t last_report;
static unsigned int reports;

#undef USER_VECT
#define USER_VECT(code) vectors[(code) >> 5]
#undef get_imask
#define get_imask() (interrupt_mask)
#undef set_imask
#define set_imask(mask) (interrupt_mask = (mask))

#include "engine/dc/crash_watch.c"

void crash_entry(void)
{
}

uint32_t crash_scanout(void)
{
    return scanout;
}

void crash_capture_hang(uint32_t elapsed)
{
    last_report = elapsed;
    reports++;
}

SYCHAIN syChainAddHandler(SY_CHAIN_EVENT event, void (*handler)(void *),
                          Uint32 priority, void *arg)
{
    assert(interrupt_mask == IMASK_ALL && !vblank);
    assert(event == SYD_CHAIN_EVENT_IML6_VBLANK && priority == 0x7fffffff);
    assert(arg == NULL);
    registrations++;
    if (next_handle) {
        vblank = handler;
    }
    return next_handle;
}

void syChainDeleteHandler(SYCHAIN handle)
{
    assert(interrupt_mask == IMASK_ALL && handle && handle == next_handle);
    assert(vblank);
    vblank = NULL;
}

int main(void)
{
    static const int events[] = {CRASH_ADDRESS_READ, CRASH_ADDRESS_WRITE,
                                 CRASH_TRAPA, CRASH_ILLEGAL,
                                 CRASH_ILLEGAL_SLOT};
    unsigned int i;
    unsigned int tick;

    for (i = 0; i < sizeof events / sizeof events[0]; i++) {
        USER_VECT(events[i]) = i + 1;
    }
    crash_remove();
    crash_install(); /* A full chain pool still permits exception capture. */
    assert(!vblank && interrupt_mask == 4);
    for (i = 0; i < sizeof events / sizeof events[0]; i++) {
        assert(USER_VECT(events[i]) == (Uint32)crash_entry);
    }
    next_handle = 7;
    crash_install();
    crash_install();
    assert(vblank && registrations == 2 && interrupt_mask == 4);

    for (tick = 1; tick <= 1860; tick++) {
        vblank(NULL);
        assert(reports ==
               (unsigned)((tick >= 1800) + (tick >= 1830) + (tick >= 1860)));
    }
    assert(last_report == 1860);
    scanout++;
    vblank(NULL);
    for (tick = 1; tick < 1800; tick++) {
        vblank(NULL);
    }
    assert(reports == 3);
    vblank(NULL);
    assert(reports == 4 && last_report == 1800);

    crash_remove();
    crash_remove();
    assert(vblank && interrupt_mask == 4);
    USER_VECT(CRASH_TRAPA) = 99;
    crash_remove();
    crash_remove();
    assert(!vblank && interrupt_mask == 4);
    for (i = 0; i < sizeof events / sizeof events[0]; i++) {
        assert(USER_VECT(events[i]) == (events[i] == CRASH_TRAPA ? 99 : i + 1));
    }

    next_handle = 0;
    crash_install();
    crash_remove();
    assert(!vblank && registrations == 3 && interrupt_mask == 4);
    assert(USER_VECT(CRASH_TRAPA) == 99);
    return 0;
}

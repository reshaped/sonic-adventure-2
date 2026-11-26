/* the crash screen's installation: its exception vectors, and its watch for a
 * hang on the vblank chain. each image that links the screen has its own copy,
 * with its own counts and saved vectors */
#include "crash.h"

#include <machine.h>
#include <sg_chain.h>
#include <shinobi.h>

#include "crash_report.h"

/* a hang is a scanout base unchanged for HANG_VBLANKS vblanks. the watch
 * reports it then, and every HANG_AGAIN vblanks after until the base changes */
#define HANG_VBLANKS 1800
#define HANG_AGAIN 30
/* the highest priority syChainAddHandler takes. a chain calls its handlers by
 * ascending priority, so hang_vblank runs last */
#define CHAIN_PRIORITY 0x7fffffff

/* the codes of the exceptions the screen takes, each below VECT_TABLE.
 * USER_VECT finds the word of such a code's handler only from a signed code */
static const Sint32 codes[] = {CRASH_ADDRESS_READ, CRASH_ADDRESS_WRITE,
                               CRASH_TRAPA, CRASH_ILLEGAL, CRASH_ILLEGAL_SLOT};
#define CODES (sizeof codes / sizeof codes[0])
/* each code's handler before crash_install replaced it */
static Uint32 replaced[CODES];
/* the crash_install calls that crash_remove has yet to undo */
static int installs;
/* the number syChainAddHandler gave hang_vblank, or 0 for none */
static SYCHAIN chain;

/* the buffer FB_R_SOF1 last named */
static Uint32 shown;
/* the vblanks since shown changed */
static Uint32 idle;
/* the vblanks until the hang is reported again, or 0 before its first report */
static Uint32 again;

static void hang_vblank(void *arg)
{
    Uint32 buffer = crash_scanout();

    (void)arg;
    if (buffer != shown) {
        shown = buffer;
        idle = 0;
        again = 0;
        return;
    }
    if (++idle < HANG_VBLANKS) {
        return;
    }
    if (again != 0 && --again != 0) {
        return;
    }
    again = HANG_AGAIN;
    crash_capture_hang(idle);
}

void crash_install(void)
{
    Uint16 mask = get_imask();
    Uint32 i;

    set_imask(IMASK_ALL);
    if (installs++ == 0) {
        for (i = 0; i < CODES; i++) {
            replaced[i] = USER_VECT(codes[i]);
            USER_VECT(codes[i]) = (Uint32)crash_entry;
        }
        shown = crash_scanout();
        idle = 0;
        again = 0;
    }
    /* with the handler pool full, syChainAddHandler returns 0, and the next
     * install tries again. the vectors work without the watch */
    if (chain == 0) {
        chain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK, hang_vblank,
                                  CHAIN_PRIORITY, NULL);
    }
    set_imask(mask);
}

void crash_remove(void)
{
    Uint16 mask = get_imask();
    Uint32 i;

    set_imask(IMASK_ALL);
    if (installs != 0 && --installs == 0) {
        if (chain != 0) {
            syChainDeleteHandler(chain);
            chain = 0;
        }
        for (i = 0; i < CODES; i++) {
            if (USER_VECT(codes[i]) == (Uint32)crash_entry) {
                USER_VECT(codes[i]) = replaced[i];
            }
        }
    }
    set_imask(mask);
}

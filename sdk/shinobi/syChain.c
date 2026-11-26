/* interrupt chains: each event's handlers, called in ascending order of
 * priority */
#include <kamui.h>
#include <machine.h>
#include <sg_chain.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the pool's handlers. syChainAddHandler hands out handlers from
 * FIRST_HANDLER up, and returns 0 for a full pool */
#define POOL_HANDLERS 256
#define FIRST_HANDLER 1
/* bit 31 of a handler's priority marks it in use */
#define HANDLER_USED 0x80000000
/* KEY gives an event's key, its index in sygChainKeyTbl, and EVENT a key's
 * first event. the 16 codes of a key share one chain */
#define KEY(event) ((event) >> 4)
#define EVENT(key) ((key) << 4)
/* the end of the event codes, which run from 0, and the number of keys
 * that cover them */
#define EVENT_END 0xc00
#define KEYS KEY(EVENT_END)

/* the CPU's interrupt codes of levels 6, 4 and 2, at which the system bus's
 * interrupts arrive */
#define EVENT_IML6 0x320
#define EVENT_IML4 0x360
#define EVENT_IML2 0x3a0

/* the system bus's interrupt registers, at their P2 addresses. a status
 * word has a bit set for each source pending. a level 4 word has one set
 * for each source enabled at level 4. each register has a word for the
 * normal sources and one for the external */
#define INT_STATUS_NORMAL (*(Uint32 *)0xa05f6900)
#define INT_STATUS_EXTERNAL (*(Uint32 *)0xa05f6904)
#define INT_LEVEL4_NORMAL (*(Uint32 *)0xa05f6920)
#define INT_LEVEL4_EXTERNAL (*(Uint32 *)0xa05f6924)
/* a bus event's kind, normal or external, and its source's number within
 * the kind */
#define KIND(event) ((event) >> 8)
#define KIND_NORMAL 0xa
#define KIND_EXTERNAL 0xb
#define SOURCE_SHIFT 5
#define SOURCE(event) ((Uint8)(event) >> SOURCE_SHIFT)
/* a normal source's bit in the bus's registers is its number plus
 * NORMAL_FIRST_BIT. an external source's bit is its number */
#define NORMAL_FIRST_BIT 12
/* syChainClx1Handler serves the first NORMAL_SOURCES normal sources and the
 * first EXTERNAL_SOURCES external ones */
#define NORMAL_SOURCES 7
#define EXTERNAL_SOURCES 4
/* the interrupt masks of the blank callbacks' chains and of the level 4
 * handler */
#define IMASK_BLANK 6
#define IMASK_BUS 4

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const sygChainBuild =
    "\nsyChain Ver 1.50 Build:May 31 1999 16:35:21\n";
const Uint8 sygChainVer[4] = {1, 5, 0, 0};
/* the pool, and each key's chain: its first handler, or null */
SYS_CHAIN_ISR sygChainIsrPool[POOL_HANDLERS];
SYS_CHAIN_ISR *sygChainKeyTbl[KEYS];

void syChainMasterHandler(SY_CHAIN_EVENT event);
void syChainMasterHandlerPVR_V(void *arg);
void syChainMasterHandlerPVR_H(void *arg);
void syChainClx1Handler(SY_CHAIN_EVENT event);
/* NOLINTEND(misc-use-internal-linkage) */

void syChainInit(void)
{
    Uint16 mask = get_imask();
    Uint32 i;

    set_imask(IMASK_ALL);
    for (i = 0; i < POOL_HANDLERS; i++) {
        sygChainIsrPool[i].priority = 0;
    }
    for (i = 0; i < KEYS; i++) {
        sygChainKeyTbl[i] = NULL;
    }
    set_imask(mask);
}

SYCHAIN syChainAddHandler(SY_CHAIN_EVENT event, void (*usrIsr)(void *),
                          Uint32 priority, void *arg)
{
    Uint16 mask = get_imask();
    SYS_CHAIN_ISR *isr;
    SYS_CHAIN_ISR *before;
    Uint32 i;
    SYCHAIN chain;

    set_imask(IMASK_ALL);
    /* isr becomes the first handler at priority or above, or null where none
     * is. before becomes the handler ahead of isr, but stays isr where isr
     * heads the chain. so both are null only for an empty chain */
    isr = sygChainKeyTbl[KEY(event)];
    before = isr;
    while (isr != NULL && isr->priority < (priority | HANDLER_USED)) {
        before = isr;
        isr = isr->next;
    }
    for (i = FIRST_HANDLER; i < POOL_HANDLERS; i++) {
        if (!(sygChainIsrPool[i].priority & HANDLER_USED)) {
            break;
        }
    }
    if (i < POOL_HANDLERS) {
        /* @hack spelling: the stores of usrIsr, arg and priority in each
         * arm, where one set after the arms would do */
        if (isr == NULL && before == NULL) {
            if (event < SYD_CHAIN_EVENT_IML6_VBLANK) {
                USER_VECT(event) = (Uint32)syChainMasterHandler;
            } else if (event == SYD_CHAIN_EVENT_IML6_VBLANK) {
                kmSetVSyncCallback(syChainMasterHandlerPVR_V, NULL);
            } else if (event == SYD_CHAIN_EVENT_IML6_HBLANK) {
                kmSetHSyncCallback(syChainMasterHandlerPVR_H, NULL);
            } else {
                USER_VECT(EVENT_IML4) = (Uint32)syChainClx1Handler;
            }
            sygChainKeyTbl[KEY(event)] = &sygChainIsrPool[i];
            sygChainIsrPool[i].next = NULL;
            sygChainIsrPool[i].usrIsr = usrIsr;
            sygChainIsrPool[i].arg = arg;
            sygChainIsrPool[i].priority = priority | HANDLER_USED;
            if (KIND(event) == KIND_NORMAL) {
                INT_LEVEL4_NORMAL |= 1 << (SOURCE(event) + NORMAL_FIRST_BIT);
            } else if (KIND(event) == KIND_EXTERNAL) {
                INT_LEVEL4_EXTERNAL |= 1 << SOURCE(event);
            }
        } else if (isr == sygChainKeyTbl[KEY(event)]) {
            sygChainIsrPool[i].next = isr;
            sygChainKeyTbl[KEY(event)] = &sygChainIsrPool[i];
            sygChainIsrPool[i].usrIsr = usrIsr;
            sygChainIsrPool[i].arg = arg;
            sygChainIsrPool[i].priority = priority | HANDLER_USED;
        } else {
            before->next = &sygChainIsrPool[i];
            sygChainIsrPool[i].next = isr;
            sygChainIsrPool[i].usrIsr = usrIsr;
            sygChainIsrPool[i].arg = arg;
            sygChainIsrPool[i].priority = priority | HANDLER_USED;
        }
        chain = i;
    } else {
        chain = 0;
    }
    set_imask(mask);
    return chain;
}

/* the handler of each CPU event with a chain, which calls the chain's
 * handlers in order */
void syChainMasterHandler(SY_CHAIN_EVENT event)
{
    SYS_CHAIN_ISR *isr = sygChainKeyTbl[KEY(event)];

    while (isr != NULL) {
        isr->usrIsr(isr->arg);
        isr = isr->next;
    }
}

/* Kamui's vertical sync callback */
void syChainMasterHandlerPVR_V(void *arg)
{
    SYS_CHAIN_ISR *isr;

    set_imask(IMASK_BLANK);
    isr = sygChainKeyTbl[KEY(SYD_CHAIN_EVENT_IML6_VBLANK)];
    while (isr != NULL) {
        isr->usrIsr(isr->arg);
        isr = isr->next;
    }
}

/* Kamui's horizontal sync callback */
void syChainMasterHandlerPVR_H(void *arg)
{
    SYS_CHAIN_ISR *isr;

    set_imask(IMASK_BLANK);
    isr = sygChainKeyTbl[KEY(SYD_CHAIN_EVENT_IML6_HBLANK)];
    while (isr != NULL) {
        isr->usrIsr(isr->arg);
        isr = isr->next;
    }
}

/* the level 4 interrupt's handler. it runs the chain of each source it
 * serves that is pending and enabled at level 4. it runs no external
 * source's chain while a normal source is pending and enabled */
void syChainClx1Handler(SY_CHAIN_EVENT event)
{
    Uint32 pending;
    int source;

    /* @hack redundant: the empty cases for levels 6 and 2 */
    switch (KEY(event)) {
    case KEY(EVENT_IML6):
        break;
    case KEY(EVENT_IML4):
        set_imask(IMASK_BUS);
        pending = (INT_STATUS_NORMAL & INT_LEVEL4_NORMAL) >> NORMAL_FIRST_BIT;
        /* @hack spelling: each loop's test of source's bit against the bit
         * past the last source, where source < NORMAL_SOURCES and source <
         * EXTERNAL_SOURCES would do */
        if (pending != 0) {
            for (source = 0; (1 << source) != (1 << NORMAL_SOURCES); source++) {
                if (pending & (1 << source)) {
                    syChainMasterHandler(SYD_CHAIN_EVENT_IML4_NRM +
                                         (source << SOURCE_SHIFT));
                }
            }
        } else {
            pending = INT_STATUS_EXTERNAL & INT_LEVEL4_EXTERNAL;
            for (source = 0; (1 << source) != (1 << EXTERNAL_SOURCES);
                 source++) {
                if (pending & (1 << source)) {
                    syChainMasterHandler(SYD_CHAIN_EVENT_IML4_EXT +
                                         (source << SOURCE_SHIFT));
                }
            }
        }
        break;
    case KEY(EVENT_IML2):
        break;
    }
}

void syChainDeleteHandler(SYCHAIN chain)
{
    Uint16 mask = get_imask();
    Uint32 i;

    set_imask(IMASK_ALL);
    for (i = FIRST_HANDLER; i < POOL_HANDLERS; i++) {
        if (sygChainIsrPool[i].next == &sygChainIsrPool[chain] &&
            (sygChainIsrPool[i].priority & HANDLER_USED)) {
            break;
        }
    }
    if (i != POOL_HANDLERS) {
        sygChainIsrPool[i].next = sygChainIsrPool[chain].next;
        sygChainIsrPool[chain].priority = 0;
    } else {
        /* @bug logic: leaves a handler that heads key 0's chain, of events 0
         * to 15, on the chain and in use. the search of the heads starts at
         * key 1 (MOV R5,R14 at 0x8c110018, with 1 in R5). no image adds a
         * handler for an event below 0x900 */
        for (i = 1; i < KEYS; i++) {
            if (sygChainKeyTbl[i] == &sygChainIsrPool[chain]) {
                sygChainKeyTbl[i] = sygChainIsrPool[chain].next;
                sygChainIsrPool[chain].priority = 0;
                if (sygChainKeyTbl[i] == NULL) {
                    if (i == KEY(SYD_CHAIN_EVENT_IML6_VBLANK)) {
                        kmSetVSyncCallback(NULL, NULL);
                    } else if (i == KEY(SYD_CHAIN_EVENT_IML6_HBLANK)) {
                        kmSetHSyncCallback(NULL, NULL);
                    } else {
                        SY_CHAIN_EVENT event = EVENT(i);

                        if (KIND(event) == KIND_NORMAL) {
                            INT_LEVEL4_NORMAL &=
                                ~(1 << (SOURCE(event) + NORMAL_FIRST_BIT));
                        } else if (KIND(event) == KIND_EXTERNAL) {
                            INT_LEVEL4_EXTERNAL &= ~(1 << SOURCE(event));
                        }
                        /* @bug arithmetic: for an event below 0x200, stores
                         * syIntBrkRteRte H'20000000 bytes past the vector
                         * syChainAddHandler set, as USER_VECT gives for an
                         * unsigned code (SHLR2 R13 at 0x8c1100a6 and SHLR
                         * R13 at 0x8c1100a8, against SHAR R3 at 0x8c10fd30
                         * to 0x8c10fd34). no image adds a handler for an
                         * event below 0x900 */
                        USER_VECT(EVENT(i)) = (Uint32)syIntBrkRteRte;
                    }
                }
                break;
            }
        }
        /* @hack redundant: an empty test of i */
        if (i == KEYS) {
        }
    }
    set_imask(mask);
}

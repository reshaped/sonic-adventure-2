/* fiber servers. each fiber runs on a stack of its own. setjmp and longjmp
 * switch between a fiber and its server */
#include <machine.h>
#include <setjmp.h>
#include <sg_xpt.h>
#include <string.h>

#ifdef NONMATCHING
/* this side's checks of a fiber's stack take uintptr_t and UINTPTR_MAX from
 * here */
#include <stdint.h>
#endif

#include "vms.h"

#define SERVERS 2
/* a server's slots, the length of its fibers[] */
#define FIBERS 10

/* a fiber's states. a run that reaches a killed fiber marks it ended. a run
 * that reaches an ended fiber frees its slot */
#define FIBER_FREE 0
#define FIBER_READY 1 /* due once the server's tick reaches wake_tick */
#define FIBER_KILLING 2
#define FIBER_ENDED 3

/* the wake_tick of a fiber that sleeps till buFbrWakeupFiber. the fiber
 * still wakes once its server's tick reaches it, 2^31 - 1 runs after
 * buFbrCreateServer */
#define TICK_NEVER 0x7fffffff

/* a saved context. setjmp fills its first 38 words, as it fills a jmp_buf.
 * word CONTEXT_SP holds the stack pointer, and word CONTEXT_PC the address
 * longjmp resumes at */
#define CONTEXT_SP 0
#define CONTEXT_PC 2
typedef int CONTEXT[64];

typedef struct {
    Uint32 *stack; /* the lowest address of the fiber's stack */
    Sint32 stack_words;
    Sint32 unk_08;
    Sint32 state;
    /* the level buFbrKillFiberLevel compares. buFbrCreateFiber sets it to 0,
     * and nothing sets another */
    Sint32 level;
    Uint32 wake_tick;
    CONTEXT context;
    void (*on_free)(void); /* called as the slot is freed, where not null */
    void *unk_11c;
    void *unk_120;
} FIBER;

struct bu_fbr_server {
    Sint32 in_use;
    /* the run's context, which a sleeping fiber's longjmp returns to */
    CONTEXT scheduler;
    /* the context buFbrCreateServer saves. buFbrCreateFiber copies it into
     * each new fiber, and then overwrites the whole copy. no fiber starts
     * from it */
    CONTEXT creation;
    Uint32 tick; /* the runs of the server since buFbrCreateServer */
    /* Slots each run scans. The original caller must keep room at most
     * FIBERS. The GCC constructor checks that bound. */
    Sint32 room;
    Sint32 live_count;
    FIBER fibers[FIBERS];
    FIBER *running;
};

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy every server in use */
void buFbrExit(void);
/* kill the server's fiber index if it is ready, and return 0. a killed fiber
 * never runs again. returns -1, and changes nothing, for a fiber in any other
 * state */
Sint32 buFbrKillFiber(BU_FBR_SERVER *server, Sint32 index);
/* kill each ready fiber of the server whose level is at most level */
void buFbrKillFiberLevel(BU_FBR_SERVER *server, Sint32 level);
/* make the server's fiber index due now, unless its slot is free */
void buFbrWakeupFiber(BU_FBR_SERVER *server, Sint32 index);
/* sleep the running fiber till its server has run ticks more times. a
 * negative ticks sleeps it till buFbrWakeupFiber. a ticks of 0 runs it again
 * in the same run. the call returns as the fiber runs again */
void buFbrSleepFiber(Sint32 ticks);
/* NOLINTEND(misc-use-internal-linkage) */

static BU_FBR_SERVER servers[SERVERS];
/* the server whose run is under way */
static BU_FBR_SERVER *current;
#ifdef NONMATCHING
#else
/* current as buFbrExecFiberServer found it, restored as the run ends */
static BU_FBR_SERVER *previous;
#endif
static Sint32 servers_created;

void buFbrInit(void)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    int unused;

    memset(servers, 0, sizeof servers);
    current = NULL;
#ifdef NONMATCHING
#else
    previous = NULL;
#endif
    servers_created = 0;
}

BU_FBR_SERVER *buFbrCreateServer(Sint32 fibers)
{
    int i;
    int mask;
    BU_FBR_SERVER *server;

#ifdef NONMATCHING
    /* this side refuses a count outside 1 to FIBERS, the room a server holds */
    if (fibers <= 0 || fibers > FIBERS) {
        return NULL;
    }
#endif
    mask = get_imask();
    set_imask(IMASK_ALL);
    for (server = servers, i = 0; i < SERVERS; i++, server++) {
        if (!server->in_use) {
            break;
        }
    }
    if (i == SERVERS) {
        server = NULL;
    } else {
        server->room = fibers;
        server->live_count = 0;
        server->tick = 0;
        memset(server->fibers, 0, sizeof server->fibers);
#ifdef NONMATCHING
#else
        memset(server->creation, 0, sizeof server->creation);
        (void)setjmp(server->creation);
#endif
        server->in_use = 1;
        servers_created++;
    }
    set_imask(mask);
    return server;
}

Sint32 buFbrCreateFiber(BU_FBR_SERVER *server, void (*entry)(void),
                        Uint32 *stack, Uint32 size)
{
    int i;
    FIBER *fiber;
#ifdef NONMATCHING
    /* this side returns -1 for a null or unused server, a null entry, and a
     * stack that is null, unaligned, not whole words long, under two words or
     * wrapping past the end of memory. the matching side goes on with each. it
     * repairs the bug tagged below: live_count counts the live fibers, and the
     * scan for a free slot stops at room. the asserts hold CONTEXT to the
     * jmp_buf that setjmp.src fills and longjmp.src reads */
    _Static_assert(sizeof(jmp_buf) == 152 && sizeof(jmp_buf) <= sizeof(CONTEXT),
                   "SH-4 register save must fit the fiber context");
    _Static_assert(CONTEXT_SP * sizeof(int) == 0 &&
                       CONTEXT_PC * sizeof(int) == 8,
                   "SH-4 stack and resume addresses must match longjmp.src");

    if (server == NULL || !server->in_use || entry == NULL || stack == NULL ||
        (uintptr_t)stack % sizeof(*stack) != 0 || size % sizeof(*stack) != 0 ||
        size < 2 * sizeof(*stack) ||
        (uintptr_t)stack > UINTPTR_MAX - (size - sizeof(*stack)) ||
        server->live_count >= server->room) {
        return -1;
    }
    for (i = 0; i < server->room; i++) {
        if (server->fibers[i].state == FIBER_FREE) {
            break;
        }
    }
    if (i == server->room) {
        return -1;
    }
    fiber = &server->fibers[i];
#else
    jmp_buf env;

    memset(env, 0, sizeof env);
    /* @bug logic: the test for a full server never holds where room is 1 or
     * more (CMP/EQ at 0x8c0cbe3a). live_count starts at 0, nothing raises it,
     * and free_fiber lowers it (ADD #-1 at 0x8c0cbf88). a fiber created on a
     * full server takes a slot past room, which no run reaches. with all
     * FIBERS slots taken, the scan for a free slot runs past fibers[].
     * vmsfs_c_init creates no more fibers than its server has room for */
    if (server->live_count == server->room) {
        return -1;
    }
    for (i = 0, fiber = server->fibers; fiber->state; fiber++, i++) {
    }
#endif
    fiber->unk_08 = 0;
    size /= sizeof(*stack);
    fiber->state = FIBER_READY;
    fiber->wake_tick = server->tick;
    fiber->stack = stack;
    fiber->stack_words = size;
    fiber->on_free = NULL;
    fiber->level = 0;
    fiber->unk_11c = NULL;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: setjmp fills the context in
     * place, so no copy reads past env. it clears the unused words too, since a
     * recycled slot keeps its old contents. live_count counts the fiber */
    memset(fiber->context, 0, sizeof fiber->context);
    (void)setjmp(fiber->context);
    server->live_count++;
#else
    memcpy(fiber->context, server->creation, sizeof fiber->context);
    (void)setjmp(env);
    /* @bug memory: the copy reads past env, a jmp_buf of 152 bytes, since it
     * takes a whole context's 256 (MOV.W at 0x8c0cbf12). its last 104 bytes
     * are 28 of this frame past env and 76 of the caller's (JSR at
     * 0x8c0cbf1e) */
    memcpy(fiber->context, env, sizeof fiber->context);
#endif
    fiber->context[CONTEXT_SP] = (int)&stack[size - 1];
    fiber->context[CONTEXT_PC] = (int)entry;
    return i;
}

static void free_fiber(BU_FBR_SERVER *server, FIBER *fiber)
{
    server->live_count--;
    fiber->state = FIBER_FREE;
    if (fiber->on_free != NULL) {
        fiber->on_free();
    }
}

void buFbrExit(void)
{
    BU_FBR_SERVER *server;
    int i;

    for (server = servers, i = 0; i < SERVERS; i++, server++) {
        if (server->in_use) {
            buFbrDestroyServer(server);
        }
    }
}

void buFbrDestroyServer(BU_FBR_SERVER *server)
{
    FIBER *fiber;
    int i;

#ifdef NONMATCHING
    /* this side refuses a missing server, one not in use, and one whose run is
     * in progress */
    if (server == NULL || !server->in_use || server->running != NULL) {
        return;
    }
#endif

    for (i = 0, fiber = server->fibers; i < server->room; i++, fiber++) {
        if (fiber->state) {
            free_fiber(server, fiber);
        }
    }
    memset(server, 0, sizeof *server);
}

void buFbrEndFiber(void)
{
    current->running->state = FIBER_ENDED;
    buFbrSleepFiber(0);
}

Sint32 buFbrKillFiber(BU_FBR_SERVER *server, Sint32 index)
{
    FIBER *fiber;

#ifdef NONMATCHING
    /* this side refuses a missing server, one not in use, and an index past its
     * fibers */
    if (server == NULL || !server->in_use || index < 0 ||
        index >= server->room) {
        return -1;
    }
#endif
    fiber = &server->fibers[index];
    if (fiber->state == FIBER_READY) {
        buFbrWakeupFiber(server, index);
        fiber->state = FIBER_KILLING;
        return 0;
    }
    return -1;
}

void buFbrKillFiberLevel(BU_FBR_SERVER *server, Sint32 level)
{
    int i;
    FIBER *fiber;

    for (i = 0, fiber = server->fibers; i < server->room; i++, fiber++) {
        if (!fiber->state) {
            continue;
        }
        if (fiber->level <= level) {
            buFbrKillFiber(server, i);
        }
    }
}

void buFbrWakeupFiber(BU_FBR_SERVER *server, Sint32 index)
{
    FIBER *fiber;

#ifdef NONMATCHING
    /* this side refuses a missing server, one not in use, and an index past its
     * fibers */
    if (server == NULL || !server->in_use || index < 0 ||
        index >= server->room) {
        return;
    }
#endif
    fiber = &server->fibers[index];
    if (fiber->state) {
        fiber->wake_tick = server->tick;
    }
}

void buFbrSleepFiber(Sint32 ticks)
{
    current->running->wake_tick =
        ticks >= 0 ? current->tick + ticks : TICK_NEVER;
    if (setjmp(current->running->context) == 0) {
        longjmp(current->scheduler, 1);
    }
}

void buFbrSleepVFiber(void)
{
    buFbrSleepFiber(1);
}

void buFbrExecFiberServer(BU_FBR_SERVER *server)
{
    int i;
    int fpscr;
    /* @hack redundant: unused, a local that only sizes the frame */
    int unused;
    FIBER *fiber;

#ifdef NONMATCHING
    BU_FBR_SERVER *caller = current;

    /* Another server can run on this fiber's stack. Reentering an active
     * server would overwrite the return context of its suspended run. */
    if (server == NULL || !server->in_use || server->running != NULL) {
        return;
    }
#else
    previous = current;
#endif
    current = server;
    fiber = server->fibers;
    i = 0;
    fpscr = get_fpscr();
    set_fpscr(FPSCR_DN);
#ifdef NONMATCHING
#else
    /* @hack implicit: i and fiber without volatile, though C leaves them
     * indeterminate after a sleeping fiber's longjmp to this setjmp. SHC keeps
     * both in the frame, so the scan goes on from the fiber that slept */
    (void)setjmp(server->scheduler);
#endif
    for (; i < server->room; i++, fiber++) {
        server->running = fiber;
#ifdef NONMATCHING
        /* Save after selecting this slot. A yield returns here before the
         * loop changes either cursor. Reinspect its state and wake time:
         * sleep(0) runs again, end frees it, and a later wake advances. */
        if (fiber->state == FIBER_READY && fiber->wake_tick <= server->tick) {
            (void)setjmp(server->scheduler);
        }
#endif
        switch (fiber->state) {
        case FIBER_FREE:
            break;
        case FIBER_ENDED:
            free_fiber(server, fiber);
            break;
        case FIBER_READY:
            if (fiber->wake_tick > server->tick) {
                break;
            }
            longjmp(fiber->context, 1);
            break;
        case FIBER_KILLING:
            /* a run frees an ended fiber without resuming it, so nothing
             * takes the address stored here */
            fiber->state = FIBER_ENDED;
            fiber->context[CONTEXT_PC] = (int)buFbrEndFiber;
            break;
        /* @hack redundant: an empty default */
        default:
            break;
        }
    }
    server->tick++;
#ifdef NONMATCHING
    /* this side marks no fiber running, so the server can run again, and
     * restores the server this run interrupted. previous holds one server for
     * every run, which a nested run overwrites */
    server->running = NULL;
    current = caller;
#else
    current = previous;
#endif
    set_fpscr(fpscr);
}

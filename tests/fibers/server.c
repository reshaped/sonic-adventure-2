/* Exercise the real private slots as well as the exported scheduling calls.
 * Only the interrupt mask is modeled. */
#include <machine.h>

static int interrupt_mask;
#undef get_imask
#undef set_imask
#define get_imask() interrupt_mask
#define set_imask(mask) (interrupt_mask = (mask))

#ifndef __sh__
/* The host checks the precise save area and admission without switching its
 * own stack. Its register writer fills exactly the target's 38 words. */
#undef get_fpscr
#undef set_fpscr
#define get_fpscr() 0
#define set_fpscr(value) ((void)(value))
#define setjmp fixture_save
#define longjmp fixture_restore
#endif

#include "../../sdk/backup/rfiber.c"

static Uint32 stacks[FIBERS][256];

#ifndef __sh__
int fixture_save(jmp_buf env)
{
    unsigned i;

    for (i = 0; i < sizeof(jmp_buf) / sizeof(int); i++) {
        env[i] = 100 + i;
    }
    return 0;
}

void fixture_restore(jmp_buf env, int value)
{
    (void)env;
    (void)value;
    __builtin_trap();
}
#endif

static void empty_entry(void)
{
    buFbrEndFiber();
}

static int admission(void)
{
    BU_FBR_SERVER *server;
    BU_FBR_SERVER *other;
    unsigned slot;
    unsigned word;
    unsigned room;

    for (room = VMS_DRIVES; room <= FIBERS; room += FIBERS - VMS_DRIVES) {
        buFbrInit();
        if (buFbrCreateServer(-1) || buFbrCreateServer(0) ||
            buFbrCreateServer(FIBERS + 1)) {
            return 1;
        }
        server = buFbrCreateServer(room);
        other = buFbrCreateServer(1);
        if (!server || !other || buFbrCreateServer(1)) {
            return 2;
        }
        if (buFbrCreateFiber(server, NULL, stacks[0], sizeof stacks[0]) != -1 ||
            buFbrCreateFiber(server, empty_entry, NULL, sizeof stacks[0]) !=
                -1 ||
            buFbrCreateFiber(server, empty_entry, stacks[0], 0) != -1 ||
            buFbrCreateFiber(server, empty_entry, stacks[0], sizeof(Uint32)) !=
                -1 ||
            buFbrCreateFiber(server, empty_entry, stacks[0],
                             sizeof stacks[0] - 1) != -1 ||
            buFbrCreateFiber(server, empty_entry,
                             (void *)((char *)stacks[0] + 1),
                             sizeof stacks[0]) != -1) {
            return 3;
        }
        for (slot = 0; slot < room; slot++) {
            FIBER *fiber = &server->fibers[slot];

            memset(fiber->context, 0xfa, sizeof fiber->context);
            if (buFbrCreateFiber(server, empty_entry, stacks[slot],
                                 sizeof stacks[slot]) != (int)slot ||
                fiber->context[CONTEXT_SP] !=
                    (int)(unsigned long)&stacks[slot][255] ||
                fiber->context[CONTEXT_PC] != (int)(unsigned long)empty_entry) {
                return 4;
            }
            for (word = sizeof(jmp_buf) / sizeof(int);
                 word < sizeof(CONTEXT) / sizeof(int); word++) {
                if (fiber->context[word] != 0) {
                    return 5;
                }
            }
#ifndef __sh__
            for (word = 0; word < sizeof(jmp_buf) / sizeof(int); word++) {
                if (word != CONTEXT_SP && word != CONTEXT_PC &&
                    fiber->context[word] != (int)(100 + word)) {
                    return 6;
                }
            }
#endif
        }
        if (buFbrCreateFiber(server, empty_entry, stacks[0],
                             sizeof stacks[0]) != -1 ||
            buFbrKillFiber(server, -1) != -1 ||
            buFbrKillFiber(server, room) != -1) {
            return 7;
        }
        buFbrWakeupFiber(server, room);
        if (server->live_count != (Sint32)room || other->live_count != 0) {
            return 8;
        }
        if (buFbrKillFiber(server, 3) != 0) {
            return 9;
        }
        /* Keep all the other fibers asleep while the killed slot is reaped. */
        for (slot = 0; slot < room; slot++) {
            server->fibers[slot].wake_tick = 3;
        }
        buFbrExecFiberServer(server);
        buFbrExecFiberServer(server);
        if (server->live_count != (Sint32)room - 1 ||
            buFbrCreateFiber(server, empty_entry, stacks[3],
                             sizeof stacks[3]) != 3 ||
            server->live_count != (Sint32)room || interrupt_mask != 0) {
            return 10;
        }
    }
    return 0;
}

#ifdef __sh__
static BU_FBR_SERVER *outer_server;
static BU_FBR_SERVER *inner_server;
static char trace[16];
static unsigned events;
static int failure;

/* start.s captures the untouched entry stack and checks R8/FR12 over a yield.
 */
unsigned long entry_sp;
void register_fiber(void);
void registers_restored(void);

static void event(char tag)
{
    if (events >= sizeof trace) {
        failure = 17;
        return;
    }
    trace[events++] = tag;
}

static void nested_fiber(void)
{
    event('n');
    if (get_fpscr() != FPSCR_DN) {
        failure = 18;
    }
    buFbrExecFiberServer(outer_server);
    buFbrDestroyServer(outer_server);
    buFbrEndFiber();
}

static void first_fiber(void)
{
    event('a');
    if (get_fpscr() != FPSCR_DN) {
        failure = 18;
    }
    set_fpscr(FPSCR_DN | 1);
    buFbrExecFiberServer(inner_server);
    if (current != outer_server || get_fpscr() != (FPSCR_DN | 1)) {
        failure = 11;
    }
    buFbrSleepFiber(0);
    if (get_fpscr() != (FPSCR_DN | 1)) {
        failure = 18;
    }
    event('b');
    buFbrSleepFiber(-1);
    event('d');
    buFbrEndFiber();
}

void registers_restored(void)
{
    event('r');
    buFbrWakeupFiber(outer_server, 0);
    buFbrEndFiber();
}

static int schedules(void)
{
    unsigned tick;
    const int caller_fpscr = FPSCR_DN | 1;
    static const char expected[] = {'a', 'n', 'b', 'r', 'd'};

    buFbrInit();
    set_fpscr(FPSCR_DN);
    outer_server = buFbrCreateServer(2);
    inner_server = buFbrCreateServer(1);
    if (!outer_server || !inner_server ||
        buFbrCreateFiber(outer_server, first_fiber, stacks[0],
                         sizeof stacks[0]) != 0 ||
        buFbrCreateFiber(outer_server, register_fiber, stacks[1],
                         sizeof stacks[1]) != 1 ||
        buFbrCreateFiber(inner_server, nested_fiber, stacks[2],
                         sizeof stacks[2]) != 0) {
        return 12;
    }
    set_fpscr(caller_fpscr);
    for (tick = 0; tick < 3; tick++) {
        buFbrExecFiberServer(outer_server);
        if (current || outer_server->running || get_fpscr() != caller_fpscr ||
            events != 3 + tick) {
            return 13;
        }
    }
    if (failure) {
        return failure;
    }
    if (entry_sp != (unsigned long)&stacks[1][255] || outer_server->tick != 3 ||
        inner_server->tick != 1 || outer_server->live_count != 0 ||
        inner_server->live_count != 0) {
        return 14;
    }
    for (tick = 0; tick < sizeof expected; tick++) {
        if (trace[tick] != expected[tick]) {
            return 15;
        }
    }
    return 0;
}

/* Freestanding target guest. */
void *memset(void *address, int byte, size_t size)
{
    unsigned char *out = address;

    while (size--) {
        *out++ = (unsigned char)byte;
    }
    return address;
}
#endif

int main(void)
{
    int result = admission();

#ifdef __sh__
    if (result == 0) {
        result = schedules();
    }
#endif
    return result;
}

#include <access/cardprobe.h>
#include <access/cmcontex.h>
#include <access/vsyncpro.h>
#include <machine.h>
#include <mdmAPI.h>
#include <ninja.h>
#include <sg_chain.h>
#include <sg_xpt.h>
#include <string.h>
#include "daemons.h"
#include "ppp_modem.h"

#pragma section ATA
static char build_version[45];
/* the library's version and build time, exported through ntgSupBuild, which
 * no header declares */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
char *ntgSupBuild = build_version;
static char build_version[45] =
    "\nntDmcn Ver 1.08 Build:Oct 26 2000 20:41:07\n";
#pragma section

#define DAEMON_STACK_WORDS 4096
#define STACK_FILL 0xaa
#define DAEMON_TURNS 128
#define VBLANK_MASK 6
/* network_vblank's priority on the vertical blank's interrupt chain */
#define VBLANK_PRIORITY 76

#ifdef NONMATCHING
/* this side marks the three volatile, the plain form the hack tag below gives:
 * network_vblank changes them under the foreground */
#define VBLANK_SHARED volatile
#else
/* @hack type: no volatile on ticks, ppp_enabled and tcp_enabled, which
 * network_vblank shares with the foreground, where volatile would do */
#define VBLANK_SHARED
#endif
static VBLANK_SHARED Uint32 ticks = 0;
static VBLANK_SHARED Bool ppp_enabled = FALSE;
static VBLANK_SHARED Bool tcp_enabled = FALSE;
static void (*wait_frame)(void) = NULL;

/* eight zero bytes that end this file's initialized data */
#ifdef DREAMCAST
static Uint8 reserved_data[8] = {0};
#endif

static SYCHAIN vblank_chain;
static CM_CONTEXT interrupt_context, ppp_context, tcp_context;
static Uint32 ppp_stack[DAEMON_STACK_WORDS], tcp_stack[DAEMON_STACK_WORDS];
/* the turns each daemon takes in a vertical blank. no header declares
 * vsVSyncLoopCount, which the library exports */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
int vsVSyncLoopCount;

static void ppp_daemon(void)
{
    for (;;) {
        if (AT_CardType == AT_CARD_TYPE_PPP) {
            if (ppp_enabled) {
                PP_TimerTask();
            }
        }
        cmSwitchContext(&interrupt_context, &ppp_context);
    }
}
static void tcp_daemon(void)
{
    for (;;) {
        if (tcp_enabled) {
            TcpIntervalTimerTaskFunc();
        }
        cmSwitchContext(&interrupt_context, &tcp_context);
    }
}
static void network_vblank(void *argument)
{
    int i;
    MDM *port = ppp_modem_port;
    ticks++;
    if (AT_CardType == AT_CARD_TYPE_PPP) {
        mdmServicePort(*port);
    }
    for (i = 0; i < vsVSyncLoopCount; i++) {
        cmSwitchContext(&tcp_context, &interrupt_context);
        cmSwitchContext(&ppp_context, &interrupt_context);
    }
    if (AT_CardType == AT_CARD_TYPE_PPP) {
        mdmServicePort(*port);
    }
}
void vsInitVSyncProc(void (*callback)(void))
{
    wait_frame = callback;
    vsVSyncLoopCount = DAEMON_TURNS;
    memset(ppp_stack, STACK_FILL, sizeof(ppp_stack));
    cmInitContext(&ppp_context, ppp_daemon, &ppp_stack[DAEMON_STACK_WORDS - 1]);
    memset(tcp_stack, STACK_FILL, sizeof(tcp_stack));
    cmInitContext(&tcp_context, tcp_daemon, &tcp_stack[DAEMON_STACK_WORDS - 1]);
    vblank_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                                     network_vblank, VBLANK_PRIORITY, NULL);
}
void vsFinishVSyncProc(void)
{
    tcp_enabled = FALSE;
    ppp_enabled = FALSE;
    syChainDeleteHandler(vblank_chain);
}
void vsWaitVSync(int count)
{
    int i;
    for (i = 0; i < count; i++) {
        if (wait_frame) {
            wait_frame();
        } else {
            njWaitVSync();
        }
    }
}
void vsStartPppDaemon(void)
{
    ppp_enabled = TRUE;
}
void vsStopPppDaemon(void)
{
    ppp_enabled = FALSE;
}
void vsStartTcpipDaemon(void)
{
    tcp_enabled = TRUE;
}
void vsStopTcpipDaemon(void)
{
    tcp_enabled = FALSE;
}
int vsEnterCriticalSection(void)
{
    int level = get_imask();
    if (level < VBLANK_MASK) {
        set_imask(VBLANK_MASK);
    }
    return level;
}
void vsLeaveCriticalSection(int savedLevel)
{
    set_imask(savedLevel);
}
void vsPausePppDaemon(void)
{
    int level = get_imask();
    if (!level) {
        vsWaitVSync(1);
    } else {
        cmSwitchContext(&interrupt_context, &ppp_context);
    }
}
void vsPauseTcpipDaemon(void)
{
    int level = get_imask();
    if (!level) {
        vsWaitVSync(1);
    } else {
        cmSwitchContext(&interrupt_context, &tcp_context);
    }
}
int vsGetTickCount(void)
{
    return (int)ticks;
}

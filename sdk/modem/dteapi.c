/* the modem's interface to the host. the host opens and closes the modem
 * through the calls dteGetApi gives it. the modem moves the host's bytes
 * through calls the host gives Open. ModemMainCall does the modem's work at
 * each vertical blank */
#include <sg_xpt.h>
#include <string.h>

#include "bl.h"
#include "cmi.h"
#include "cmm.h"
#include "dspdb.h"
#include "dte_vxd.h"
#include "dteapi.h"
#include "osif.h"
#include "pacing.h"
#include "roc_k56.h"
#include "sysmain.h"
#include "target.h"
#include "tci_modm.h"

/* ModemMainCall counts each vertical blank as VBLANK_MS milliseconds. it
 * runs the 10 ms timers once for each TIMER_MS of them */
#define VBLANK_MS 17
#define TIMER_MS 10

/* the bytes of the blacklist, as BL_GetBlackListBuffer gives it */
#define BLACKLIST_SIZE 2400
/* the event word with every bit set */
#define EVENTS_ALL 0xffffffff

/* what open returns where it fails */
#define DTE_OPEN_NO_DDAL 0x04 /* the host gave no struct ddal_api */
#define DTE_OPEN_NO_PUMP 0x40 /* the data pump failed target_test_dp */
#define DTE_OPEN_NO_PORT 0x200 /* the port's SCC_PC was null */

static char dteapi_take_tick(void);
static void dteapi_update_line(struct dte_line *line);
static void dteapi_restore_blacklist(void);
static void dteapi_save_blacklist(void);

struct SCC_PC *glbScc_Pc;

/* the member exports these names, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ModemMainCall(void *arg);
Uint32 Open(struct _DTEDEV_OPEN *open);
Bool Close(struct _DTEDEV_CLOSE *close);
void SetEvent(struct _DTEDEV_SETEVENT *event);
void init_dteapi_cc_f(void);
void end_dteapi_cc_f(void);

struct ddal_api *ddalApi;
void *Host;
struct SCC_PC *Scc_Pc;
struct CommonMemoryInformation *Cmi;
struct CommonMemoryInformation CmiStatic;
struct SCC_PC Scc_Pc_Static;
struct dte_api Api;
struct dte_line McrShadow;
Uint8 ModemReady;
/* NOLINTEND(misc-use-internal-linkage) */

/* the library's version and build time */
static const char *const modem_build =
    "\nmodem Ver 1.0311 Build:Oct 02 2000 22:47:05\n";
/* the milliseconds ModemMainCall has counted and not yet run the timers
 * for */
static Uint32 dteapi_ms = 0;
/* 1 until an Open succeeds. until then, Open leaves the blacklist as it
 * is */
static Uint8 dteapi_first_open = 1;
/* the blacklist as the last Close found it */
static Uint8 dteapi_blacklist[BLACKLIST_SIZE];

struct dte_api *dteGetApi(void)
{
    Api.open = Open;
    Api.close = Close;
    Api.set_event = SetEvent;
    return &Api;
}

void ModemIntrCall(void *arg)
{
    dp_interrupt();
}

void ModemMainCall(void *arg)
{
    if (!ModemReady) {
        return;
    }
    dteapi_ms += VBLANK_MS;
    while (dteapi_take_tick()) {
        TCI_timer();
        vxd_esc_seq_timr();
        break_check();
    }
    MainPacingFunc();
    sysmainCall();
    SecondPacingFunc();
}

static char dteapi_take_tick(void)
{
    char due;

    /* @hack spelling: the quotient by TIMER_MS tested, and due set in each
     * arm, where an early return for dteapi_ms < TIMER_MS would do */
    if (dteapi_ms / TIMER_MS) {
        due = 1;
        dteapi_ms -= TIMER_MS;
    } else {
        due = 0;
    }
    return due;
}

Uint32 Open(struct _DTEDEV_OPEN *open)
{
    /* @hack redundant: spare, a local that only sizes the frame */
    Uint8 spare[4];
    struct _DTEDEV_SETEVENT mask;

    ModemReady = 0;
    Host = open->host;
    DspDbSetApi(open->opt);
    targetSetOptApi(open->opt);
    ddalApi = open->ddal;
    if (ddalApi == NULL) {
        return DTE_OPEN_NO_DDAL;
    }
    targetOpen();
    Cmi = &CmiStatic;
    Cmi->mem = &Buffer_memory;
    memset(&McrShadow, 0, sizeof(McrShadow));
    Scc_Pc = &Scc_Pc_Static;
    SCC_PC_SetCmi(Scc_Pc, Cmi);
    /* @hack spelling: the store in one arm and the return in the other, where
     * an early return for a null Scc_Pc would do */
    if (Scc_Pc != NULL) {
        glbScc_Pc = Scc_Pc;
    } else {
        return DTE_OPEN_NO_PORT;
    }
    SCC_PC_InitCom_Memory(Scc_Pc);
    if (!target_test_dp()) {
        Cmi = NULL;
        Scc_Pc = NULL;
        glbScc_Pc = NULL;
        return DTE_OPEN_NO_PUMP;
    }
    OpenPacing();
    sysmainOpen();
    dteapi_restore_blacklist();
    Scc_Pc->mem->event_mask_lo = 0;
    Scc_Pc->mem->event_mask_hi = 0;
    mask.event.all = EVENTS_ALL;
    ddalApi->set_mask(&mask);
    osifHookTimerIntr(1, ModemMainCall);
    dteapi_first_open = 0;
    return 0;
}

Bool Close(struct _DTEDEV_CLOSE *close)
{
    sysmainClose();
    Cmi = NULL;
    Scc_Pc = NULL;
    glbScc_Pc = NULL;
    osifHookTimerIntr(0, NULL);
    osifHookDataPumpIntr(0, NULL);
    targetClose();
    dteapi_save_blacklist();
    ModemReady = 0;
    return TRUE;
}

void SetEvent(struct _DTEDEV_SETEVENT *event)
{
    /* @hack redundant: the tests of tx and rx, which do nothing either
     * way */
    if (event->event.bit.tx) {
    }
    if (event->event.bit.rx) {
    }
    /* @hack spelling: the test of ModemReady nested, where && would do */
    if (event->event.bit.line) {
        if (ModemReady) {
            dteapi_update_line(ddalApi->get_line());
        }
    }
    /* @hack redundant: the tests of unk_bit28 and unk_bit27, which do
     * nothing either way */
    if (event->event.bit.unk_bit28) {
    }
    if (event->event.bit.unk_bit27) {
    }
    if (event->event.bit.flush_out) {
        glbScc_Pc->cmi->flush_out = 1;
    }
    if (event->event.bit.flush_in) {
        glbScc_Pc->cmi->flush_in = 1;
    }
    if (event->event.bit.ready) {
        /* @bug memory: ready sets ModemReady whatever Open returned. after an
         * Open that returned DTE_OPEN_NO_DDAL, the calls ModemReady guards
         * read through a null ddalApi. Open tests it at 0x0cadf9d6. SetEvent
         * reads through it at 0x0cadfb98, dteGetTxBuffer at 0x0cadfc1e,
         * dteSetRxBuffer at 0x0cadfc50 and dteSetModemStatusRegister at
         * 0x0cadfc66. the game sends ready only after an Open that returned
         * 0, at 0x0c9e8492 */
        ModemReady = 1;
    }
}

struct ddal_result *dteGetTxBuffer(struct ddalDataTransfer *transfer)
{
    static struct ddal_result result;
    static Uint32 made;
    struct ddal_result *none;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    if (transfer->length == 0 || !ModemReady) {
        result.length = 0;
        result.remaining = 1;
        /* @hack spelling: none, a copy of &result, where return &result would
         * do */
        none = &result;
        return none;
    }
    return ddalApi->get_tx(transfer);
}

struct ddal_result *dteSetRxBuffer(struct ddalDataTransfer *transfer)
{
    static struct ddal_result result;
    static Uint32 made;

    /* @hack redundant: made, a static the first call sets to 1, which
     * nothing reads */
    made ? (void)0 : (void)made++;
    if (!ModemReady) {
        result.length = 0;
        result.remaining = 0;
        return &result;
    }
    return ddalApi->set_rx(transfer);
}

void dteSetModemStatusRegister(struct dteModemStatusRegister *status)
{
    if (!ModemReady) {
        return;
    }
    ddalApi->set_status(status);
}

static void dteapi_update_line(struct dte_line *line)
{
    struct Md md;
    struct Mcr mcr;

    Md_ct(&md);
    if (line->mcr != McrShadow.mcr) {
        Mcr_ct(&mcr);
        McrShadow.mcr = line->mcr;
        *mcr.ref = line->mcr;
        SCC_PC_SetMCR(glbScc_Pc, &mcr);
        md.value.bits.mcr = 1;
        SCC_PC_SendInt(glbScc_Pc, &md);
    }
    if (line->lcr != McrShadow.lcr) {
        McrShadow.lcr = line->lcr;
        SCC_PC_SetLCR(glbScc_Pc, line->lcr);
        md.value.bits.lcr = 1;
        SCC_PC_SendInt(glbScc_Pc, &md);
    }
    if (line->baud != McrShadow.baud) {
        McrShadow.baud = line->baud;
        SCC_PC_Delta_BaudRate(glbScc_Pc, line->baud);
    }
}

static void dteapi_restore_blacklist(void)
{
    Uint8 *list;
    Uint16 size;

    if (dteapi_first_open) {
        return;
    }
    size = BL_GetBlackListBuffer(&list);
    memcpy(list, dteapi_blacklist, size);
}

static void dteapi_save_blacklist(void)
{
    Uint8 *list;
    Uint16 size;

    size = BL_GetBlackListBuffer(&list);
    memcpy(dteapi_blacklist, list, size);
}

void init_dteapi_cc_f(void)
{
    CommonMemoryInformation_ct(&CmiStatic);
    SCC_PC_ct_void(&Scc_Pc_Static);
}

void end_dteapi_cc_f(void)
{
    CommonMemoryInformation_dt(&CmiStatic);
}

/* the member's entries in DSG_INIT_ and DSG_END_. the Chao area calls one as
 * it starts, and the other as it ends */
#pragma section _INIT_
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*init_dteapicc)(void) = init_dteapi_cc_f;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section _END_
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*end_dteapicc)(void) = end_dteapi_cc_f;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section
